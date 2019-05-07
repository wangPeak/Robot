#include "USART3.h"

char Uart3_Buffer[Max_BUFF_Len] = {0};
char Uart3_Rx = 0;
char F1 =0;
char F2 =0;
char E1 =0;
char RX_OK = 0;
extern Ctrl car;
extern int S;
void USART3_Init(u32 baud)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;





	//配置串口  
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


	// Configure USART3   
	USART_Init(USART3, &USART_InitStructure);//配置串口3 
	// Enable USART3 Receive interrupts 使能串口接收中断  
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	// Enable the USART3   
	USART_Cmd(USART3, ENABLE);//使能串口3  

	//串口中断配置  
	//Configure the NVIC Preemption Priority Bits     
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// Enable the USART3 Interrupt   
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void USART3_IRQHandler(void)                    //串口3中断服务程序
{
//	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
//	{
//		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//		
//		
//		
//	}
//	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题
//	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) == SET)
//	{
//		USART_ReceiveData(USART3);									//读数据
//		USART_ClearFlag(USART3, USART_FLAG_ORE);		//清除状态标志位
//	}
//	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET) //中断产生 
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE); //清除中断标志

		Uart3_Buffer[Uart3_Rx] = USART_ReceiveData(USART3);     //接收串口3数据到buff缓冲区
		Uart3_Rx++;
		if(F1&&F2)
		{
			if((Uart3_Buffer[Uart3_Rx-1] == (char)0xED) && Uart3_Rx-1 == 15)
			{
				E1 =1;
			}
			else if(Uart3_Rx-1 >15)
			{
				E1 = 0;
				F1 = F2 = 0;
				Uart3_Rx = 0;
			}
		}
		else
		{
			if((Uart3_Buffer[Uart3_Rx-1] == (char)0xFA) && (F1 == 0))
			{
				F1 = 1;
			}
			else if((Uart3_Buffer[Uart3_Rx-1] == (char)0xAF) && (F1 == 1)&& (F2 == 0))
			{
				F2 = 1;
			}else 
			{
				F1 = F2 = 0;
				Uart3_Rx = 0;
			}

		}
		
		
		if(E1 == 1)
		{
			
			F1 = F2 = E1 = Uart3_Rx = 0;		
			
			car.Velocity_LB = Uart3_Buffer[2] & 0xff;
			car.Velocity_LB = car.Velocity_LB<< 8;
			car.Velocity_LB = car.Velocity_LB| Uart3_Buffer[3];

			car.Velocity_LF = Uart3_Buffer[4] & 0xff;
			car.Velocity_LF = car.Velocity_LF<< 8;
			car.Velocity_LF = car.Velocity_LF| Uart3_Buffer[5];

			car.Velocity_RB = Uart3_Buffer[6] & 0xff;
			car.Velocity_RB = car.Velocity_RB<< 8;
			car.Velocity_RB = car.Velocity_RB| Uart3_Buffer[7];

			car.Velocity_RF = Uart3_Buffer[8] & 0xff;
			car.Velocity_RF = car.Velocity_RF<< 8;
			car.Velocity_RF = car.Velocity_RF| Uart3_Buffer[9];

			car.X = Uart3_Buffer[11] & 0xff;
			car.X = car.X<< 8;
			car.X = car.X| Uart3_Buffer[12];

			car.Y = Uart3_Buffer[13] & 0xff;
			car.Y = car.Y<< 8;
			car.Y = car.Y| Uart3_Buffer[14];
			
			RX_OK = 1;
			 //Car_Vector_S(S);
				
		}

	}
			
	
	
	
	

}

void USART3_Sned_Char(u8 temp)
{
	USART_SendData(USART3, (u8)temp);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);

}

void USART3_Sned_Char_Buff(u8 buf[], u32 len)
{
	u32 i;
	for (i = 0; i < len; i++)
		USART3_Sned_Char(buf[i]);


	

}

char Get_IO(char a)
{
	if(a>7)
	{
		printf("Get_IO_Error");
		return 0;
	}
	if(Uart3_Buffer[10]&(1<<a))
	{
		return 1;
	}
	else
	{
		return	0;
	}
}
