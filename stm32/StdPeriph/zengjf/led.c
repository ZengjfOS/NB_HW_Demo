#include "zengjf.h"

/*
 * 函数名：LED_GPIO_Config
 * 描述  ：配置LED用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void LED_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6 | GPIO_Pin_5;            
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  
	
    GPIO_Init(GPIOE,&GPIO_InitStructure); 
}

/*******************************************
 * Toggle LED 
 *******************************************/
void led_toggle(GPIO_TypeDef * gpio, uint16_t pin)
{
    /* Read LED output (GPIOA PIN8) status */
    uint8_t led_bit = GPIO_ReadOutputDataBit(gpio, pin);
     
    /* If LED output set, clear it */
    if(led_bit == (uint8_t)Bit_SET)
    {
        GPIO_ResetBits(gpio, pin);
    }
    /* If LED output clear, set it */
    else
    {
        GPIO_SetBits(gpio, pin);
    }
}
