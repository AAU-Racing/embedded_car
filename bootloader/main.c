#include <stdint.h>
#include <stdbool.h>

#include <board_driver/uart.h>
#include <board_driver/flash.h>
#include <board_driver/init.h>


int main(void) {
	BSP_UART_init();

	while (1) {
		uint8_t c;
		BSP_UARTx_Receive(&c, 1);
		if (c == 'k') {
			BSP_UARTx_transmit((uint8_t[]){'y'}, 1);
			break;
		}
	}

	uint32_t start_address = 0x08010000;
	size_t len = 8132;
	uint8_t data[len];

	for (size_t i = 0; i < len; i++) {
		uint8_t c;
		BSP_UARTx_Receive(&c, 1);
		data[i] = c;
		BSP_UARTx_transmit((uint8_t[]){c}, 1);
	}

	if (write_flash(start_address, data, len)) {
		BSP_UARTx_transmit((uint8_t[]){"Error\n"}, 6);
		while(1);
	} else {
		BSP_UARTx_transmit((uint8_t[]){"flash done\n"}, 11);
	}

	BSP_UARTx_transmit((uint8_t[]){"Booting application...\n\n"}, 24);
	boot(start_address);
}
