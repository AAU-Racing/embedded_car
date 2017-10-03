#include <stm32f4xx_hal.h>
#include <board_driver/uart.h>
// #include <shield_drivers/com_node/sd_setup.h>


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

extern PCD_HandleTypeDef hpcd; // TODO make a get function for this instead!?

#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
	HAL_PCD_IRQHandler(&hpcd);
}

// TODO are we missing the uart interupts here for using uart over usb?

/*void DMA2_Stream3_IRQHandler(void) {
	SD_DMA_Rx_IRQHandler();
}

void DMA2_Stream6_IRQHandler(void) {
	SD_DMA_Tx_IRQHandler();
}

void SDIO_IRQHandler(void) {
	SD_IRQHandler();
}
*/
