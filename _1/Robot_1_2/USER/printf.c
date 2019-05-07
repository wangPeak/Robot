
#include "printf.h"

#include "stm32f10x.h"	   

#include "stm32f10x_rcc.h"

#include "stm32f10x_gpio.h"

#include "stm32f10x_usart.h"	 

#include "misc.h"	

int fputc(int ch,FILE *f)

{  

    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET); 

    USART_SendData(USART1,(unsigned char)ch);    

    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);  

    return (ch);  

}

 

void printf_init(void)

{

    GPIO_InitTypeDef GPIO_InitStructure;  

    USART_InitTypeDef USART_InitStructure;

    

       /*config	USART clock*/

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE); 

//    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);

       /*USART1 GPIO config*/	

    GPIO_InitStructure.GPIO_Pin= GPIO_Pin_9;  

    GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF_PP; //复用推挽输出  

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  

    GPIO_Init(GPIOA,&GPIO_InitStructure); 

	

	  GPIO_InitStructure.GPIO_Pin= GPIO_Pin_10;  

    GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN_FLOATING;  //复用开漏输入  

    GPIO_Init(GPIOA,&GPIO_InitStructure); 

       /*USART1 mode Config*/	

    USART_InitStructure.USART_BaudRate = 115200;

    USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    USART_InitStructure.USART_StopBits = USART_StopBits_1;

    USART_InitStructure.USART_Parity = USART_Parity_No;

    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1,&USART_InitStructure);

    USART_Cmd(USART1,ENABLE); 

		
}
 

