#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <board_driver/i2c.h>
#include <board_driver/can.h>

int main(void) {
	// uart_init();
	printf("UART init complete\n");
	HAL_Delay(100);

	i2c_init();
	printf("i2c init complete\n");

	for (uint8_t i = 0; ; i++) {
	 	i2c_master_transmits(17, &i, 1);
		printf("Transmit complete\n");

		HAL_Delay(100);
	}
}
