#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <board_driver/uart.h>
#include <board_driver/can.h>
#include <board_driver/obdii.h>
#include <board_driver/adc.h>

#include <shield_drivers/main_board/gear.h>
#include <shield_drivers/main_board/oil_pressure.h>
#include <shield_drivers/main_board/neutral.h>
#include <shield_drivers/main_board/water_temp.h>

#define DISABLE_ELECTRONIC_GEAR

void setup(void);
void loop(void);

static uint32_t last_oil_transmit = 0;
static uint32_t last_neutral_transmit = 0;
static uint32_t last_water_transmit = 0;

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
	printf("\n\n");
	printf("UART init complete\n");

	init_oil_pressure();
	printf("Oil pressure init complete\n");

	if (get_oil_pressure()) {
		printf("Oil pressure is now high\n");
	}
	else {
		printf("Oil pressure is now low\n");
	}

	neutral_switch_init();
	printf("Neutral switch init complete\n");

	init_adc(2);
	printf("ADC init complete\n");

	init_water_temp();
	printf("Water sensor init complete\n");

	start_adc();
	printf("ADC started\n");

	//Setting up CAN and it's filters
	if (can_init(CAN_PD0) != CAN_OK) {
		printf("Error initializing CAN\n");
	}
	else {
		printf("CAN init complete\n");
	}

#ifndef DISABLE_ELECTRONIC_GEAR
	init_gear();
	printf("Init gear complete\n");
#endif

	obdii_init();
	printf("OBDII init complete\n");

	if (can_start() != CAN_OK) {
		printf("Error starting CAN\n");
	}
	else {
		printf("CAN started\n");
	}

	// Wait for everything to be ready
	HAL_Delay(25);

	// Signal all others that the main board is ready
	if (can_transmit(CAN_MAIN_BOARD_STARTED, (uint8_t[]) { 1 }, 1) != CAN_OK) {
		//log_error(CAN_ERROR, "CAN send error");
	}

	// Wait then start initial gear sequence to detect which gear we are in
	HAL_Delay(10);

#ifndef DISABLE_ELECTRONIC_GEAR
	read_initial_gear();
	printf("Initial gear is %d\n", gear_number());
#endif
}

void loop(void){
	obdii_request_next();

	if (HAL_GetTick() - last_oil_transmit > 100) {
		can_transmit(CAN_OIL_PRESSURE, (uint8_t[]) { get_oil_pressure() }, 1);
		last_oil_transmit = HAL_GetTick();
	}

	if (HAL_GetTick() - last_neutral_transmit > 100) {
		can_transmit(CAN_NEUTRAL_SWITCH, (uint8_t[]) { neutral_switch_get_state() }, 1);
		last_neutral_transmit = HAL_GetTick();
	}

	if (HAL_GetTick() - last_water_transmit > 1000) {
		uint16_t water_temp = read_water_in();
		can_transmit(CAN_WATER_TEMPERATURE_IN, (uint8_t*) &water_temp, 2);
		last_water_transmit = HAL_GetTick();
	}

#ifndef DISABLE_ELECTRONIC_GEAR
	change_gear();
#endif
}
