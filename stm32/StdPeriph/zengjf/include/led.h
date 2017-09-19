
/******************** 尚学科技 **************************
 * 实验平台：开拓者STM32开发板
 * 库版本  ：ST3.5.0
 * 作者    ：尚学科技团队 
 * 淘宝    ：http://shop102218275.taobao.com/
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *版权所有，盗版必究。
**********************************************************************************/


#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"

#define LED0_OFF GPIO_SetBits(GPIOE,GPIO_Pin_6);     //PE6输出高电平
#define LED0_ON  GPIO_ResetBits(GPIOE,GPIO_Pin_6);   //PE6输出低电平

void LED_GPIO_Config(void);   /* LED 端口初始化 */
void led_toggle(GPIO_TypeDef * gpio, uint16_t pin);

#endif /* __LED_H */
