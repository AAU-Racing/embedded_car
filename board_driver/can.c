#include <stm32f4xx_hal.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "can.h"
#include "gpio.h"

#define MAX_FRAME_LEN 8
#define MAX_STD_ID    0x7FF
#define NUMBER_BANKS  28
#define BUFFER_SIZE   256

#define CAN_INSTANCE ((CAN_TypeDef* const []) { \
	CAN1, \
	CAN2, \
	CAN1, \
	CAN2, \
	CAN1, \
})

#define CAN_RX_PIN ((const GPIO_Pin const []) { \
	PIN_11, \
	PIN_5,	 \
	PIN_8,	 \
	PIN_12, \
	PIN_0,  \
})

#define CAN_RX_PORT ((GPIO_TypeDef* const []) { \
	GPIOA, \
	GPIOB, \
	GPIOB, \
	GPIOB, \
	GPIOD, \
})

#define CAN_TX_PIN ((const GPIO_Pin const []) { \
	PIN_12, \
	PIN_6,	 \
	PIN_9,	 \
	PIN_13, \
	PIN_1,  \
})

#define CAN_TX_PORT ((GPIO_TypeDef* const []) { \
	GPIOA, \
	GPIOB, \
	GPIOB, \
	GPIOB, \
	GPIOD, \
})

typedef struct {
	uint16_t StdId;
	uint8_t Length;
	uint8_t Msg[8];
} CAN_TransmitFrame;

// General variables for CAN
static CAN_TypeDef *handle;
static bool initialized = false;
static bool started = false;
static int filter_num = 0;
static volatile CAN_Statistics stats;

// Callback function stack
static volatile CAN_RX_Callback rx_callback[14];
static volatile uint8_t top = 0;

// Transmit queue
static volatile uint16_t head = 0;
static volatile uint16_t tail = 0;
static volatile uint16_t size = 0;
static volatile CAN_TransmitFrame transmit_buffer[BUFFER_SIZE];

/////////////////////
// Local functions //
/////////////////////
static void configure_gpio_pin(GPIO_TypeDef *port, GPIO_Pin pin) {
	gpio_af_init(port, pin, GPIO_HIGH_SPEED, GPIO_PULL_UP, GPIO_AF9);
}

static void naive_memcpy(volatile uint8_t *dst, volatile uint8_t *src, uint8_t size) {
	for (uint8_t i = 0; i < size; i++) {
		dst[i] = src[i];
	}
}

//////////////////////
// Public interface //
//////////////////////
CAN_Statistics CAN_GetStats() {
	return stats;
}

uint8_t CAN_Filter(uint16_t id, uint16_t mask, CAN_RX_Callback callback) {
	// Validate input
	if (id > MAX_STD_ID) {
		return CAN_INVALID_ID;
	}

	if (mask > MAX_STD_ID) {
		return CAN_INVALID_ID;
	}

	if (!initialized) {
		return CAN_DRIVER_ERROR;
	}

	// Set callback function for that specific filter
	rx_callback[top++] = callback;

	// Enter filter initialization mode
	SET_BIT(handle->FMR, CAN_FMR_FINIT);

	// Set id and mask
	handle->sFilterRegister[filter_num].FR1 = id << 21; // FR1[31:21]
	handle->sFilterRegister[filter_num].FR2 = mask << 21; // FR2[31:21]

	// Activate filter
	SET_BIT(handle->FA1R, 1 << filter_num);

	// Leave filter initialization mode
	CLEAR_BIT(handle->FMR, CAN_FMR_FINIT);

	// Increment filter number for next filter
	filter_num++;

	return CAN_OK;
}

uint8_t enter_init() {
    SET_BIT(handle->MCR, CAN_MCR_INRQ); // Request entering initialisation

	uint32_t tickstart = HAL_GetTick();

	while((handle->MSR & CAN_MSR_INAK) != CAN_MSR_INAK) // Wait for initialisation
    {
		if((HAL_GetTick() - tickstart ) > 10U) // Wait up to 10 ms then timeout
		{
			return CAN_INIT_TIMEOUT;
		}
    }

	return CAN_OK;
}

