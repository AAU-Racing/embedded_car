#ifndef USB_H
#define USB_H

#include <stdlib.h>

int usb_init(void);
int usb_transmit(void *buf, size_t len);

#endif /* USB_H */
