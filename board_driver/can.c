#include <stm32f4xx_hal.h>
#include <string.h>
#include <stdio.h>

#include "can.h"

#define MAX_FRAME_LEN 8
#define MAX_STD_ID 0x7FF
#define NUMBER_BANKS 28

typedef struct {
	CAN_Frame array[20];
	uint8_t count;
} Buffer;

CAN_HandleTypeDef CanHandle;
volatile CAN_Statistics stats;
volatile Buffer receive_buffer;
volatile Buffer transmit_buffer;

/////////////////////
// Local functions //
/////////////////////
static void Error_Handler(CAN_HandleTypeDef* hcan) {
	uint32_t state = HAL_CAN_GetError(hcan);
	stats.error_total++;
	switch(state) {
		case HAL_CAN_ERROR_EWG:
		stats.error_ewg++;
		break;
		case HAL_CAN_ERROR_EPV:
		stats.error_epv++;
		break;
		case HAL_CAN_ERROR_BOF:
		stats.error_bof++;
		break;
		case HAL_CAN_ERROR_STF:
		stats.error_stuff++;
		break;
		case HAL_CAN_ERROR_FOR:
		stats.error_form++;
		break;
		case HAL_CAN_ERROR_ACK:
		stats.error_ack++;
		break;
		case HAL_CAN_ERROR_BR:
		stats.error_recess++;
		break;
		case HAL_CAN_ERROR_BD:
		stats.error_dominant++;
		break;
		case HAL_CAN_ERROR_CRC:
		stats.error_crc++;
		break;
	}
}

static void Init_Handle() {
	static CanTxMsgTypeDef        TxMessage;
	static CanRxMsgTypeDef        RxMessage;

	// Configure the CAN peripheral
	CanHandle.Instance = CANx;
	CanHandle.pTxMsg = &TxMessage;
	CanHandle.pRxMsg = &RxMessage;

	CanHandle.Init.TTCM = DISABLE; // Time triggered communication
	CanHandle.Init.ABOM = ENABLE; // Try to rejoin the bus when bus becomes off
	CanHandle.Init.AWUM = DISABLE; // Automatic wakeup
	CanHandle.Init.NART = DISABLE; // Non-automatic retransmission
	CanHandle.Init.RFLM = DISABLE; // Receive FIFO locked mode (disable means overwriting when receiving a new message)
	CanHandle.Init.TXFP = ENABLE; // Transmit FIFO priority (Choose the most important of the three transmit mailboxes)
	CanHandle.Init.Mode = CAN_MODE_NORMAL;

	/* 500 kbps with 87.5% sample point */
	CanHandle.Init.SJW = CAN_SJW_1TQ;
	CanHandle.Init.BS1 = CAN_BS1_13TQ;
	CanHandle.Init.BS2 = CAN_BS2_2TQ;
	CanHandle.Init.Prescaler = 21;
	// AHBCLK = SYSCLOCK / 1 = 168 / 1 MHz = 168 MHz
	// APB1CLK = AHBCLK / 1 = 168 / 1 MHz = 168 MHz

	// Length of 1 tq = prescaler * (1 / APB1CLK)
	// NBT = 1 / bitrate
	// TQ per bit = NBT / 1 tq

	if(HAL_CAN_Init(&CanHandle) != HAL_OK) {
		Error_Handler(&CanHandle); // Initialization Error
	}

	// Configure Transmission data
	CanHandle.pTxMsg->StdId = 0x01;
	CanHandle.pTxMsg->ExtId = 0x01;
	CanHandle.pTxMsg->RTR = CAN_RTR_DATA;
	CanHandle.pTxMsg->IDE = CAN_ID_STD;
	CanHandle.pTxMsg->DLC = 0;
}

static void safe_memcpy(volatile void *arr1, volatile void *arr2, size_t bytes) {
	for (size_t i = 0; i < bytes; i++) {
		*((uint8_t*) arr1 + i) = *((uint8_t*) arr2 + i);
	}
}

//////////////////////
// Public interface //
//////////////////////
uint8_t CAN_Filter(uint16_t id, uint16_t mask) {
	static int filter_num = CANx == CAN1 ? 0 : 14;
	static int init = 0;
	assert_param(id <= MAX_STD_ID);
	assert_param(mask <= MAX_STD_ID);
	assert_param(filter_num + 1 < NUMBER_BANKS);
	// If not initialized
	if (!init) {
		// Reset statistics
		for (size_t i = 0; i < sizeof(CAN_Statistics); i++) {
			*((uint8_t*) &stats + i) = 0;
		}
		// Initialize handle
		Init_Handle();
		// And now it is initialized
		init = 1;
	}

	if(HAL_CAN_ConfigFilter(&CanHandle, &(CAN_FilterConfTypeDef) {
		.FilterNumber         = filter_num++,
		.FilterMode           = CAN_FILTERMODE_IDMASK,
		.FilterScale          = CAN_FILTERSCALE_32BIT,
		.FilterIdHigh         = id << 5,
		.FilterIdLow          = 0,
		.FilterMaskIdHigh     = mask << 5,
		.FilterMaskIdLow      = 0,
		.FilterFIFOAssignment = 0,
		.FilterActivation     = ENABLE,
	}) != HAL_OK) {
		Error_Handler(&CanHandle); 	// Filter configuration Error
	}

	return filter_num - 1;
}