uint8_t leave_init() {
    CLEAR_BIT(handle->MCR, CAN_MCR_INRQ); // Request exiting initialisation

	uint32_t tickstart = HAL_GetTick();

	while((handle->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) // Wait for leaving initialisation
    {
		if((HAL_GetTick() - tickstart ) > 10U) // Wait up to 10 ms then timeout
		{
			return CAN_INIT_TIMEOUT;
		}
    }

	return CAN_OK;
}

uint8_t CAN_Init(uint8_t config) {
	// Configure the CAN peripheral
	handle = CAN_INSTANCE[config];

	// Start clock for both CAN interfaces
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN1EN);
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN2EN);

	// Exit sleep mode
    CLEAR_BIT(handle->MCR, CAN_MCR_SLEEP);

	// Enter initialisation mode
	if (enter_init() == CAN_INIT_TIMEOUT) {
		return CAN_INIT_TIMEOUT;
	}

	CLEAR_BIT(handle->MCR, CAN_MCR_TTCM); // Disable time triggered communication (which is just counter of when reception has occured)
	SET_BIT(handle->MCR, CAN_MCR_ABOM); // Enable automatic bus off management aka. try to rejoin the bus when bus becomes off
	SET_BIT(handle->MCR, CAN_MCR_AWUM); // Enable automatic wakeup
	CLEAR_BIT(handle->MCR, CAN_MCR_NART); // Enable automatic retransmission aka. retransmit if an error occours during transmission
	CLEAR_BIT(handle->MCR, CAN_MCR_RFLM); // Diable receive FIFO locked mode (disable means overwriting when receiving a new message)
	CLEAR_BIT(handle->MCR, CAN_MCR_TXFP); // Enable transmit priority (Choose the most important of the three transmit mailboxes based on ID)
	CLEAR_BIT(handle->BTR, CAN_BTR_SILM); // Normal mode
	CLEAR_BIT(handle->BTR, CAN_BTR_LBKM); // Disable loopback

	// 500 kbps with 87.5% sample point

	// AHBCLK = SYSCLOCK / 1 = 168 / 1 MHz = 168 MHz
	// APB1CLK = AHBCLK / 1 = 168 / 1 MHz = 168 MHz

	// Length of 1 tq = prescaler * (1 / APB1CLK)
	// NBT = 1 / bitrate
	// TQ per bit = NBT / 1 tq

	MODIFY_REG(handle->BTR, CAN_BTR_SJW_Msk, 0 << CAN_BTR_SJW_Pos); // Set synchronization jump width to 1 TQ (BTR[25:24] + 1)
	MODIFY_REG(handle->BTR, CAN_BTR_TS1_Msk, 12 << CAN_BTR_TS1_Pos); // Set time segment 1 to 13 TQ (BTR[19:16] + 1)
	MODIFY_REG(handle->BTR, CAN_BTR_TS2_Msk, 1 << CAN_BTR_TS2_Pos); // Set time segment 2 to 2 TQ (BTR[22:20] + 1)
	MODIFY_REG(handle->BTR, CAN_BTR_BRP_Msk, 20 << CAN_BTR_BRP_Pos); // Set baud rate prescaler to 21 (BTR[9:0] + 1)

	// Configure pins
	configure_gpio_pin(CAN_RX_PORT[config], CAN_RX_PIN[config]);
	configure_gpio_pin(CAN_TX_PORT[config], CAN_TX_PIN[config]);

	// Set lower filter number
	if (handle == CAN1) {
		filter_num = 0;
	}
	else {
		filter_num = 14;
	}

	// Enter filter initialization mode
	SET_BIT(handle->FMR, CAN_FMR_FINIT);

	handle->FFA1R = 0; // Set all filters to FIFO 0
	handle->FS1R = 0x0FFFFFFF; // Set all filters to 32-bit single filter
	handle->FM1R = 0; // Set all filters to mask mode

	MODIFY_REG(handle->FMR, CAN_FMR_CAN2SB_Msk, 14 << CAN_FMR_CAN2SB_Pos); // Assign the first 14 filter banks to CAN1 and the last 14 to CAN2

	// Leave filter initialization mode
	CLEAR_BIT(handle->FMR, CAN_FMR_FINIT);

	// Enable interrupt vectors
	// RX fifo 0 interrupt (the only fifo we use)
	NVIC_SetPriority(handle == CAN1 ? CAN1_RX0_IRQn : CAN2_RX0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x0, 0x0));
	NVIC_EnableIRQ(handle == CAN1 ? CAN1_RX0_IRQn : CAN2_RX0_IRQn);

	// TX interrupt
	NVIC_SetPriority(handle == CAN1 ? CAN1_TX_IRQn : CAN2_TX_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x0, 0x0));
	NVIC_EnableIRQ(handle == CAN1 ? CAN1_TX_IRQn : CAN2_TX_IRQn);

	// Status change/error interrupt
	NVIC_SetPriority(handle == CAN1 ? CAN1_SCE_IRQn : CAN2_SCE_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x0, 0x0));
	NVIC_EnableIRQ(handle == CAN1 ? CAN1_SCE_IRQn : CAN2_SCE_IRQn);

	// Enable error interrupts
	SET_BIT(handle->IER, CAN_IER_ERRIE); // Allow error interrupts
	SET_BIT(handle->IER, CAN_IER_LECIE); // Enable last error code interrupt (error code for last message if an error occoured)
	SET_BIT(handle->IER, CAN_IER_BOFIE); // Enable bus-off error interrupt
	SET_BIT(handle->IER, CAN_IER_EWGIE); // Enable error warning interrupt
	SET_BIT(handle->IER, CAN_IER_EPVIE); // Enable error passive interrupt

	// Enable tx complete interrupts
	SET_BIT(handle->IER, CAN_IER_TMEIE);

	// Go into active mode
	if (leave_init() == CAN_INIT_TIMEOUT) {
		return CAN_INIT_TIMEOUT;
	}

	// Initialization was successful
	initialized = true;

	return CAN_OK;
}

