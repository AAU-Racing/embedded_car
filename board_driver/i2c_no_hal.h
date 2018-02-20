#define I2C                             I2C2
#define I2C_CLK_ENABLE()                __HAL_RCC_I2C2_CLK_ENABLE()

/* Definition for I2C Pins */
#define I2C_SCL_PIN                    PIN_10
#define I2C_SCL_GPIO_PORT              GPIOB
#define I2C_SCL_AF                     GPIO_AF4
#define I2C_SDA_PIN                    PIN_11
#define I2C_SDA_GPIO_PORT              GPIOB
#define I2C_SDA_AF                     GPIO_AF4
