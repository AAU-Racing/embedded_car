#include "board_driver/bootloader_protocol.h"

#ifndef USB_DRIVER_H
#define USB_DRIVER_H

int usb_driver_receive_packet(Packet* packet);
void usb_driver_transmit_packet(Packet packet);

#endif