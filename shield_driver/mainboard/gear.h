#ifndef GEAR_H
#define GEAR_H

#include <stdbool.h>
#include <stdint.h>

int init_gear();
uint8_t gear_number();
void change_gear();
void read_initial_gear();
void wanted_gear_up();
void wanted_gear_down();

#endif
