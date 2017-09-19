#include "zengjf.h"

/* SPI Driver */
extern ARM_DRIVER_SPI Driver_SPI1;
osSemaphoreId_t spi_Thread_Semaphore; 
 
void mySPI_callback(uint32_t event)
{
    switch (event)
    {
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
        /* Success: Wakeup Thread */
        osSemaphoreRelease (spi_Thread_Semaphore);
        break;
    case ARM_SPI_EVENT_DATA_LOST:
        /*  Occurs in slave mode when data is requested/sent by master
            but send/receive/transfer operation has not been started
            and indicates that data is lost. Occurs also in master mode
            when driver cannot transfer data fast enough. */
        __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    case ARM_SPI_EVENT_MODE_FAULT:
        /*  Occurs in master mode when Slave Select is deactivated and
            indicates Master Mode Fault. */
        __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    }
}
 
/* Test data buffers */
uint8_t testdata_out[8] = { 0, 1, 2, 3, 4, 5, 6, 7 }; 
uint8_t testdata_in [8];
 
void vTaskSPI1(void* arg)
{ 
    spi_Thread_Semaphore = osSemaphoreNew(1, 0, NULL);
    if (!spi_Thread_Semaphore) {
        printf("get spi_Thread_Semaphore error.\r\n");  // Semaphore object not created, handle failure
    }
    
    ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
 
    /* Initialize the SPI driver */
    SPIdrv->Initialize(mySPI_callback);
    /* Power up the SPI peripheral */
    SPIdrv->PowerControl(ARM_POWER_FULL);
    /* Configure the SPI to Master, 8-bit mode @10000 kBits/sec */
    SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_SS_MASTER_SW | ARM_SPI_DATA_BITS(8), 10000000);
 
    /* SS line = INACTIVE = HIGH */
    SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
 
    /* thread loop */
    while (1)
    {
        /* SS line = ACTIVE = LOW */
        SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
        /* Transmit some data */
        SPIdrv->Transfer(testdata_out, testdata_in, sizeof(testdata_out));
        /* Wait for completion */
        osSemaphoreAcquire (spi_Thread_Semaphore, osWaitForever);
        /* SS line = INACTIVE = HIGH */
        SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
        
        int i = 0;
        for (i = 0; i < sizeof(testdata_in) / sizeof(testdata_in[0]); i++) {
            printf("%d\n\r", testdata_in[i]);
            testdata_out[i] += 1;
        }
        
        osDelay(1000); 
    }
}
