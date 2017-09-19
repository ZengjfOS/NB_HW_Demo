#include "zengjf.h"

osSemaphoreId_t sid_Thread_Semaphore; 
int count = 50;
extern char* welcome_msg;
char iot_send_buf[128] = {0};
char iot_recev_buf[128] = {0};

/* USART Driver */
extern ARM_DRIVER_USART Driver_USART2;
 
void myUSART_callback(uint32_t event) { 
    uint32_t mask;
    mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
           ARM_USART_EVENT_TRANSFER_COMPLETE |
           ARM_USART_EVENT_SEND_COMPLETE     |
           ARM_USART_EVENT_TX_COMPLETE       ;
    if (event & mask) {
        /* Success: Wakeup Thread */
        memset(iot_send_buf, 0, 128);
    }
    if (event & ARM_USART_EVENT_RX_TIMEOUT) {
        __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
    }
    if (event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_TX_UNDERFLOW)) {
        __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
    }
}
static ARM_DRIVER_USART * USART2drv = &Driver_USART2;

void vTaskLedRed(void *p)
{
    for (;;)
    {
        led_toggle(GPIOE, GPIO_Pin_6);
        Delay(0x1FFFFF);
    }
}

void vTaskEXTILed(void *p)
{
    for (;;)
    {
        osSemaphoreAcquire (sid_Thread_Semaphore, osWaitForever);
        led_toggle(GPIOE, GPIO_Pin_5);
        printf("EXTI Count Value: %d\r\n", count);
        
        strcpy(iot_send_buf, "{\"temperature\":");
        iot_send_buf[strlen(iot_send_buf)] = (count / 10) + 0x30;
        iot_send_buf[strlen(iot_send_buf)] = (count % 10) + 0x30;
        iot_send_buf[strlen(iot_send_buf)] = '}';
        USART2drv->Send(iot_send_buf, strlen(iot_send_buf));
    }
}

void vTaskDebugPort(void *p)
{
    printf("%s", welcome_msg);
    printf("AplexOS # ");
    for (;;)
    {
        get_cmd_parser_char();
        led_toggle(GPIOE, GPIO_Pin_5);
    }
}

void vTaskI2C0(void *p)
{
    i2c_data_transmission();
}


int main(void)
{
    USART1_Config(115200);
    LED_GPIO_Config();
    EXTI_Config();
 
    /*Initialize the USART driver */
    USART2drv->Initialize(myUSART_callback);
    /*Power up the USART peripheral */
    USART2drv->PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 115200 Bits/sec */
    USART2drv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, 9600);
     
    /* Enable Receiver and Transmitter lines */
    USART2drv->Control (ARM_USART_CONTROL_TX, 1);
    USART2drv->Control (ARM_USART_CONTROL_RX, 1);
    
    // char* sendstr = "\r\nPress Enter to receive a message";
    // USART2drv->Send(sendstr, strlen(sendstr));

    // System Initialization
    SystemCoreClockUpdate();
    #ifdef RTE_Compiler_EventRecorder
    // Initialize and start Event Recorder
    EventRecorderInitialize(EventRecordError, 1U);
    #endif

    osKernelInitialize();                               // Initialize CMSIS-RTOS
    
    osThreadNew(vTaskLedRed, NULL, NULL);               // Create application thread
    osThreadNew(vTaskDebugPort, NULL, NULL);            // Create application thread
    // osThreadNew(vTaskI2C0, NULL, NULL);                 // Create application thread

    // osThreadNew(vTaskSPI1, NULL, NULL);              // Create application thread
    
    sid_Thread_Semaphore = osSemaphoreNew(1, 0, NULL);
    if (!sid_Thread_Semaphore) {
        printf("get sid_Thread_Semaphore error.\r\n");  // Semaphore object not created, handle failure
    }
    osThreadNew(vTaskEXTILed, NULL, NULL);              // Create application thread
    
    osKernelStart();                                    // Start thread execution
}
