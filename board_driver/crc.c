#include <stm32f4xx.h>

void crc_init (){
    // Enable clock
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_CRCEN);
}

uint32_t crc_calculate(uint32_t buffer[], size_t n){
    //Reset CRC register
    SET_BIT(CRC->CR, CRC_CR_RESET);

    // For all data
    for (size_t i = 0; i < n; i++) {
        // Accumulate CRC
        CRC->DR = buffer[i];
    }

    // Result in data register
    return CRC->DR;
}
