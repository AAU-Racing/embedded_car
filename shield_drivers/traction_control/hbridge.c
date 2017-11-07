#include <board_driver/gpio.h>
#include <board_driver/pwm.h>

#include "hbridge.h"

TIM_HandleTypeDef timer;

void init_hbridge() {
	HBRIDGE1_PWM_CLK_ENABLE();
	HBRIDGE2_PWM_CLK_ENABLE();

	gpio_output_init(HBRIDGE1_EN_PORT, HBRIDGE1_EN_PIN);
	gpio_output_init(HBRIDGE1_DIR_PORT, HBRIDGE1_DIR_PIN);

	gpio_output_init(HBRIDGE2_EN_PORT, HBRIDGE2_EN_PIN);
	gpio_output_init(HBRIDGE2_DIR_PORT, HBRIDGE2_DIR_PIN);

	HBRIDGE_PWM_TIMER_CLK_ENABLE();
	init_pwm(HBRIDGE_PWM_TIMER, &timer);
	init_pwm_pin(HBRIDGE1_PWM_PORT, HBRIDGE1_PWM_PIN, HBRIDGE1_PWM_GPIO_AF);
	init_pwm_pin(HBRIDGE2_PWM_PORT, HBRIDGE2_PWM_PIN, HBRIDGE2_PWM_GPIO_AF);
	configure_pwm_channel(&timer, HBRIDGE1_PWM_CHANNEL, 100);
	configure_pwm_channel(&timer, HBRIDGE2_PWM_CHANNEL, 100);
}

void hbridge1forward() {
	gpio_toogle_off(HBRIDGE1_DIR_PORT, HBRIDGE1_DIR_PIN);
	gpio_toogle_on(HBRIDGE1_EN_PORT, HBRIDGE1_EN_PIN);

	start_pwm_channel(&timer, HBRIDGE1_PWM_CHANNEL);
}

void hbridge1reverse() {
	gpio_toogle_off(HBRIDGE1_EN_PORT, HBRIDGE1_EN_PIN);
	gpio_toogle_on(HBRIDGE1_DIR_PORT, HBRIDGE1_DIR_PIN);

	start_pwm_channel(&timer, HBRIDGE1_PWM_CHANNEL);
}

void hbridge1stop() {
	gpio_toogle_off(HBRIDGE1_EN_PORT, HBRIDGE1_EN_PIN);
	gpio_toogle_off(HBRIDGE1_DIR_PORT, HBRIDGE1_DIR_PIN);

	stop_pwm_channel(&timer, HBRIDGE1_PWM_CHANNEL);
}

void hbridge2forward() {
	gpio_toogle_off(HBRIDGE2_DIR_PORT, HBRIDGE2_DIR_PIN);
	gpio_toogle_on(HBRIDGE2_EN_PORT, HBRIDGE2_EN_PIN);

	start_pwm_channel(&timer, HBRIDGE2_PWM_CHANNEL);
}

void hbridge2reverse() {
	gpio_toogle_off(HBRIDGE2_EN_PORT, HBRIDGE2_EN_PIN);
	gpio_toogle_on(HBRIDGE2_DIR_PORT, HBRIDGE2_DIR_PIN);

	start_pwm_channel(&timer, HBRIDGE2_PWM_CHANNEL);
}

void hbridge2stop() {
	gpio_toogle_off(HBRIDGE2_EN_PORT, HBRIDGE2_EN_PIN);
	gpio_toogle_off(HBRIDGE2_DIR_PORT, HBRIDGE2_DIR_PIN);

	stop_pwm_channel(&timer, HBRIDGE2_PWM_CHANNEL);
}
