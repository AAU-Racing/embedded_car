#include "usbd_cdc_interface.h"


#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048


uint8_t UserRxBuffer[APP_RX_DATA_SIZE]; /* Received Data over USB are stored in this buffer */
uint8_t UserTxBuffer[APP_TX_DATA_SIZE]; /* Received Data over UART (CDC interface) are stored in this buffer */

/* USB handler declaration */
extern USBD_HandleTypeDef  USBD_Device;

static int8_t CDC_Itf_Init(void);
static int8_t CDC_Itf_DeInit(void);
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive(uint8_t* pbuf, uint32_t *Len);

USBD_CDC_ItfTypeDef USBD_CDC_fops = {
	CDC_Itf_Init,
	CDC_Itf_DeInit,
	CDC_Itf_Control,
	CDC_Itf_Receive
};


static int8_t CDC_Itf_Init(void) {
	USBD_CDC_SetTxBuffer(&USBD_Device, UserTxBuffer, 0);
	USBD_CDC_SetRxBuffer(&USBD_Device, UserRxBuffer);

	return (USBD_OK);
}


static int8_t CDC_Itf_DeInit(void) {
  return (USBD_OK);
}


/*
 * @brief  CDC_Itf_Control
 *         Manage the CDC class requests
 * @param  Cmd: Command code
 * @param  Buf: Buffer containing command data (request parameters)
 * @param  Len: Number of data to be sent (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CDC_Itf_Control (uint8_t cmd, uint8_t* pbuf, uint16_t length) {
	return (USBD_OK);
}


static int8_t CDC_Itf_Receive(uint8_t* Buf, uint32_t *Len) {
	(void)Buf;
	(void)Len;
	USBD_CDC_ReceivePacket(&USBD_Device);
	return (USBD_OK);
}
