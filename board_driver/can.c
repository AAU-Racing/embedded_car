 #include <stm32f4xx_hal.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "can.h"

#define MAX_FRAME_LEN 8
#define MAX_STD_ID 0x7FF
#define NUMBER_BANKS 28
#define BUFFER_SIZE 256

#define CAN_INSTANCE ((CAN_TypeDef* const []) { \
	CAN1, \
	CAN2, \
	CAN1, \
	CAN2, \
	CAN1, \
})

#define CAN_RX_PIN ((const uint32_t const []) { \
	GPIO_PIN_11, \
	GPIO_PIN_5,	 \
	GPIO_PIN_8,	 \
	GPIO_PIN_12, \
	GPIO_PIN_0,  \
})

#define CAN_RX_PORT ((GPIO_TypeDef* const []) { \
	GPIOA, \
	GPIOB, \
	GPIOB, \
	GPIOB, \
	GPIOD, \
})

#define CAN_TX_PIN ((const uint32_t const []) { \
	GPIO_PIN_12, \
	GPIO_PIN_6,	 \
	GPIO_PIN_9,	 \
	GPIO_PIN_13, \
	GPIO_PIN_1,  \
})

#define CAN_TX_PORT ((GPIO_TypeDef* const []) { \
	GPIOA, \
	GPIOB, \
	GPIOB, \
	GPIOB, \
	GPIOD, \
})

CAN_HandleTypeDef can_handle;
volatile CAN_Statistics stats;
static int filter_num = 0;
static volatile CAN_RX_Callback rx_callback[14];
static uint8_t top = 0;
static volatile uint16_t head = 0;
static volatile uint16_t tail = 0;
static volatile uint16_t size = 0;
static volatile CanTxMsgTypeDef transmit_buffer[BUFFER_SIZE];

/////////////////////
// Local functions //
/////////////////////
static void error_handler(CAN_HandleTypeDef* hcan) {
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

static void init_gpio_clock(GPIO_TypeDef *port) {
	if (port == GPIOA) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}
	else if (port == GPIOB) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}
	else if (port == GPIOC) {
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}
	else if (port == GPIOD) {
		__HAL_RCC_GPIOD_CLK_ENABLE();
	}
	else if (port == GPIOE) {
		__HAL_RCC_GPIOE_CLK_ENABLE();
	}
	else if (port == GPIOF) {
		__HAL_RCC_GPIOF_CLK_ENABLE();
	}
	else if (port == GPIOG) {
		__HAL_RCC_GPIOG_CLK_ENABLE();
	}
	else if (port == GPIOH) {
		__HAL_RCC_GPIOH_CLK_ENABLE();
	}
	else if (port == GPIOI) {
		__HAL_RCC_GPIOI_CLK_ENABLE();
	}
}

static void configure_gpio_pin(GPIO_TypeDef *port, uint32_t pin) {
	init_gpio_clock(port);

	HAL_GPIO_Init(port, &(GPIO_InitTypeDef) {
		.Pin       = pin,
		.Mode      = GPIO_MODE_AF_PP,
		.Speed     = GPIO_SPEED_FAST,
		.Pull      = GPIO_PULLUP,
		.Alternate = GPIO_AF9_CAN1, // CAN1 and CAN2 uses the same Alternate Function (AF9), so init using one.
	});
}

static void configure_pins(uint8_t config) {
	configure_gpio_pin(CAN_RX_PORT[config], CAN_RX_PIN[config]);
	configure_gpio_pin(CAN_TX_PORT[config], CAN_TX_PIN[config]);
}

//////////////////////
// Public interface //
//////////////////////
CAN_StatusTypeDef CAN_Filter(uint16_t id, uint16_t mask, CAN_RX_Callback callback) {
	if (id > MAX_STD_ID) {
		return CAN_INVALID_ID;
	}

	if (mask > MAX_STD_ID) {
		return CAN_INVALID_ID;
	}

	rx_callback[top++] = callback;

	HAL_StatusTypeDef result = HAL_CAN_ConfigFilter(&can_handle, &(CAN_FilterConfTypeDef) {
		.FilterNumber         = filter_num++,
		.FilterMode           = CAN_FILTERMODE_IDMASK,
		.FilterScale          = CAN_FILTERSCALE_32BIT,
		.FilterIdHigh         = id << 5,
		.FilterIdLow          = 0,
		.FilterMaskIdHigh     = mask << 5,
		.FilterMaskIdLow      = 0,
		.FilterFIFOAssignment = CAN_FILTER_FIFO0,
		.FilterActivation     = ENABLE,
		.BankNumber			  = 14,
	});

	if (result != HAL_OK) {
		error_handler(&can_handle); 	// Filter configuration Error
		return CAN_DRIVER_ERROR;
	}

	return CAN_OK;
}

