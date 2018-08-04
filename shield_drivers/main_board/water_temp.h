#ifndef WATER_TEMP_H
#define WATER_TEMP_H

#include <stm32f4xx_hal.h>

#define ADC_WATER1_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC_WATER1_CHANNEL_PIN                GPIO_PIN_2
#define ADC_WATER1_CHANNEL_GPIO_PORT          GPIOC
#define ADC_WATER1_CHANNEL                    ADC_CHANNEL_12

#define ADC_WATER2_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC_WATER2_CHANNEL_PIN                GPIO_PIN_3
#define ADC_WATER2_CHANNEL_GPIO_PORT          GPIOC
#define ADC_WATER2_CHANNEL                    ADC_CHANNEL_13

HAL_StatusTypeDef init_water_temp();
uint16_t read_water_in();
uint16_t read_water_out();

#endif /* WATER_TEMP_H */