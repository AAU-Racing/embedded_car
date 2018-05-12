#include "flash.h"
#include "bootloader_protocol.h"

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

void StartUpdate(Packet* packet, uint32_t* len, uint32_t* rtc);
void ReceiveImage(Packet* packet, uint8_t* data, uint32_t* offset);
void UpdateImage(uint32_t address, Packet* packet, uint8_t* data, uint32_t len);

#endif