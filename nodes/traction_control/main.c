#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/uart.h>
#include <board_driver/adc.h>
#include <board_driver/can.h>
#include <shield_drivers/traction_control/water_temp.h>
#include <shield_drivers/traction_control/wheel_sensor.h>
#include <shield_drivers/traction_control/gear.h>
#include <shield_drivers/traction_control/gear_feedback.h>
#include <shield_drivers/traction_control/oil_pressure.h>
#include <shield_drivers/traction_control/hbridge.h>
#include <shield_drivers/traction_control/current_clamps.h>

#include "send_data.h"

static bool adc_ready = false;

void setup();
void loop();

int main(void) {
	setup();

	while (1) {
		loop();
	}
}

void setup() {
	// Init peripherals
	uart_init();

	if (CAN_Init(CAN_PB5) != CAN_OK) {
		printf("CAN init failed\n");
	}

	if (init_adc(5) != HAL_OK) { // 2 for water, 2 for current_clamps, and 1 for gear feedback
		printf("ADC init failed\n");
        CAN_Start();
		CAN_Send(CAN_ADC_ERROR, (uint8_t[]) { CAN_ADC_INIT_ERROR }, 1);
	}
	else {
		adc_ready = true;
	}

	if (init_water_temp() != HAL_OK) {
		printf("Water temp init failed\n");
        CAN_Start();
		CAN_Send(CAN_ADC_ERROR, (uint8_t[]) { CAN_ADC_WATER_TEMP_ERROR }, 1);
		adc_ready = false;
	}

	if (init_current_clamps() != 0) {
		printf("Current clamps init failed\n");
        CAN_Start();
		CAN_Send(CAN_ADC_ERROR, (uint8_t[]) { CAN_ADC_CURRENT_CLAMPS_ERROR }, 1);
		adc_ready = false;
	}

	if (init_gear() != HAL_OK) {
		printf("Gear init failed\n");
        CAN_Start();
		CAN_Send(CAN_ADC_ERROR, (uint8_t[]) { CAN_ADC_GEAR_ERROR }, 1);
		adc_ready = false;
	}

	wheel_sensor_init();
	init_hbridge();

	// Check oil pressure at the start
	//init_oil_pressure();

	printf("Init complete\n");
	HAL_Delay(10);

	// Start peripherals
	if (start_adc() != HAL_OK) {
        printf("Start ADC failed\n");
        CAN_Start();
		CAN_Send(CAN_ADC_ERROR, (uint8_t[]) { CAN_ADC_START_ERROR }, 1);
		adc_ready = false;
	}

    CAN_Start();

	printf("Peripherals started\n");
	HAL_Delay(10);

	CAN_Send(CAN_NODE_STARTED, (uint8_t[]) { CAN_NODE_TRACTION_CONTROL_STARTED }, 1);
}

void loop() {
#ifdef DEBUG
	static uint32_t last_transmit = 0;

	if (HAL_GetTick() - last_transmit > 100) {
		printf("Sending via CAN\n");
		last_transmit = HAL_GetTick();
	}
#endif
	handle_oil_pressure();
	handle_water_temp(adc_ready);
	handle_current_clamps(adc_ready);
	handle_gear();
	handle_wheel_sensor();
}
