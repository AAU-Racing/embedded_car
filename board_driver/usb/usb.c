#include <usbd_cdc.h>
#include "usbd_desc.h"
#include "usbd_cdc_interface.h"

#include "usb.h"

extern USBD_CDC_ItfTypeDef  USBD_CDC_fops;
extern USBD_DescriptorsTypeDef VCP_Desc;

USBD_HandleTypeDef USBD_Device;

int usb_init(void) {
	/* Init Device Library */
	USBD_Init(&USBD_Device, &VCP_Desc, 0);

	/* Add Supported Class */
	USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

	/* Add CDC Interface Class */
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

	/* Start Device Process */
	USBD_Start(&USBD_Device);

	return 0;
}

int usb_transmit(void *buf, size_t len) {
	USBD_CDC_SetTxBuffer(&USBD_Device, (uint8_t*)buf, len);

	return USBD_CDC_TransmitPacket(&USBD_Device) == USBD_OK;
}
