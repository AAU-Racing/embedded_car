#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <board_driver/uart.h>
#include <board_driver/can.h>
#include <board_driver/obdii.h>

void setup(void);
void loop(void);

// A main function in the philosophy of Arduino (setup + loop)
int main(void) {
	setup();

	while(1){
		loop();
	}

	return 0;
}

void setup(void){
	uart_init();
	printf("UART init complete\n");

	//Setting up CAN and it's filters
	if (CAN_Init(CAN_PB12) != CAN_OK) {
		//log_error(CAN_ERROR, "CAN init error");
	}

	OBDII_Init();

	if (CAN_Start() != CAN_OK) {
		//log_error(CAN_ERROR, "CAN start error");
	}

	// Wait for everything to be ready
	HAL_Delay(25);

	// Signal all others that the com node is ready
	if (CAN_Send(CAN_NODE_STARTED, (uint8_t[]) { CAN_NODE_COM_NODE_STARTED }, 1) != CAN_OK) {
		//log_error(CAN_ERROR, "CAN send error");
	}
}

void loop(void){
}
