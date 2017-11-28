#ifndef DAC_H
#define DAC_H

#include <stdint.h>

void init_DAC_channel_1(void);
void init_DAC_channel_2(void);
void output_DAC_channel_1(uint8_t output);
void output_DAC_channel_2(uint8_t output);

#endif