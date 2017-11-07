#ifndef SPI_H
#define SPI_H

#include <stm32f4xx_hal.h>

#define SPIx                               SPI1
/*#define SPIx_CLK_ENABLE()                  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN)
#define SPIx_FORCE_RESET()				 	       (RCC->APB2RSTR |= (RCC_APB2RSTR_SPI1RST))
#define SPIx_RELEASE_RESET()				       (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI1RST))*/

#define SPIx_SCK_AF                        GPIO_AF5
#define SPIx_SCK_GPIO_PORT                 GPIOA
#define SPIx_SCK_PIN                       PIN_5

#define SPIx_MOSI_AF                  		 GPIO_AF5
#define SPIx_MOSI_GPIO_PORT           		 GPIOA
#define SPIx_MOSI_PIN						           PIN_7


void configure_SPI_SCK(void);
void SPI_init(void);
void SPI_transmit(uint8_t *input, uint32_t Size);
//HAL_StatusTypeDef SPI_transmit(void *tx, uint32_t size);

#endif /* SPI_H */
