#include <stm32f407xx.h>

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
#define GPIO_NOPULL 0x00
#define SPI_ENABLE 0x01

static SPI_HandleTypeDef spi_handle;

SPI_TypeDef *SPIHandle = SPI1;


SPIx_SCK_GPIO_CLK_ENABLE();
SPIx_MOSI_GPIO_CLK_ENABLE();
SPIx_CLK_ENABLE();

void configure_SPI_SCK(void)
{
	gpio_af_init(SPIx_SCK_GPIO_PORT,
		SPIx_SCK_PIN,
		GPIO_HIGH_SPEED,
		GPIO_NO_PULL,
		SPIx_SCK_AF);

	gpio_af_init(SPIx_SCK_GPIO_PORT,
		SPIx_MOSI_PIN,
		GPIO_HIGH_SPEED,
		GPIO_NO_PULL,
		SPIx_MOSI_AF);
}

void SPI_init(void)
{
		WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_SPE_Msk, SPI_ENABLE);

		configure_SPI_SCK();

		WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_BR_Msk, BAUDRATEPRESCALER_8);
		WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_BIDIMODE_Msk , DIRECTION_2LINES);
		WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_CPHA_Msk, PHASE_2EDGE);
		WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_CPOL_Msk , POLARITY_HIGH);
		WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_CRCEN_Msk, CRCCALCULATION_DISABLED);
		WRITE_REG_MASK(SPIHandle->CRCPR, SPI_CRCPR_CRCPOLY_Msk, POLYNOMIAL_RESET);
		WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_DFF_Msk, DATASIZE_8BIT);
		WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_LSBFIRST_Msk, FIRSTBIT_MSB);
		WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_SSM_Msk, NSS_SOFT);
		WRITE_REG_MASK(SPIHandle->CR2, SPI_CR1_FRF_Msk, TIMODE_DISABLED);
		WRITE_REG_MASK(SPIHandle->CR1, SPI_CR1_MSTR_Msk, MODE_MASTER);
}


SPI_transmit()
{
	if(count == 0x01)
	{
		/**((__IO uint8_t*)&hspi->Instance->DR) = (*pData);
		pData += sizeof(uint8_t);
		hspi->TxXferCount--;*/
	}
	while(count > 0U)
	{
		/* Wait until TXE flag is set to send data *
		if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE))
		{
			*((__IO uint8_t*)&hspi->Instance->DR) = (*pData);
			pData += sizeof(uint8_t);
			hspi->TxXferCount--;
		}
		else
		{
			/* Timeout management *
			if((Timeout == 0U) || ((Timeout != HAL_MAX_DELAY) && ((HAL_GetTick()-tickstart) >=  Timeout)))
			{
				errorcode = HAL_TIMEOUT;
				goto error;
			}
		}*/
	}
}

/*HAL_StatusTypeDef SPI_transmit(void *tx, uint32_t size) {
	return HAL_SPI_Transmit(&spi_handle, tx, size, TX_TIMEOUT_MS);*/


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
