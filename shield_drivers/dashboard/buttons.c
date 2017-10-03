#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stdint.h>

#include <board_driver/gpio.h>

#include "buttons.h"

bool triggeredBtn[DASHBOARD_BUTTONn] = {false};

static uint8_t button_number(uint16_t pin) {
	switch(pin) {
		case BUTTON1_PIN:
			return 0;
		case BUTTON2_PIN:
			return 1;
		case BUTTON3_PIN:
			return 2;
		case BUTTON4_PIN:
			return 3;
		case BUTTON5_PIN:
			return 4;
		case BUTTON6_PIN:
			return 5;
		default:
			return 0xFF;
	}
}

static void button_gpio_clk_enable(dashboard_button b) {
	switch(b) {
		case DASHBOARD_BUTTON1: BUTTON1_GPIO_CLK_ENABLE(); break;
		case DASHBOARD_BUTTON2: BUTTON2_GPIO_CLK_ENABLE(); break;
		case DASHBOARD_BUTTON3: BUTTON3_GPIO_CLK_ENABLE(); break;
		case DASHBOARD_BUTTON4: BUTTON4_GPIO_CLK_ENABLE(); break;
		case DASHBOARD_BUTTON5: BUTTON5_GPIO_CLK_ENABLE(); break;
		case DASHBOARD_BUTTON6: BUTTON6_GPIO_CLK_ENABLE(); break;
		case DASHBOARD_BUTTONn:
		default:
			break;
	}
}

void buttons_handle(uint16_t number) {
	triggeredBtn[button_number(number)] = true;
}

void dashboard_buttons_init(void) {
	for (dashboard_button b = 0; b < DASHBOARD_BUTTONn; b++) {
		button_gpio_clk_enable(b);
		gpio_exti_init(BUTTON_PORT[b], BUTTON_PIN[b], GPIO_MODE_IT_FALLING, buttons_handle);
	}
}

bool dashboard_button_get_state(dashboard_button b, GPIO_PinState* state) {
	return gpio_get_state(BUTTON_PORT[b], BUTTON_PIN[b], state);
}

bool is_triggered(dashboard_button b){
	bool triggered = triggeredBtn[b];
	triggeredBtn[b] = false;
	return triggered;
}
