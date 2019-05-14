#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <board_driver/uart.h>
#include "../../newlib_calls.h"

int main(void) {
	HAL_Delay(10);
	debug_uart_init(DASH_DEBUG_UART);
	printf("Starting\n\n");
	printf("Write something in the terminal and hit Enter:\n");

	while (1) {
		char input = getchar();
		printf("getchat: %c\n", input);
		HAL_Delay(100);
	}
}
