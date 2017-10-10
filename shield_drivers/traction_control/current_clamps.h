#ifndef CURRENT_CLAMPS_H
#define CURRENT_CLAMPS_H

#include <stm32f4xx_hal.h>

#define ADC_CURRENT1_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADC_CURRENT1_CHANNEL_PIN                GPIO_PIN_0
#define ADC_CURRENT1_CHANNEL_GPIO_PORT          GPIOB
#define ADC_CURRENT1_CHANNEL                    ADC_CHANNEL_8

#define ADC_CURRENT2_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADC_CURRENT2_CHANNEL_PIN                GPIO_PIN_1
#define ADC_CURRENT2_CHANNEL_GPIO_PORT          GPIOB
#define ADC_CURRENT2_CHANNEL                    ADC_CHANNEL_9

HAL_StatusTypeDef init_current_clamps();
uint16_t read_battery_current();
uint16_t read_inverter_current();

#endif /* CURRENT_CLAMPS_H */
