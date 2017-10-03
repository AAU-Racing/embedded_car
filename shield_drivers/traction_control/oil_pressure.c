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
	OIL_PRESSURE_GPIO_CLK_ENABLE();

	// Init as normal pin to check it's on
	HAL_GPIO_Init(OIL_PRESSURE_GPIO_PORT, &(GPIO_InitTypeDef) {
		.Pin = OIL_PRESSURE_PIN,
		.Mode = GPIO_MODE_INPUT,
		.Pull = GPIO_PULLDOWN,
		.Speed = GPIO_SPEED_FAST,
	});

	HAL_Delay(1);

	on = HAL_GPIO_ReadPin(OIL_PRESSURE_GPIO_PORT, OIL_PRESSURE_PIN);

	HAL_GPIO_DeInit(OIL_PRESSURE_GPIO_PORT, OIL_PRESSURE_PIN);

	gpio_exti_init(OIL_PRESSURE_GPIO_PORT, OIL_PRESSURE_PIN, GPIO_MODE_IT_RISING_FALLING, oil_pressure_callback);
}