uint8_t CAN_Start() {
	if (!initialized) {
		return CAN_DRIVER_ERROR;
	}

	// Start reception by enabling rx pending interrupt
	SET_BIT(handle->IER, CAN_IER_FMPIE0);

	started = true;

	return CAN_OK;
}

uint8_t CAN_Send(uint16_t id, uint8_t msg[], uint8_t length) {
	// Validate input
	if (id > MAX_STD_ID) {
		return CAN_INVALID_ID;
	}

	if (length > MAX_FRAME_LEN) {
		return CAN_INVALID_FRAME;
	}

	if (!started) {
		return CAN_DRIVER_ERROR;
	}

	// Select mailbox
	uint8_t transmitmailbox = 255;

	// Check if transmit mailbox is empty
    if ((handle->TSR & CAN_TSR_TME0) == CAN_TSR_TME0) { // TME0 = Transmit mailbox 0 empty
		transmitmailbox = 0;
	}
    else if ((handle->TSR & CAN_TSR_TME1) == CAN_TSR_TME1) { // TME1 = Transmit mailbox 1 empty
		transmitmailbox = 1;
	}
	else if ((handle->TSR & CAN_TSR_TME2) != CAN_TSR_TME2) { // TME2 = Transmit mailbox 2 empty
		transmitmailbox = 2;
	}
	else {
		// No mailbox is empty
		if (size < BUFFER_SIZE) { // Is queue full?
			// Copy to queue
			transmit_buffer[head].StdId = id;
			transmit_buffer[head].Length = length;
			naive_memcpy(transmit_buffer[tail].Msg, msg, length);

			// Increment queue head and size
			tail = (tail + 1) % BUFFER_SIZE;
			size++;

			return CAN_OK; // Queued for transmission
		}
		else {
			return CAN_BUFFER_FULL; // Cannot queue
		}
	}

	handle->sTxMailBox[transmitmailbox].TIR = id << 21; // EXID = 0, IDE = 0, RTR = 0, TXRQ = 0, STID = id
	handle->sTxMailBox[transmitmailbox].TDTR &= length; // Set DLC

	naive_memcpy((uint8_t*) &handle->sTxMailBox[transmitmailbox].TDLR, msg, length); // This will fill both TDLR and TDHR because offset for TDLR and TDHR is 8 and 12 relative to mailbox base address.

	// Request transmission
	SET_BIT(handle->sTxMailBox[transmitmailbox].TIR, CAN_TI0R_TXRQ);

	return CAN_OK;
}

