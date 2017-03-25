#include <stdint.h>
#include <stdbool.h>

#include <board_driver/uart.h>


int main(void) {
	BSP_UART_init();

	int counter = 0;

	while (1) {
		uint8_t str[7] = "node  \n";
		str[5] = ++counter + '0';
		if (counter == 9) counter = 0;
		BSP_UARTx_transmit(str, 7);
		HAL_Delay(100);
	}
}
