#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <board_driver/i2c_no_hal.h>

int main(void) {
	// Init uart and crc
	uart_init();
	printf("UART init complete\n");
	HAL_Delay(100);

	i2c_init();
	printf("i2c init complete\n");

	HAL_Delay(100);
	printf("(%08x, %08x, %08x)\n", I2C2->CR1, I2C2->OAR1, I2C2->OAR2);

	for (uint32_t i = 0; ; i++) {
		i2c_master_transmit(0x1234, &i, 4);
		printf("Transmit complete\n");

		HAL_Delay(100);
	}
}
