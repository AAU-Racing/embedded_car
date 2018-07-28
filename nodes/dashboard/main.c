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


uint8_t brightness_level = 3;
bool oil_pressure_is_ok = false;

uint8_t red[] = 	{0x4, 0x0, 0x0};
uint8_t green[] = 	{0x0, 0x3, 0x0};
uint8_t blue[] = 	{0x0, 0x0, 0x6};
uint8_t yellow[] = 	{0x3, 0x3, 0x0};
uint8_t white[] = 	{0x2, 0x2, 0x2};

void setup(void);
void loop(void);
void brightness_level_up();
void write_to_led(uint8_t ledNumber, uint16_t r, uint16_t g, uint16_t b);
void low_oil_pressure_warning();
void show_rpm(void);
void show_gear(void);

int main(void) {
	setup();

	while (1) {
		loop();
	}
}

void setup(void) {
	CAN_Init(CAN_PD0);

	led_driver_init(true);
	init_lcd();
	// dashboard_buttons_init();
	sw_buttons_init();
	//gear_init();
	oil_init();

	OBDII_Init();
	CAN_Start();

	CAN_Send(CAN_NODE_STARTED, (uint8_t[]) { CAN_NODE_DASHBOARD_STARTED }, 1);
}

void loop(void) {
	// if(is_triggered(DASHBOARD_BUTTON1)){
	// 	brightness_level_up();
	// }

	oilPressure_OK(&oil_pressure_is_ok);
	while(!oil_pressure_is_ok) {
		low_oil_pressure_warning();
		oilPressure_OK(&oil_pressure_is_ok);
	}




	show_rpm();
	//show_gear();
}

void write_to_led(uint8_t ledNumber, uint16_t r, uint16_t g, uint16_t b){
	r = r * 5 * brightness_level;
	g = g * 5 * brightness_level;
	b = b * 5 * brightness_level;

	set_led(ledNumber, r, g, b);
}

void brightness_level_up(){
	if (brightness_level <= 7){
		brightness_level++;
	}
	else{
		brightness_level = 1;
	}
}

void low_oil_pressure_warning(){

	for(int i = 0; i < 15; i++){
		write_to_led(i, red[0], red[1], red[2]);
	}

	HAL_Delay(200);

	for(int i = 0; i < 15; i++){
		write_to_led(i, 0, 0, 0);
	}

	HAL_Delay(200);
}

void show_rpm(void){
	uint8_t rpm_level = get_rpm_level();

	for(uint8_t led = 0; led < 15; led++){
		write_to_led(led, 0, 0, 0);
	}

	for(uint8_t led = 0; led < 5; led++){
		if(rpm_level > led)
			write_to_led(led, green[0], green[1], green[2]);
	}

	for(uint8_t led = 5; led < 10; led++){
		if(rpm_level > led)
			write_to_led(led, red[0], red[1], red[2]);
	}

	if(rpm_level == 11){
		for(uint8_t led = 10; led < 15; led++){
			write_to_led(led, blue[0], blue[1], blue[2]);
		}
	}
}

void show_gear(void){
	gear_t gear;
	get_gear(&gear);

	if(gear < GEAR_5){
		for(uint8_t led = 15; led < 20; led++){
			write_to_led(led, 0, 0, 0);
		}
		write_to_led((gear + 15), white[0], white[1], white[2]);
	}
	else{
		for(uint8_t led = 15; led < 20; led++){
			write_to_led(led, white[0], white[1], white[2]);
		}
	}
}
