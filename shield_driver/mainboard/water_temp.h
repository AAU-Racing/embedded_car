#ifndef WATER_TEMP_H
#define WATER_TEMP_H

#include <stm32f4xx_hal.h>

#define ADC_WATER_IN_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC_WATER_IN_CHANNEL_PIN                GPIO_PIN_4
#define ADC_WATER_IN_CHANNEL_GPIO_PORT          GPIOC
#define ADC_WATER_IN_CHANNEL                    ADC_CHANNEL_14

#define ADC_WATER_OUT_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC_WATER_OUT_CHANNEL_PIN                GPIO_PIN_7
#define ADC_WATER_OUT_CHANNEL_GPIO_PORT          GPIOA
#define ADC_WATER_OUT_CHANNEL                    ADC_CHANNEL_7

HAL_StatusTypeDef init_water_temp();
uint16_t read_water_in();
uint16_t read_water_out();

#endif /* WATER_TEMP_H */
