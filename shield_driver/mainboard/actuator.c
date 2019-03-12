#include <board_driver/gpio.h>
#include <board_driver/pwm.h>

#include "actuator.h"

void init_actuator() {
	gpio_output_init(ACTUATOR1_EN_PORT, ACTUATOR1_EN_PIN);
	gpio_output_init(ACTUATOR1_DIR_PORT, ACTUATOR1_DIR_PIN);

	gpio_output_init(ACTUATOR2_EN_PORT, ACTUATOR2_EN_PIN);
	gpio_output_init(ACTUATOR2_DIR_PORT, ACTUATOR2_DIR_PIN);
}

void actuator_backward_start() {
	gpio_toggle_on(ACTUATOR1_EN_PORT, ACTUATOR1_EN_PIN);
}

void actuator_backward_stop() {
	gpio_toggle_off(ACTUATOR1_EN_PORT, ACTUATOR1_EN_PIN);
}

void actuator_forward_start() {
	gpio_toggle_on(ACTUATOR2_EN_PORT, ACTUATOR2_EN_PIN);
}

void actuator_forward_stop() {
	gpio_toggle_off(ACTUATOR2_EN_PORT, ACTUATOR2_EN_PIN);
}
