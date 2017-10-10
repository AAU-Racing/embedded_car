#define ADC_BRAKE1_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define ADC_BRAKE1_CHANNEL_PIN                GPIO_PIN_0
#define ADC_BRAKE1_CHANNEL_GPIO_PORT          GPIOA
#define ADC_BRAKE1_CHANNEL                    ADC_CHANNEL_0

#define ADC_BRAKE2_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC_BRAKE2_CHANNEL_PIN                GPIO_PIN_1
#define ADC_BRAKE2_CHANNEL_GPIO_PORT          GPIOA
#define ADC_BRAKE2_CHANNEL                    ADC_CHANNEL_1

HAL_StatusTypeDef init_brake_pressure();
int read_brake_pressure_front();
int read_brake_pressure_rear();
