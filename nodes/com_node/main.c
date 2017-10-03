#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <board_driver/uart.h>
#include <board_driver/rtc.h>
#include <board_driver/can.h>
#include <board_driver/obdii.h>
#include <shield_drivers/dashboard/oil.h>
#include <shield_drivers/com_node/water_temperature.h>
#include <shield_drivers/com_node/current_clamps.h>
#include <shield_drivers/com_node/wheel_speed.h>
#include <shield_drivers/com_node/error.h>

#include "sd.h"
#include "telemetry.h"

static uint32_t last_obd_burst = 0;

void setup(void){
	uart_init();
	printf("UART init complete\n");

	(void)BSP_RTC_Init();

	init_sd();
	init_telemtry();

	//Setting up CAN and it's filters
	if (CAN_Init(CAN_PB12) != CAN_OK) {
		log_error(CAN_ERROR, "CAN init error");
	}

	oil_init();
	water_temp_init();
	wheel_speed_init();
	current_clamps_init();
	error_init();
	OBDII_Init();

	if (CAN_Start() != CAN_OK) {
		log_error(CAN_ERROR, "CAN start error");
	}

	// Wait for everything to be ready
	HAL_Delay(25);

	// Signal all others that the com node is ready
	if (CAN_Send(CAN_NODE_STARTED, (uint8_t[]) { CAN_NODE_COM_NODE_STARTED }, 1) != CAN_OK) {
		log_error(CAN_ERROR, "CAN send error");
	}
}

void loop(void){
	// Burst only every 50 ms
	if (HAL_GetTick() - last_obd_burst > 50) {
		OBDII_Burst();
		last_obd_burst = HAL_GetTick();
	}

	// Handle logging and telemetry which are the main responsability of the com node
	handle_logging();
	handle_telemetry();
	HAL_Delay(1);
}

// A main function in the philosophy of Arduino (setup + loop)
int main(void) {
	setup();

	while(1){
		loop();
	}

	return 0;
}
