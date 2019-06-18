#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <board_driver/uart.h>
#include "../../newlib_calls.h"

int main(void) {
	HAL_Delay(10);
	debug_uart_init(DEV_DEBUG_UART);
	printf("Starting\n\n");

	uart_init(DEV_TELEMETRY_UART);
	printf("Done starting the telemetry\n");

	char* hello = "Hello\n";
	uart_send_buf(DEV_TELEMETRY_UART, hello, 6);
	printf("Done sending byte\n");

	printf("Write something in the terminal and hit Enter:\n");

	while (1) {
		char input = getchar();
		printf("getchat: %c\n", input);
		HAL_Delay(100);
	}
}
