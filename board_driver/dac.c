#include <stdio.h>
#include <stdint.h>
#include <stm32f4xx.h>


#define WRITE_REG_MASK(REG, MASK, VAL) WRITE_REG(REG, (READ_REG(REG) & (~(MASK))) | ((VAL) & (MASK)))

DAC_TypeDef *DacHandle = DAC;


void init_DAC_channel_1(void)
{
   WRITE_REG_MASK(DacHandle->CR, DAC_CR_EN1_Msk, 1);
}


void init_DAC_channel_2(void)
{
   WRITE_REG_MASK(DacHandle->CR, DAC_CR_EN2_Msk, 1);
}

void output_DAC_channel_1(uint8_t output){
    DacHandle->DHR8R1 = 0;
    DacHandle->DHR8R1 = output;
    //WRITE_REG_MASK(DacHandle->DHR8R1, DAC_DHR8R1_DACC1DHR_Msk, output);
}

void output_DAC_channel_2(uint8_t output){
    //DacHandle->DHR8R1 = 0;
    DacHandle->DHR8R1 = output;
    //WRITE_REG_MASK(DacHandle->DHR8R1, DAC_DHR8R1_DACC1DHR_Msk, output);
}