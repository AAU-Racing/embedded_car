#include "board_driver/bootloader_protocol.h"

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

int can_driver_receive_packet(Packet* packet);
void can_driver_transmit_packet(Packet packet);

#endif