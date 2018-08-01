#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include <board_driver/can.h>
#include <board_driver/obdii.h>

#include <shield_drivers/dashboard/TLC59116.h>
#include <shield_drivers/dashboard/ea_dogxl160w-7.h>

#include <shield_drivers/dashboard/buttons.h>
#include <shield_drivers/dashboard/steering_wheel_buttons.h>

#include <shield_drivers/dashboard/rpm.h>
#include <shield_drivers/dashboard/gear.h>
#include <shield_drivers/dashboard/oil.h>
#include <shield_drivers/dashboard/neutral.h>

#define DISABLE_ELECTRONIC_GEAR

#define BLINK_PULSE_WIDTH 200

uint8_t brightness_level = 2;
bool oil_pressure_warning_state = false;
bool oil_warning_active = false;
uint32_t last_state_change = 0;
bool triggered = false;

uint8_t red[] = 	{0x4, 0x0, 0x0};
uint8_t green[] = 	{0x0, 0x3, 0x0};
uint8_t blue[] = 	{0x0, 0x0, 0x6};
uint8_t yellow[] = 	{0x3, 0x3, 0x0};
uint8_t white[] = 	{0x2, 0x2, 0x2};

void setup(void);
void loop(void);
void brightness_level_up();
void write_to_led(uint8_t led_number, uint16_t r, uint16_t g, uint16_t b);
void oil_pressure_warning();
void show_rpm(void);
void check_neutral_switch();

#ifndef DISABLE_ELECTRONIC_GEAR
	gear_t show_gear(void);
	bool check_neutral_request(gear_t gear, bool neutral_btn, bool clutch);
	bool check_down_request(gear_t gear, bool down_btn, bool clutch);
	bool check_up_request(gear_t gear, bool up_btn, bool clutch);
#endif

int main(void) {
	setup();

	while (1) {
		loop();
	}
}

void setup(void) {
	can_init(CAN_PD0);

	// Init simple peripherals
	led_driver_init(true);
	sw_buttons_init();

	// CAN filters
#ifdef DISABLE_ELECTRONIC_GEAR
	gear_init();
#endif
	oil_init();
	obdii_init();
	neutral_init();

	// Start CAN
	can_start();

	HAL_Delay(10);

	can_transmit(CAN_DASHBOARD_STARTED, (uint8_t[]) { 1 }, 1);
}

void loop(void) {
	bool up_btn = sw_button_get_state(SW_BUTTON1);
	bool neutral_btn = sw_button_get_state(SW_BUTTON2);
	bool down_btn = sw_button_get_state(SW_BUTTON4);
	bool clutch = sw_button_get_state(SW_BUTTON6);
	bool oil_pressure = oil_pressure_ok();


#ifndef DISABLE_ELECTRONIC_GEAR
	gear_t gear = show_gear();
#endif

	if (neutral_btn && up_btn) {
		brightness_level_up();
	}
#ifndef DISABLE_ELECTRONIC_GEAR
	else if (!check_neutral_request(gear, neutral_btn, clutch)) {
		if (!check_down_request(gear, down_btn, clutch)) {
			if (!check_up_request(gear, up_btn, clutch)) {
				triggered = false;
			}
		}
	}
#endif

	if (oil_pressure && oil_warning_active) {
		for (int i = 0; i < 15; i++) {
			write_to_led(i, 0, 0, 0);
		}
	}

	check_neutral_switch();

	if (oil_pressure) {
		show_rpm();
		oil_warning_active = false;
	} else {
		oil_pressure_warning();
	}
}

void write_to_led(uint8_t led_number, uint16_t r, uint16_t g, uint16_t b) {
	r = r * 5 * brightness_level;
	g = g * 5 * brightness_level;
	b = b * 5 * brightness_level;

	set_led(led_number, r, g, b);
}

void brightness_level_up() {
	if (brightness_level <= 7) {
		brightness_level++;
	}
	else {
		brightness_level = 1;
	}
}

