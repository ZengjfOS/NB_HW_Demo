#include "zengjf.h"

// I2C driver instance
extern ARM_DRIVER_I2C Driver_I2C1; 
static ARM_DRIVER_I2C *i2cDev = &Driver_I2C1;
 
static volatile uint32_t event = 0;
 
static void I2C_DrvEvent (uint32_t e) {
    event |= e;
}

void i2c_data_transmission(void) 
{
    uint8_t cnt = 0;
 
    /* Initialize I2C peripheral */
    i2cDev->Initialize(I2C_DrvEvent);
 
    /* Power-on SPI peripheral */
    i2cDev->PowerControl(ARM_POWER_FULL);
 
    /* Configure USART bus*/
    i2cDev->Control(ARM_I2C_OWN_ADDRESS, 0x33);
     
    while (1) {
        /* Receive chuck */
        i2cDev->SlaveReceive(&cnt, 1);
        while ((event & ARM_I2C_EVENT_TRANSFER_DONE) == 0);
        event &= ~ARM_I2C_EVENT_TRANSFER_DONE;
        
        printf("Receive a byte data: %x\r\n", cnt);
 
        /* Transmit chunk back */
        i2cDev->SlaveTransmit(&cnt, 1);
        while ((event & ARM_I2C_EVENT_TRANSFER_DONE) == 0);
        event &= ~ARM_I2C_EVENT_TRANSFER_DONE;
        
        printf("Send a byte data: %x\r\n", cnt);
    }
}
