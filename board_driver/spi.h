#ifndef SPI_H
#define SPI_H

#include <stm32f4xx_hal.h>

#define SPIx                               SPI1
#define SPIx_CLK_ENABLE()                  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN)
#define SPIx_FORCE_RESET()				 	       (RCC->APB2RSTR |= (RCC_APB2RSTR_SPI1RST))
#define SPIx_RELEASE_RESET()				       (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI1RST))

#define SPIx_SCK_AF                        GPIO_AF5_SPI1
#define SPIx_SCK_GPIO_PORT                 GPIOA
#define SPIx_SCK_PIN                       ((uint16_t)0x0020)
#define SPIx_SCK_GPIO_CLK_ENABLE()         SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN)
#define SPIx_SCK_GPIO_CLK_DISABLE()        (RCC->AHB1ENER &= ~(RCC_AHB1ENR_GPIOAEN))

#define SPIx_MOSI_AF                  		GPIO_AF5_SPI1
#define SPIx_MOSI_GPIO_PORT           		GPIOA
#define SPIx_MOSI_PIN						          ((uint16_t)0x0080)
#define SPIx_MOSI_GPIO_CLK_ENABLE()   		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN)
#define SPIx_MOSI_GPIO_CLK_DISABLE()  		(RCC->AHB1ENER &= ~(RCC_AHB1ENR_GPIOAEN))


void SPI_init(void);
HAL_StatusTypeDef SPI_transmit(void *tx, uint32_t size);

#endif /* SPI_H */