////////////////////////
// Callback functions //
////////////////////////
void CAN_TxCallback() {
	stats.transmit++;

	// If queue is non-empty
	if (size > 0) {
		// Select mailbox
		uint8_t transmitmailbox = 255;

		// Select empty transmit mailbox
	    if ((handle->TSR & CAN_TSR_TME0) == CAN_TSR_TME0) { // TME0 = Transmit mailbox 0 empty
			transmitmailbox = 0;
		}
	    else if ((handle->TSR & CAN_TSR_TME1) == CAN_TSR_TME1) { // TME1 = Transmit mailbox 1 empty
			transmitmailbox = 1;
		}
		else if ((handle->TSR & CAN_TSR_TME2) != CAN_TSR_TME2) { // TME2 = Transmit mailbox 2 empty
			transmitmailbox = 2;
		}

		// If transmit mailbox actually is empty
		if (transmitmailbox != 255) {
			handle->sTxMailBox[transmitmailbox].TIR = transmit_buffer[head].StdId << 21; // EXID = 0, IDE = 0, RTR = 0, TXRQ = 0, STID = id
			handle->sTxMailBox[transmitmailbox].TDTR &= transmit_buffer[head].Length; // Set DLC

			naive_memcpy((uint8_t*) &handle->sTxMailBox[transmitmailbox].TDLR, transmit_buffer[head].Msg, transmit_buffer[head].Length); // This will fill both TDLR and TDHR because offset for TDLR and TDHR is 8 and 12 relative to mailbox base address.

			// Request transmission
			SET_BIT(handle->sTxMailBox[transmitmailbox].TIR, CAN_TI0R_TXRQ);

			// Increment tail for next item and decrement size to indicate removal.
			tail = (tail + 1) % BUFFER_SIZE;
			size--;
		}
	}
}

void CAN_RxCallback() {
	stats.receive++;

	CAN_RxFrame frame;

	// Get FMI
	frame.FMI = (handle->sFIFOMailBox[0].RDTR && CAN_RDT0R_FMI_Msk) >> CAN_RDT0R_FMI_Pos;

	// Get Std Id
	frame.StdId = (handle->sFIFOMailBox[0].RIR && CAN_RI0R_STID_Msk) >> CAN_RI0R_STID_Pos;

	// Get DLC
	frame.Length = handle->sFIFOMailBox[0].RDTR & CAN_RDT0R_DLC_Msk;

	// Copy data from fifo to frame
	naive_memcpy(frame.Msg, (uint8_t*) &handle->sFIFOMailBox[0].RDLR, frame.Length);

	// Release output mailbox for fifo
	SET_BIT(handle->RF0R, CAN_RF0R_RFOM0);

	// Callback for filter
	rx_callback[frame.FMI](&frame);
}

void CAN_ErrorCallback() {
	// Increment total error count
	stats.error_total++;

	if (READ_BIT(handle->ESR, CAN_ESR_BOFF) == CAN_ESR_BOFF) { // If callback was due to bus off
		stats.error_bof++;
	}
	else if (READ_BIT(handle->ESR, CAN_ESR_EWGF) == CAN_ESR_EWGF) { // If callback was due to error warning flag
		stats.error_ewg++;
	}
	else if (READ_BIT(handle->ESR, CAN_ESR_EPVF) == CAN_ESR_EPVF) { // If callback was due to error passive flag
		stats.error_epv++;
	}
	else if ((handle->ESR & CAN_ESR_LEC) != 0) { // If message was due to message error flag
		switch ((handle->ESR & CAN_ESR_LEC) >> CAN_ESR_LEC_Pos) {
			case 1:
				stats.error_stuff++; // Incorrect bit stuffing which causes wrong synchronization
				break;
			case 2:
				stats.error_form++; // Wrong state of static bits (aka. frame form error). E.g. CRC delimiter, ACK delimiter, End of Frame
				break;
			case 3:
				stats.error_ack++; // No acknowledge on transmit
				break;
			case 4:
				stats.error_recess++; // During any transmission, the controller wanted to transmit a recessive bit (1/true) but observed a dominant bit (0/false)
				break;
			case 5:
				stats.error_dominant++; // During any transmission, the controller wanted to transmit a dominant bit (0/false) but observed a recessive bit (1/true)
				break;
			case 6:
				stats.error_crc++; // Error in CRC value of received message
				break;
		}
	}

	// Clear general error interrupt flag
	CLEAR_BIT(handle->MSR, CAN_MSR_ERRI);
}

// Interrupts
void CAN1_RX0_IRQHandler() {
	CAN_RxCallback();
}
void CAN2_RX0_IRQHandler() {
	CAN_RxCallback();
}
void CAN1_TX_IRQHandler() {
	CAN_TxCallback();
}
void CAN2_TX_IRQHandler() {
	CAN_TxCallback();
}
void CAN1_SCE_IRQHandler() {
	CAN_ErrorCallback();
}
void CAN2_SCE_IRQHandler() {
	CAN_ErrorCallback();
}
