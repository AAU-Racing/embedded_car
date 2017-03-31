#include <stm32f4xx_hal.h>
#include "uart.h"


/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

void NMI_Handler(void) {}

void HardFault_Handler(void) {
	while (1);
}

void MemManage_Handler(void) {
	while (1);
}

void BusFault_Handler(void) {
	while (1);
}

void UsageFault_Handler(void) {
	while (1);
}

void SVC_Handler(void) {}

void DebugMon_Handler(void) {}

void PendSV_Handler(void) {}

void SysTick_Handler(void) {
	HAL_IncTick();
}


/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) , for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f407xx.s).                                               */
/******************************************************************************/
