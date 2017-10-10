#ifndef CAN_GUARD
#define CAN_GUARD

#include <stm32f4xx_hal.h>
#include <stdbool.h>

typedef struct {
	uint8_t transmit;
	uint8_t receive;
	uint8_t error_total;
	uint8_t error_ewg; /* Error warning */
	uint8_t error_epv; /* Error passive (when transmit error counter, TEC, reaches 127) */
	uint8_t error_bof; /* Bus off (when TEC reaches 255) */
	uint8_t error_stuff; /* Bit stuffing error */
	uint8_t error_form; /* Form error (When fixed format bits are not correct)*/
	uint8_t error_ack; /* Acknowledgement error */
	uint8_t error_recess; /* Bit recessive error (if monitoring opposite bit than transmitted after RTR)*/
	uint8_t error_dominant; /* Bit dominant error (if monitoring opposite bit than transmitted after RTR)*/
	uint8_t error_crc; /* CRC error */
} CAN_Statistics;

typedef struct {
	uint16_t Id;
	uint8_t Msg[8];
	uint8_t Dlc;
	uint8_t Filter;
} CAN_Frame;

#define EMPTY_FRAME (CAN_Frame) {0}
#define CAN_PA11

//////////////////////////////////////////
// CAN1 RX: PA11
// CAN1 TX: PA12
/////////////////////////////////////////
#ifdef CAN_PA11

#define CANx    CAN1
#define CANx_CLK_ENABLE()         __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()

#define CANx_FORCE_RESET()        __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET()      __HAL_RCC_CAN1_RELEASE_RESET()

#define CANx_TX_PIN           GPIO_PIN_12
#define CANx_TX_GPIO_PORT     GPIOA
#define CANx_TX_AF            GPIO_AF9_CAN1
#define CANx_RX_PIN           GPIO_PIN_11
#define CANx_RX_GPIO_PORT     GPIOA
#define CANx_RX_AF            GPIO_AF9_CAN1

#define CANx_RX_IRQn          CAN1_RX0_IRQn
#define CANx_RX_IRQHandler    CAN1_RX0_IRQHandler

#define CANx_TX_IRQn          CAN1_TX_IRQn
#define CANx_TX_IRQHandler    CAN1_TX_IRQHandler

#endif

//////////////////////////////////////////
// CAN2 RX: PB5
// CAN2 TX: PB6
/////////////////////////////////////////
#ifdef CAN_PB5

#define CANx    CAN2
#define CANx_CLK_ENABLE()         do {__HAL_RCC_CAN1_CLK_ENABLE(); __HAL_RCC_CAN2_CLK_ENABLE();} while(0)
#define CANx_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()

#define CANx_FORCE_RESET()        __HAL_RCC_CAN2_FORCE_RESET()
#define CANx_RELEASE_RESET()      __HAL_RCC_CAN2_RELEASE_RESET()

#define CANx_TX_PIN           GPIO_PIN_6
#define CANx_TX_GPIO_PORT     GPIOB
#define CANx_TX_AF            GPIO_AF9_CAN2
#define CANx_RX_PIN           GPIO_PIN_5
#define CANx_RX_GPIO_PORT     GPIOB
#define CANx_RX_AF            GPIO_AF9_CAN2

#define CANx_RX_IRQn          CAN2_RX0_IRQn
#define CANx_RX_IRQHandler    CAN2_RX0_IRQHandler

#define CANx_TX_IRQn          CAN2_TX_IRQn
#define CANx_TX_IRQHandler    CAN2_TX_IRQHandler

#endif

//////////////////////////////////////////
// CAN1 RX: PB8
// CAN1 TX: PB9
/////////////////////////////////////////
#ifdef CAN_PB8

#define CANx    CAN1
#define CANx_CLK_ENABLE()         __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()

#define CANx_FORCE_RESET()        __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET()      __HAL_RCC_CAN1_RELEASE_RESET()

#define CANx_TX_PIN           GPIO_PIN_9
#define CANx_TX_GPIO_PORT     GPIOB
#define CANx_TX_AF            GPIO_AF9_CAN1
#define CANx_RX_PIN           GPIO_PIN_8
#define CANx_RX_GPIO_PORT     GPIOB
#define CANx_RX_AF            GPIO_AF9_CAN1

#define CANx_RX_IRQn          CAN1_RX0_IRQn
#define CANx_RX_IRQHandler    CAN1_RX0_IRQHandler

#define CANx_TX_IRQn          CAN1_TX_IRQn
#define CANx_TX_IRQHandler    CAN1_TX_IRQHandler

#endif

//////////////////////////////////////////
// CAN2 RX: PB12
// CAN2 TX: PB13
/////////////////////////////////////////
#ifdef CAN_PB12

#define CANx    CAN2
#define CANx_CLK_ENABLE()         do {__HAL_RCC_CAN1_CLK_ENABLE(); __HAL_RCC_CAN2_CLK_ENABLE();} while(0)
#define CANx_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()

#define CANx_FORCE_RESET()        __HAL_RCC_CAN2_FORCE_RESET()
#define CANx_RELEASE_RESET()      __HAL_RCC_CAN2_RELEASE_RESET()

#define CANx_TX_PIN           GPIO_PIN_13
#define CANx_TX_GPIO_PORT     GPIOB
#define CANx_TX_AF            GPIO_AF9_CAN2
#define CANx_RX_PIN           GPIO_PIN_12
#define CANx_RX_GPIO_PORT     GPIOB
#define CANx_RX_AF            GPIO_AF9_CAN2

#define CANx_RX_IRQn          CAN2_RX0_IRQn
#define CANx_RX_IRQHandler    CAN2_RX0_IRQHandler

#define CANx_TX_IRQn          CAN2_TX_IRQn
#define CANx_TX_IRQHandler    CAN2_TX_IRQHandler

#endif

void CAN_Send(uint16_t id, uint8_t msg[], uint8_t length);
bool CAN_Receive(uint8_t filter_num, CAN_Frame* output_frame);
uint8_t CAN_Filter(uint16_t id, uint16_t mask);
void CAN_Start();

#endif /* CAN_GUARD */
