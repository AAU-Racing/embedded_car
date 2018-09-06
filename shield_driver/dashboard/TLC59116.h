#ifndef TLC59116_H
#define TLC59116_H

#include <stdbool.h>

int led_driver_init(bool);
void set_led(unsigned, unsigned, unsigned, unsigned);

#endif /* TLC59116_H */