CAN_StatusTypeDef CAN_Init(uint8_t config) {
	static CanTxMsgTypeDef        TxMessage;
	static CanRxMsgTypeDef        RxMessage;

	// Configure the CAN peripheral
	can_handle.Instance = CAN_INSTANCE[config];
	can_handle.pTxMsg = &TxMessage;
	can_handle.pRxMsg = &RxMessage;

	can_handle.Init.TTCM = DISABLE; // Time triggered communication
	can_handle.Init.ABOM = ENABLE; // Try to rejoin the bus when bus becomes off
	can_handle.Init.AWUM = ENABLE; // Automatic wakeup
	can_handle.Init.NART = DISABLE; // Non-automatic retransmission
	can_handle.Init.RFLM = DISABLE; // Receive FIFO locked mode (disable means overwriting when receiving a new message)
	can_handle.Init.TXFP = DISABLE; // Transmit FIFO priority (Choose the most important of the three transmit mailboxes)
	can_handle.Init.Mode = CAN_MODE_NORMAL;

	/* 500 kbps with 87.5% sample point */
	can_handle.Init.SJW = CAN_SJW_1TQ;
	can_handle.Init.BS1 = CAN_BS1_13TQ;
	can_handle.Init.BS2 = CAN_BS2_2TQ;
	can_handle.Init.Prescaler = 21;

	configure_pins(config);
	filter_num = CAN_INSTANCE[config] == CAN1 ? 0 : 14;

	// AHBCLK = SYSCLOCK / 1 = 168 / 1 MHz = 168 MHz
	// APB1CLK = AHBCLK / 1 = 168 / 1 MHz = 168 MHz

	// Length of 1 tq = prescaler * (1 / APB1CLK)
	// NBT = 1 / bitrate
	// TQ per bit = NBT / 1 tq

	HAL_StatusTypeDef result = HAL_CAN_Init(&can_handle);

	if (result != HAL_OK) {
		error_handler(&can_handle); // Initialization Error
		return CAN_DRIVER_ERROR;
	}

	return CAN_OK;
}

CAN_StatusTypeDef CAN_Start() {
	// Start reception
	HAL_StatusTypeDef result = HAL_CAN_Receive_IT(&can_handle, CAN_FIFO0);

	if (result != HAL_OK) {
		error_handler(&can_handle);
		return CAN_DRIVER_ERROR;
	}

	return CAN_OK;
}

CAN_StatusTypeDef CAN_Send(uint16_t id, uint8_t msg[], uint8_t length) {
	if (id > MAX_STD_ID)
		return CAN_INVALID_ID;

	if (length > MAX_FRAME_LEN)
		return CAN_INVALID_FRAME;

	// Set transmit parameters
	can_handle.pTxMsg->StdId = id;
	can_handle.pTxMsg->ExtId = 0x01;
	can_handle.pTxMsg->RTR = CAN_RTR_DATA;
	can_handle.pTxMsg->IDE = CAN_ID_STD;
	can_handle.pTxMsg->DLC = length;
	// Copy data
	for (uint8_t i = 0; i < length; i++) {
		can_handle.pTxMsg->Data[i] = msg[i];
	}

	// Transmit with interrupts
	HAL_StatusTypeDef result = HAL_CAN_Transmit_IT(&can_handle);

	if (result != HAL_OK) {
		if (size < BUFFER_SIZE) {
			transmit_buffer[head] = *can_handle.pTxMsg;

			head = (head + 1) % BUFFER_SIZE;
			size++;
		}

		error_handler(&can_handle);
		return CAN_DRIVER_ERROR;
	}

	return CAN_OK;
}

////////////////////////
// Callback functions //
////////////////////////
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan) {
	(void) hcan;
	stats.transmit++;

	if (size > 0) {
		static CanTxMsgTypeDef transmit;
		transmit = transmit_buffer[tail];
		hcan->pTxMsg = &transmit;
		tail = (tail + 1) % BUFFER_SIZE;
		size--;
		HAL_CAN_Transmit_IT(hcan);
	}
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan) {
	stats.receive++;

	rx_callback[hcan->pRxMsg->FMI](hcan->pRxMsg);

	if(HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK) {
		error_handler(hcan);
	}
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
	error_handler(hcan);
}

/////////////////////////
// MSP init and deinit //
/////////////////////////
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan) {
	(void) hcan;
	__HAL_RCC_CAN1_CLK_ENABLE(); // Enable CAN clock
	__HAL_RCC_CAN2_CLK_ENABLE();

	// Set interrupt priority
	HAL_NVIC_SetPriority(hcan->Instance == CAN1 ? CAN1_RX0_IRQn : CAN2_RX0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(hcan->Instance == CAN1 ? CAN1_RX0_IRQn : CAN2_RX0_IRQn);

	HAL_NVIC_SetPriority(hcan->Instance == CAN1 ? CAN1_TX_IRQn : CAN2_TX_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(hcan->Instance == CAN1 ? CAN1_TX_IRQn : CAN2_TX_IRQn);
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan) {
	(void) hcan;
	// Release
	__HAL_RCC_CAN1_FORCE_RESET();
	__HAL_RCC_CAN1_RELEASE_RESET();
	__HAL_RCC_CAN2_FORCE_RESET();
	__HAL_RCC_CAN2_RELEASE_RESET();

	// Stop interrupts
	HAL_NVIC_DisableIRQ(hcan->Instance == CAN1 ? CAN1_RX0_IRQn : CAN2_RX0_IRQn);
	HAL_NVIC_DisableIRQ(hcan->Instance == CAN1 ? CAN1_TX_IRQn : CAN2_TX_IRQn);
}

void CAN1_RX0_IRQHandler(void) {
	HAL_CAN_IRQHandler(&can_handle);
}

void CAN2_RX0_IRQHandler(void) {
	HAL_CAN_IRQHandler(&can_handle);
}

void CAN1_RX1_IRQHandler(void) {
	HAL_CAN_IRQHandler(&can_handle);
}

void CAN2_RX1_IRQHandler(void) {
	HAL_CAN_IRQHandler(&can_handle);
}

void CAN1_TX_IRQHandler(void) {
	HAL_CAN_IRQHandler(&can_handle);
}

void CAN2_TX_IRQHandler(void) {
	HAL_CAN_IRQHandler(&can_handle);
}
