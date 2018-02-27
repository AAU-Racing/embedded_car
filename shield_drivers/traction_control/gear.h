#ifndef GEAR_H
#define GEAR_H

#include <stdbool.h>
#include "hbridge.h"

#define gear_forward()	hbridge1forward()
#define gear_reverse() 	hbridge1reverse()
#define gear_stop()		hbridge1stop()

uint8_t init_gear(); // Requires init of gear feedback and hbridge beforehand
uint8_t gear_number();
void check_gear_change();
bool gear_change_failed();

#endif
