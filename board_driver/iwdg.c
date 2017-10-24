#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stdio.h>
#include <board_driver/usb/usb.h>

#define ENABLE_WRITE_ACCESS 0x5555
#define SET_PRESCALER32 0x03
#define SET_RELOAD_TIME 0xFFF
#define START_IDWG 0xCCCC
#define RESET_IWDG 0xAAAA

IWDG_TypeDef *IwdgHandle = IWDG;

void setup_dog()
{
    IwdgHandle->KR = ENABLE_WRITE_ACCESS;
    IwdgHandle->PR = SET_PRESCALER32;
    IwdgHandle->RLR = SET_RELOAD_TIME;

    while(!(IwdgHandle->SR & IWDG_SR_PVU_Msk == 1) && !(IwdgHandle->SR & IWDG_SR_RVU_Msk == 1))
}

//At startup check if through IWDGRSTF if reset was caused by IWDG
int pre_init_dog()
{
    return !!(RCC_SCR & RCC_CSR_IWDGRSTF_Msk)
}

int init_dog()
{
    IwdgHandle->KR = START_IDWG;
}

//Refreshes the IWDG.
int reset_dog()
{
    IwdgHandle->KR = RESET_IWDG;
}
