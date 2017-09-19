#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

void USART1_Config(int baudrate);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);

#endif /* __USART1_H */