void oil_pressure_warning() {
	oil_warning_active = true;
	if (HAL_GetTick() - last_state_change > BLINK_PULSE_WIDTH) {
		oil_pressure_warning_state = !oil_pressure_warning_state;
		last_state_change = HAL_GetTick();

		if (oil_pressure_warning_state) {
			for(int i = 0; i < 15; i++){
				write_to_led(i, red[0], red[1], red[2]);
			}
		}
		else {
			for(int i = 0; i < 15; i++){
				write_to_led(i, 0, 0, 0);
			}
		}
	}
}

void show_rpm(void){
	bool new;
	int rpm_level = get_rpm_level(&new);

	if (oil_warning_active || new) {
		for(int led = 0; led < 15; led++){
			if (rpm_level > 30 + led) {
				write_to_led(led, blue[0], blue[1], blue[2]);
			}
			else if (rpm_level > 15 + led) {
				write_to_led(led, red[0], red[1], red[2]);
			}
			else if (rpm_level > led) {
				write_to_led(led, green[0], green[1], green[2]);
			}
			else {
				write_to_led(led, 0, 0, 0);
			}
		}
	}
}

void check_neutral_switch() {
	bool neutral = false;
	bool new = get_neutral(&neutral);

	if (new) {
		if (neutral) {
			write_to_led(15, red[0], red[1], red[2]);
		}
		else {
			write_to_led(15, 0, 0, 0);
		}
	}
}

gear_t show_gear(void){
	gear_t gear;
	bool new = get_gear(&gear);

	if (!new) {
		return gear;
	}

	// Can only show gear 1 to 4 and neutral
	if(gear < GEAR_5){
		for(uint8_t led = 15; led < 20; led++){
			write_to_led(led, 0, 0, 0);
		}

		if (gear == GEAR_N) {
			write_to_led((gear + 15), red[0], red[1], red[2]);
		}
		else {
			write_to_led((gear + 15), green[0], green[1], green[2]);
		}
	}
	else {
		for(uint8_t led = 15; led < 20; led++){
			write_to_led(led, white[0], white[1], white[2]);
		}
	}

	return gear;
}

bool check_neutral_request(gear_t gear, bool neutral_btn, bool clutch) {
	if (neutral_btn) {
		if (!clutch) {
			write_to_led(20, blue[0], blue[1], blue[2]);
			return true;
		}
		else if (gear != GEAR_1) {
			write_to_led(20, red[0], red[1], red[2]);
			return true;
		}
		else {
			write_to_led(20, 0, 0, 0);
		}

		if (!triggered) {
			can_transmit(CAN_GEAR_BUTTONS, (uint8_t[]) { CAN_GEAR_BUTTON_NEUTRAL }, 1);
			triggered = true;
		}

		return true;
	}

	return false;
}


bool check_down_request(gear_t gear, bool down_btn, bool clutch) {
	if (down_btn) {
		if (!clutch) {
			write_to_led(20, blue[0], blue[1], blue[2]);
			return true;
		}
		else if (gear < GEAR_2) {
			write_to_led(20, red[0], red[1], red[2]);
			return true;
		}
		else {
			write_to_led(20, 0, 0, 0);
		}

		if (!triggered) {
			can_transmit(CAN_GEAR_BUTTONS, (uint8_t[]) { CAN_GEAR_BUTTON_DOWN }, 1);
			triggered = true;
		}
		return true;
	}

	return false;
}

bool check_up_request(gear_t gear, bool up_btn, bool clutch) {
	if (up_btn) {
		if (gear >= GEAR_5) {
			write_to_led(20, red[0], red[1], red[2]);
			return true;
		}
		else {
			write_to_led(20, 0, 0, 0);
		}

		if (!triggered) {
			can_transmit(CAN_GEAR_BUTTONS, (uint8_t[]) { CAN_GEAR_BUTTON_UP }, 1);
			triggered = true;
		}
		return true;
	}

	return false;
}
