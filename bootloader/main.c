#include <stdint.h>
#include <stdbool.h>

#include <board_driver/uart.h>
#include <board_driver/flash.h>
#include <board_driver/init.h>

#define APPSIZE 256000

int main(void) {
	BSP_UART_init();

	int counter = 0;
	bool update = false;

	while (1) {
		uint8_t c = 0;
		UARTx_read_buf(&c, 1, 1000);

		if (c == 'k') {
			UARTx_send_buf((uint8_t[]){'y'}, 1);
			update = true;
			break;
		}

		if(counter == 5) break;
		counter++;
	}

	uint32_t start_address = 0x08010000;

	if(update) {
		uint32_t len = 0;

		UARTx_read_buf((uint8_t *)&len, sizeof(len), 1000);

		if(len < APPSIZE) {
			uint8_t data[len];

			for (size_t i = 0; i < len; i++) {
				data[i] = UARTx_read_byte();
				UARTx_send_buf((uint8_t[]){data[i]}, 1);
			}

			if (write_flash(start_address, data, len)) {
				printf("Error\n");
				while(1);
			} else {
				printf("flash done\n");
			}
		}
	}

	printf("Booting application...\n\n");
	HAL_Delay(10);
	boot(start_address);
}
