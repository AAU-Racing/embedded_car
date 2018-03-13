#include <stm32f4xx.h>

#include "spi.h"
#include "gpio.h"

#define WRITE_REG_MASK(REG, MASK, VAL) WRITE_REG(REG, (READ_REG(REG) & (~(MASK))) | ((VAL) & (MASK)))

#define TX_TIMEOUT_MS	(1000)
#define BAUDRATEPRESCALER_8 0x010
#define DIRECTION_2LINES 0x00
#define PHASE_2EDGE 0x01
#define POLARITY_HIGH 0x01
#define CRCCALCULATION_DISABLED 0x00
#define POLYNOMIAL_RESET 7
#define DATASIZE_8BIT 0x00
#define FIRSTBIT_MSB 0x00
#define NSS_SOFT 0x01
#define TIMODE_DISABLED 0x00
#define MODE_MASTER 0x01
#define MODE_AF_PP 0x02
#define SPI_ENABLE 0x01

#define SPI_INSTANCE ((SPI_TypeDef* const []) { \
	SPI1, \
	SPI2, \
})

#define SPI_SCK_PIN ((const uint32_t const []) { \
	GPIO_PIN_5, \
	GPIO_PIN_13,	 \
})

#define SPI_SCK_PORT ((GPIO_TypeDef* const []) { \
	GPIOA, \
	GPIOB, \
})

#define SPI_MOSI_PIN ((const uint32_t const []) { \
	GPIO_PIN_7, \
	GPIO_PIN_15,	 \
})

#define SPI_MOSI_PORT ((GPIO_TypeDef* const []) { \
	GPIOA, \
	GPIOB, \
})

#define SPI_MISO_PIN ((const uint32_t const []) { \
	0, \
	GPIO_PIN_14,	 \
})

#define SPI_MISO_PORT ((GPIO_TypeDef* const []) { \
	NULL, \
	GPIOB, \
})

SPI_TypeDef *SPIHandle;

static void configure_spi_pins(int config)
{
	gpio_af_init(SPI_SCK_PORT[config],
        SPI_SCK_PIN[config],
		GPIO_HIGH_SPEED,
		GPIO_NO_PULL,
		GPIO_AF5_SPI1);

    if (SPI_MOSI_PORT[config] != NULL) {
    	gpio_af_init(SPI_MOSI_PORT[config],
    		SPI_MOSI_PIN[config],
    		GPIO_HIGH_SPEED,
    		GPIO_NO_PULL,
    		GPIO_AF5_SPI1);
    }

    if (SPI_MISO_PORT[config] != NULL) {
        gpio_af_init(SPI_MISO_PORT[config],
            SPI_MISO_PIN[config],
            GPIO_HIGH_SPEED,
            GPIO_NO_PULL,
            GPIO_AF5_SPI1);
    }
}

static void enable_spi_clk() {
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI2EN);
}

void SPI_init(int config)
{
    SPIHandle = SPI_INSTANCE[config];
	configure_spi_pins(config);

    enable_spi_clk();

	WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_BR_Msk, BAUDRATEPRESCALER_8);
	WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_BIDIMODE_Msk , DIRECTION_2LINES);
	WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_CPHA_Msk, PHASE_2EDGE);
	WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_CPOL_Msk , POLARITY_HIGH);
	WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_CRCEN_Msk, CRCCALCULATION_DISABLED);
	WRITE_REG_MASK(SPIHandle->CRCPR, SPI_CRCPR_CRCPOLY_Msk, POLYNOMIAL_RESET);
	WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_DFF_Msk, DATASIZE_8BIT);
	WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_LSBFIRST_Msk, FIRSTBIT_MSB);
	WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_SSM_Msk, NSS_SOFT);
	WRITE_REG_MASK(SPIHandle->CR2, SPI_CR2_FRF_Msk, TIMODE_DISABLED);
	WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_MSTR_Msk, MODE_MASTER);
}


void SPI_transmit(uint8_t *input, uint32_t count)
{
	WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_SPE_Msk, SPI_ENABLE);

	while(count > 0U)
	{
		// Wait until TXE flag is set to send data
		while(!(SPIHandle->SR & SPI_SR_TXE));

		// Write a data item to send into the SPI_DR register (this clears the TXE bit).
		SPIHandle->DR = *input;
		input += sizeof(uint8_t);
		count--;
	}

	// After writing the last data item into the SPI_DR register, wait until TXE=1, then wait until
	// BSY=0, this indicates that the transmission of the last data is complete.
	//while(!(SPIHandle->SR & SPI_SR_TXE) && (SPIHandle->SR & SPI_SR_BSY));
}








//OLD HAL CODE
/*#include <stm32f4xx_hal.h>

#include "spi.h"

#define TX_TIMEOUT_MS	(1000)

static SPI_HandleTypeDef spi_handle;

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
	(void)hspi; // unused parameter

	SPIx_SCK_GPIO_CLK_ENABLE();
	SPIx_MOSI_GPIO_CLK_ENABLE();
	SPIx_CLK_ENABLE();

	// Configure SPI SCK
	GPIO_InitTypeDef gpio = {
		.Pin       = SPIx_SCK_PIN,
		.Mode      = GPIO_MODE_AF_PP,
		.Pull      = GPIO_NOPULL,
		.Speed     = GPIO_SPEED_FAST,
		.Alternate = SPIx_SCK_AF,
	};
	HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &gpio);

	// Configure SPI MOSI
	gpio.Pin       = SPIx_MOSI_PIN;
	gpio.Alternate = SPIx_MOSI_AF;
	gpio.Pull      = GPIO_NOPULL;
	HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &gpio);
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
	(void) hspi;

	SPIx_FORCE_RESET();
	SPIx_RELEASE_RESET();

	HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
	HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
}

void SPI_init(void) {
	if (HAL_SPI_GetState(&spi_handle) == HAL_SPI_STATE_RESET) {
		spi_handle.Instance                = SPIx;

		spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
		spi_handle.Init.Direction         = SPI_DIRECTION_2LINES;
		spi_handle.Init.CLKPhase          = SPI_PHASE_2EDGE;
		spi_handle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
		spi_handle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
		spi_handle.Init.CRCPolynomial     = 7;
		spi_handle.Init.DataSize          = SPI_DATASIZE_8BIT;
		spi_handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
		spi_handle.Init.NSS               = SPI_NSS_SOFT;
		spi_handle.Init.TIMode            = SPI_TIMODE_DISABLED;
		spi_handle.Init.Mode              = SPI_MODE_MASTER;

		HAL_SPI_Init(&spi_handle);
	}
}

HAL_StatusTypeDef SPI_transmit(void *tx, uint32_t size) {
	return HAL_SPI_Transmit(&spi_handle, tx, size, TX_TIMEOUT_MS);
}*/
