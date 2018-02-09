#include <stdint.h>

#include <board_driver/gpio.h>

#include "steering_wheel_buttons.h"

__weak void SW_Button1_Callback() {}
__weak void SW_Button2_Callback() {} //Button 2 is currently unavailable
__weak void SW_Button3_Callback() {}
__weak void SW_Button4_Callback() {}
__weak void SW_Button5_Callback() {}

void sw_buttons_handle(uint16_t sw_button) {
	switch(sw_button) {
		case SW_BUTTON1_PIN:
			SW_Button1_Callback();
		case SW_BUTTON2_PIN:
			SW_Button2_Callback();
		case SW_BUTTON3_PIN:
			SW_Button3_Callback();
		case SW_BUTTON4_PIN:
			SW_Button4_Callback();
		case SW_BUTTON5_PIN:
			SW_Button5_Callback();
	}
}

void sw_buttons_init(void) {
	for (sw_button b = 0; b < SW_BUTTONn; b++) {
		//Tepmpoary fix for crashing in gpio_exti_init with sw_button2
		if (b != SW_BUTTON2)
			gpio_exti_init(SW_BUTTON_PORT[b], SW_BUTTON_PIN[b], GPIO_FALLING_EDGE, sw_buttons_handle);
	}
}

bool sw_button_get_state(sw_button b, bool* state) {
	return gpio_get_state(SW_BUTTON_PORT[b], SW_BUTTON_PIN[b], state);
}
