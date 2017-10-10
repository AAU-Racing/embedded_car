#define ADCx1_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define ADCx1_CHANNEL_PIN                GPIO_PIN_0
#define ADCx1_CHANNEL_GPIO_PORT          GPIOA
#define ADCx1_CHANNEL                    ADC_CHANNEL_0

#define ADCx2_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADCx2_CHANNEL_PIN                GPIO_PIN_3
#define ADCx2_CHANNEL_GPIO_PORT          GPIOC
#define ADCx2_CHANNEL                    ADC_CHANNEL_13

HAL_StatusTypeDef init_water_temp();
int read_water_in();
int read_water_out();
