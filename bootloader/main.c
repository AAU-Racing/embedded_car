#include <stdint.h>
#include <stdbool.h>

#include <board_driver/uart.h>
#include <board_driver/flash.h>
#include <board_driver/init.h>


int main(void) {
	uart_init();

	while (1) {
		uint8_t c = uart_read_byte();
		if (c == 'k') {
			printf("y");
			break;
		}
	}

	uint32_t start_address = 0x08010000;
	size_t len = 8132;
	uint8_t data[len];

	for (size_t i = 0; i < len; i++) {
		uint8_t c = uart_read_byte();
		data[i] = c;
		printf("%c", c);
	}

	if (write_flash(start_address, data, len)) {
		printf("Error\n");
		while(1);
	} else {
		printf("flash done\n");
	}

	printf("Booting application...\n\n");
	boot(start_address);
}
