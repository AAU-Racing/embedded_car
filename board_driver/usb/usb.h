#ifndef USB_H
#define USB_H

#include <stdlib.h>
#include "usbd_cdc_interface.h"

int usb_init(void);
void usb_transmit(void *buf, size_t len);

#endif /* USB_H */
