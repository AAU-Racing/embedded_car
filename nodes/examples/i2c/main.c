#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <board_driver/i2c_no_hal.h>
#include <board_driver/can.h>

int main(void) {
	// uart_init();
	printf("UART init complete\n");
	HAL_Delay(100);

	if (CAN_Init(CAN_PD0) != CAN_OK) {
		printf("CAN init error\n");
	}

	if (CAN_Start() != CAN_OK){
		printf("CAN start error\n");
	}
	else {
		printf("CAN started\n");
	}

	i2c_init();
	printf("i2c init complete\n");
	CAN_Send(0x7fe, (uint8_t[]) { 2 }, 1);

	for (uint32_t i = 0; ; i++) {
		CAN_Send(0x7fe, (uint8_t[]) { 3 }, 1);
	 	i2c_master_transmit(17, (uint32_t[]) { 1 }, 4);
		CAN_Send(0x7fe, (uint8_t[]) { 4 }, 1);
		printf("Transmit complete\n");

		HAL_Delay(100);
	}
}