void CAN_Start() {
	// Start reception
	if (HAL_CAN_Receive_IT(&CanHandle, CAN_FIFO0) != HAL_OK) {
		Error_Handler(&CanHandle);
	}
}

bool CAN_Receive(uint8_t filter_num, CAN_Frame* output_frame) {
	assert_param(filter_num < NUMBER_BANKS);
	*output_frame = EMPTY_FRAME;
	// Loop through buffer
	for (int i = 0; i < receive_buffer.count; i++) {
		// Until meeting correct response
		if (receive_buffer.array[i].Filter == filter_num) {
			// Copy frame
			*output_frame = receive_buffer.array[i];
			// Decrease number of responses in receive_buffer and move remaining entries down
			receive_buffer.count--;
			for (int j = i; j < receive_buffer.count; j++) {
				receive_buffer.array[j] = receive_buffer.array[j + 1];
			}
			return true;
		}
	}

	return false;
}

void CAN_Send(uint16_t id, uint8_t msg[], uint8_t length) {
	assert_param(length <= MAX_FRAME_LEN);
	assert_param(id <= MAX_STD_ID);
	// Set transmit parameters
	CanHandle.pTxMsg->StdId = id;
	CanHandle.pTxMsg->DLC = length;
	// Copy data
	for (uint8_t i = 0; i < MAX_FRAME_LEN; i++) {
		CanHandle.pTxMsg->Data[i] = (i < length ? msg[i] : 0);
	}
	// Transmit with interrupts
	if (HAL_CAN_Transmit_IT(&CanHandle) != HAL_OK) {
		CAN_Frame transmit_frame;
		transmit_frame.Id = id;
		transmit_frame.Dlc = length;
		safe_memcpy(transmit_frame.Msg, msg, length);
		transmit_buffer.array[transmit_buffer.count++] = transmit_frame;
	}
}

////////////////////////
// Callback functions //
////////////////////////
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan) {
	(void) hcan;
	stats.transmit++;
	/* Move from queue to transmit mailboxes */
	if (transmit_buffer.count > 0) {
		CanHandle.pTxMsg->StdId = transmit_buffer.array[transmit_buffer.count].Id;
		transmit_buffer.count--;
		CanHandle.pTxMsg->DLC = transmit_buffer.array[transmit_buffer.count].Dlc;
		// Copy data
		for (uint8_t i = 0; i < MAX_FRAME_LEN; i++) {
			CanHandle.pTxMsg->Data[i] =
			(i < transmit_buffer.array[transmit_buffer.count].Dlc ?
				transmit_buffer.array[transmit_buffer.count].Msg[i] :
				0);
		}
		// Transmit with interrupts
		if (HAL_CAN_Transmit_IT(&CanHandle) != HAL_OK) {
			transmit_buffer.count++;
		}
	}
}


void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan) {
	stats.receive++;
	CAN_Frame received; // Create local frame variable
	received.Id = hcan->pRxMsg->StdId; // Copy ID
	received.Dlc = hcan->pRxMsg->DLC; // Copy DLC
	received.Filter = hcan->pRxMsg->FMI;
	safe_memcpy(received.Msg, hcan->pRxMsg->Data, 8); // Copy data to frame variable
	receive_buffer.array[receive_buffer.count++] = received; // Insert into buffer

	if(HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK)
	Error_Handler(hcan);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
	Error_Handler(hcan);
}

/////////////////////////
// MSP init and deinit //
/////////////////////////
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan) {
	(void) hcan;
	CANx_CLK_ENABLE(); // Enable CAN clock
	CANx_GPIO_CLK_ENABLE(); // Enable GPIO clock (CAN-pins)

	// Initialize pins
	HAL_GPIO_Init(CANx_TX_GPIO_PORT, &(GPIO_InitTypeDef) {
		.Pin       = CANx_TX_PIN,
		.Mode      = GPIO_MODE_AF_PP,
		.Speed     = GPIO_SPEED_FAST,
		.Pull      = GPIO_PULLUP,
		.Alternate = CANx_TX_AF,
	});

	HAL_GPIO_Init(CANx_RX_GPIO_PORT, &(GPIO_InitTypeDef) {
		.Pin       = CANx_RX_PIN,
		.Mode      = GPIO_MODE_AF_PP,
		.Speed     = GPIO_SPEED_FAST,
		.Pull      = GPIO_PULLUP,
		.Alternate = CANx_RX_AF,
	});

	// Set interrupt priority
	HAL_NVIC_SetPriority(CANx_RX_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(CANx_RX_IRQn);

	HAL_NVIC_SetPriority(CANx_TX_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(CANx_TX_IRQn);
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan) {
	(void) hcan;
	// Release
	CANx_FORCE_RESET();
	CANx_RELEASE_RESET();

	// Deinitialize pins
	HAL_GPIO_DeInit(CANx_TX_GPIO_PORT, CANx_TX_PIN);
	HAL_GPIO_DeInit(CANx_RX_GPIO_PORT, CANx_RX_PIN);

	// Stop interrupts
	HAL_NVIC_DisableIRQ(CANx_RX_IRQn);
	HAL_NVIC_DisableIRQ(CANx_TX_IRQn);
}
