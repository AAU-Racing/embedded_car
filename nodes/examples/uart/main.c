#include <stdint.h>
#include <stdbool.h>

#include <board_driver/uart.h>


int main(void) {
	BSP_UART_init();

	while (1) {
		uint8_t input = ' ';
		UARTx_read_byte(&input);
		uint8_t str[8] = "echo  \n\r";
		str[5] = input;
		UARTx_send_buf(str, 8);
		HAL_Delay(100);
	}
}
