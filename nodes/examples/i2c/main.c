#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <board_driver/i2c_no_hal.h>

int main(void) {
	// Init uart and crc
	uart_init();
	i2c_init();

	printf("i2c init complete\n");

	HAL_Delay(100);

	for (uint32_t i = 0; ; i++) {
		i2c_master_transmit(0x1234, &i, 4);
		printf("Transmit complete\n");
	}
}
