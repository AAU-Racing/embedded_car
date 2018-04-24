#define I2C                             I2C2

/* Definition for I2C Pins */
#define I2C_SCL_PIN                    PIN_10
#define I2C_SCL_GPIO_PORT              GPIOB
#define I2C_SCL_AF                     GPIO_AF4
#define I2C_SDA_PIN                    PIN_11
#define I2C_SDA_GPIO_PORT              GPIOB
#define I2C_SDA_AF                     GPIO_AF4



#define I2C_FLAG_MASK 0x0000FFFFU;
#define __HAL_I2C_GET_FLAG(addr, flag) ((((uint8_t)((flag) >> 16U)) == 0x01U)?((((addr)->Instance->SR1) & ((flag) & I2C_FLAG_MASK)) == ((flag) & I2C_FLAG_MASK)): \
												 ((((addr)->Instance->SR2) & ((flag) & I2C_FLAG_MASK)) == ((flag) & I2C_FLAG_MASK)))
