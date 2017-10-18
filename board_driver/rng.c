#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stm32f4xx.h>

void rng_init()
{
	//Enable random number clock
	__IO uint32_t tmpreg; 
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN);
    /* Delay after an RCC peripheral clock enabling */ 
    tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN);
    (void)tmpreg;

    //Enable random number generator on Chip
	RNG->CR = 1 << 2;
}

//Includes min, but excludes max
uint32_t get_rng(int min, int max)
{
	return (RNG->DR % (max - min)) + min;
}