#ifndef __USART3_H
#define __USART3_H
#include "main.h"
#define    Max_BUFF_Len     16
void USART3_Init(u32 baud);
void USART3_IRQHandler(void);
void USART3_Sned_Char(u8 temp);
void USART3_Sned_Char_Buff(u8 buf[], u32 len);
char Get_IO(char a);
#endif
