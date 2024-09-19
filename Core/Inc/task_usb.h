/**
 * @brief USB CDC driver
 */

/*
Error codes:
0:  SUCCESS
1:  SMBUS_ERROR_TIMEOUT
2:  SMBUS_ERROR_BUS_BUSY
3:  SMBUS_ERROR_ADDR_NACK
4:  SMBUS_ERROR_DATA_NACK
99: unknown command
*/

#define MAX_BUFFER_LENGTH 64
#define END_FLAG '\r\n'

void ParseCommand(uint8_t *, uint32_t);
void SendResponse(uint8_t, uint8_t *, uint16_t);
void CDC_ReceiveCallback(uint8_t *, uint32_t);
void Task_USBD_DeInit();
