#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <board_driver/crc.h>

int main(void) {
	uint32_t buf[] = {0x1B08C22B, 0xFD3F12B6};
	size_t n = 2;
	uint32_t result = 1234;
	uint32_t expect = 0x9CE01764;

	crc_init();
	printf("crc init complete\n");

	HAL_Delay(100);

	result = crc_calculate(buf, n);
	printf("crc calculate complete\n");

	printf("Result = %u\n", result);
	printf("Result expected = %u\n", );
}
