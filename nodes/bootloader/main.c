#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <board_driver/uart.h>
#include <board_driver/flash.h>
#include <board_driver/init.h>

int main(void) {
	HAL_Delay(10);
	debug_uart_init(DASH_DEBUG_UART);
	printf("Booting application\n\n");
	HAL_Delay(1000);
	boot(0x08010000);
}
