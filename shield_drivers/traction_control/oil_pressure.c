#include <board_driver/gpio.h>

#include "oil_pressure.h"

bool on = false;

bool get_oil_pressure() {
	return on;
}

static void oil_pressure_callback(uint16_t pin) {
	(void)pin;

	on = !on;
}

void init_oil_pressure() {
	on = gpio_get_turn_on_state(OIL_PRESSURE_GPIO_PORT, OIL_PRESSURE_PIN);

	gpio_exti_init(OIL_PRESSURE_GPIO_PORT, OIL_PRESSURE_PIN, GPIO_RISING_FALLING_EDGE, oil_pressure_callback);
}
