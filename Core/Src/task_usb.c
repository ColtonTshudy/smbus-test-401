#include "main.h"
#include "usbd_cdc_if.h"
#include "task_usb.h"
#include "task_smbus.h"

uint8_t address = 0;
uint32_t UserRxLength;
extern USBD_HandleTypeDef hUsbDeviceFS;

// This function should be called from the CDC_Receive_FS function in usbd_cdc_if.c
void CDC_ReceiveCallback(uint8_t *buf, uint32_t len)
{
    ParseCommand(buf, len);
}

void ParseCommand(uint8_t *buf, uint32_t len)
{
    uint8_t rc = 99;
    /*
     * X = address (1 byte)
     * x = data (1 byte)
     * xx = data (2 bytes)
     * EF = END_FLAG (end of transmission, 2 bytes)
     */

    //-wb XxEF
    if (strncmp((char *)buf, "-wb", 3) == 0 && len == 8)
    {
    }
    //-wb XxxEF
    else if (strncmp((char *)buf, "-ww", 3) == 0 && len == 8)
    {
    }
    //-rb XEF
    else if (strncmp((char *)buf, "-rb", 3) == 0)
    {
        uint8_t command = buf[4];
        uint8_t data;
        rc = SMBus_ReadByte(address, command, &data);
        SendResponse(rc, &data, 1);
    }
    //-rw XEF
    else if (strncmp((char *)buf, "-rw", 3) == 0)
    {
        uint8_t command = buf[4];
        uint16_t data;
        rc = SMBus_ReadWord(address, command, &data);
        SendResponse(rc, (uint8_t *)&data, 2);
    }
    //-br XEF
    else if (strncmp((char *)buf, "-br", 3) == 0)
    {
        uint8_t command = buf[4];
        uint8_t data[33]; // length byte + 32 possible data bytes
        uint8_t length = 0;
        rc = SMBus_BlockRead(address, command, &data[1], &length);
        data[0] = length;
        SendResponse(rc, data, length + 1);
    }
    //-sa xEF
    else if (strncmp((char *)buf, "-sa", 3) == 0)
    {
        address = buf[4];
        rc = 0;
        SendResponse(rc, &address, 1); // "Success" response with no data
    }
    else if (strncmp((char *)buf, "-ss", 3) == 0)
    {
        uint32_t speed = *(uint32_t *)(&buf[4]);
        SMBus_Set_Speed(speed);
        rc = 0;
        SendResponse(0, (uint8_t *)&speed, 4); // "Success" response with no data
    }
    else
    {
        SendResponse(rc, NULL, 0); // Unknown command
    }

    if (rc != 0)
    {
        // Error_Handler();
    }
}

void SendResponse(uint8_t status, uint8_t *data, uint16_t len)
{
    uint8_t response[MAX_BUFFER_LENGTH] = {0};
    response[0] = status;
    uint16_t respLen = 1;

    if (data != NULL && len > 0)
    {
        memcpy(&response[1], data, len);
        respLen += len;
    }

    response[respLen++] = '\r';
    response[respLen++] = '\n'; // serial termination characters

    CDC_Transmit_FS(response, respLen);
}

/**
 * @brief Pipes printf to USB_CDC
 *
 * @param file
 * @param ptr
 * @param len
 * @return int
 */
int _write(int file, char *ptr, int len)
{
    (void)file;
    CDC_Transmit_FS((uint8_t *)ptr, len);
    return len;
}

void Task_USBD_DeInit()
{
    // Stop USB device
    USBD_Stop(&hUsbDeviceFS);

    // Deinitialize USB device
    USBD_DeInit(&hUsbDeviceFS);

    // Disable USB interrupts (if you've set them up separately)
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn); // or USB_LP_IRQn, depending on your specific STM32
}