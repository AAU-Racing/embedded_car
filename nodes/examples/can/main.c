#include <stm32f4xx_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <board_driver/uart.h>
#include <board_driver/can.h>
#include <board_driver/obdii.h>

extern volatile CAN_Statistics stats;

int main(void) {
	BSP_UART_init();

	uint8_t filter_num = CAN_Filter(0, 0x7FF);
	CAN_Start();

	// The send a message, wait for response, and print that to a serial
	while(true) {
		CAN_Send(0x119, (uint8_t[]) {10, 15, 61}, 3);

		CAN_Frame received = EMPTY_FRAME;
		while (!CAN_Receive(filter_num, &received)) {
			printf("Wating\n");
		}

		// If receiving from the right Id
		if (received.Id == 0x120) {
			printf("ID: %d\n", received.Id);

			printf("DLC: %d\n", received.Dlc);
			for (int i; i < received.Dlc; i++)
				printf("%d", received.Msg[i]);

			printf("\n");
		}

		// And delay to slow things down a little
		HAL_Delay(100);
	}

	// while(true) {
	// 	CAN_Frame received;
	// 	while(!CAN_Receive(filter_num, &received));
	//
	// 	if (received.Id == 0x119) {
	// 		CAN_Send(0x120, (uint8_t[]) {received.Msg[0] + 5, received.Msg[1] - 10, received.Msg[2] % 3}, 3);
	// 	}
	// }
}
