#include "zengjf.h"

/*
 * 函数名：USART1_Config
 * 描述  ：USART1 GPIO 配置,工作模式配置。115200 8-N-1
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void USART1_Config(int baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* 配置串口1 （USART1） 时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    /*串口GPIO端口配置*/
    /* 配置串口1 （USART1 Tx (PA.09)）*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    
    /* 配置串口1 USART1 Rx (PA.10)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 串口1工作模式（USART1 mode）配置 */
    USART_InitStructure.USART_BaudRate = baudrate;    //一般设置为115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure); 
    USART_Cmd(USART1, ENABLE);//使能串口
}

/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
int fputc(int ch, FILE *f)
{
    /* 将Printf内容发往串口 */
    USART_SendData(USART1, (unsigned char) ch);
    while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);  
    
    led_toggle(GPIOE, GPIO_Pin_6);
    
    return (ch);
}

/*
 * 函数名：fputc
 * 描述  ：重定向c库函数scanf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由scanf调用
 */
int fgetc(FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    
    led_toggle(GPIOE, GPIO_Pin_6);
    
    int ch = (int)USART_ReceiveData(USART1); 

    if (ch == '\r' || ch == '\n')
    {
        fputc('\r', f);
        fputc('\n', f);
    } else {
        fputc(ch, f);
    }
    
    return ch;
}
