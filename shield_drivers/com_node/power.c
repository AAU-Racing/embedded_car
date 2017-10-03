#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>

#include <stdbool.h>

#include "power.h"

__weak void Power_Interupt_Callback(){}

void power_interupt_pin_init(void){
	POWER_INTERUPT_GPIO_CLK_ENABLE();

	gpio_exti_init(POWER_INTERUPT_GPIO_PORT, POWER_INTERUPT_PIN, GPIO_MODE_IT_FALLING, Power_Interupt_Callback);
}

bool power_get_state() {
	GPIO_PinState* state;
	return gpio_get_state(POWER_INTERUPT_GPIO_PORT, POWER_INTERUPT_PIN, state);
}
