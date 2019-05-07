#include "main.h" 
#include "stdio.h"
short MAX_PWM = 5000;  																//用于限制驱动器电压的PWM   5000表示  以最大电压运行

#define    LB    			25  
#define    LB_TIME    3  
int LB_Sum = 0;
char SD = 0;
extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];  	//存放读取到的ADC值

int sun = 0;

Ctrl car ;																						//车辆位移向量控制全局变量

void Timerx_Init(u16 arr)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = (arr-1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值，计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
     
    TIM_ITConfig(  //使能或者失能指定的TIM中断
        TIM3, //TIM2
        TIM_IT_Update  |  //TIM 中断源
        TIM_IT_Trigger,   //TIM 触发中断源 
        ENABLE  //使能
        );
     
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
                             
}


void TIM3_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
        {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
			
						
		
					
						car = Car_Vector_P();

						if(abs(car.Velocity_LB) < LB && abs(car.Velocity_RB) < LB && abs(car.Velocity_LF) < LB && abs(car.Velocity_RF) < LB )
						{
							car.Velocity_LB = 0;
							car.Velocity_RB = 0;
							car.Velocity_LF = 0;
							car.Velocity_RF = 0;
							LB_Sum = 0;
						}
						else if (LB_Sum<LB_TIME+1)
						{
							LB_Sum++;
						}
						if(LB_Sum<LB_TIME)
						{
							car.Velocity_LB = 0;
							car.Velocity_RB = 0;
							car.Velocity_LF = 0;
							car.Velocity_RF = 0;
						}
					
					
					short X = 0x01F0;
					u8 String[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
					String[0] = (char)0xFA;
					String[1] = (char)0xAF;
					String[3] =  car.Velocity_LB & 0xff;
					String[2] = (car.Velocity_LB >> 8) & 0xff;
					String[5] =  car.Velocity_LF & 0xff;
					String[4] = (car.Velocity_LF >> 8) & 0xff;
					String[7] =  car.Velocity_RB & 0xff;
					String[6] = (car.Velocity_RB >> 8) & 0xff;
					String[9] =	 car.Velocity_RF & 0xff;
					String[8] = (car.Velocity_RF >> 8) & 0xff;

					String[10] = String[10] | ((GPIOA->IDR & X)>>4);
					
					String[12] =  car.X & 0xff;
					String[11] = (car.X >> 8) & 0xff;
					String[14] =  car.Y & 0xff;
					String[13] = (car.Y >> 8) & 0xff;
					String[15] = (char)0xED;
					//
					
					
					if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8))
					{
						USART3_Sned_Char_Buff(String,16);
						SD = 0;
					}
					if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) && SD <= 5)
					{
						USART3_Sned_Char_Buff(String,16);
						SD++;
					}
					

//					printf("%d\r\n",car.Velocity_RF);
//					printf("%d\r\n",car.Velocity_RB);
//					printf("%d\r\n",car.Velocity_LB);
//					printf("%d\r\n\r\n",car.Velocity_LF);
					
					
					

//					if(b<=4000 && GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == Bit_RESET)
//					{
//						GPIO_SetBits(GPIOC,GPIO_Pin_7);
//								if(a == 0)
//							{
//								GPIO_SetBits(GPIOC,GPIO_Pin_6);
//								
//								a = 1;
//							}
//							else
//							{
//								GPIO_ResetBits(GPIOC,GPIO_Pin_6);
//								b++;
//								a = 0;
//							}
////						GPIO_SetBits(GPIOB,GPIO_Pin_12);
////						GPIO_ResetBits(GPIOB,GPIO_Pin_12);
////						b++;
//					}
//					else if(b >= 1 && GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == Bit_SET)
//					{
//						
//						
////						GPIO_SetBits(GPIOB,GPIO_Pin_12);
////						GPIO_ResetBits(GPIOB,GPIO_Pin_12);
////						b--;
//						GPIO_ResetBits(GPIOC,GPIO_Pin_7);
//						if(a == 0)
//							{
//								GPIO_SetBits(GPIOC,GPIO_Pin_6);
//								
//								a = 1;
//							}
//							else
//							{
//								GPIO_ResetBits(GPIOC,GPIO_Pin_6);
//								b--;
//								a = 0;
//							}
//					}
					
//					
//					
//					
//					
//					if(b1<=400 && GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == Bit_RESET)
//					{
//						GPIO_SetBits(GPIOB,GPIO_Pin_13);
//								if(a1 == 0)
//							{
//								GPIO_SetBits(GPIOB,GPIO_Pin_12);
//								
//								a1 = 1;
//							}
//							else
//							{
//								GPIO_ResetBits(GPIOB,GPIO_Pin_12);
//								b1++;
//								a1 = 0;
//							}
////						GPIO_SetBits(GPIOB,GPIO_Pin_12);
////						GPIO_ResetBits(GPIOB,GPIO_Pin_12);
////						b++;
//					}
//					else if(b1 >= 1 && GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == Bit_SET)
//					{
//						
//						
////						GPIO_SetBits(GPIOB,GPIO_Pin_12);
////						GPIO_ResetBits(GPIOB,GPIO_Pin_12);
////						b--;
//						GPIO_ResetBits(GPIOB,GPIO_Pin_13);
//						if(a1 == 0)
//							{
//								GPIO_SetBits(GPIOB,GPIO_Pin_12);
//								
//								a1 = 1;
//							}
//							else
//							{
//								GPIO_ResetBits(GPIOB,GPIO_Pin_12);
//								b1--;
//								a1 = 0;
//							}
//					}
//					
//					
					
					

        
        }
}

void RCC_Config(void)
{
        //??GPIOA,TIM2
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
}
void Init_Wifi(void)
{
//	printf("AT+CIPMUX=0\r\n");
//	printf("AT+CIPSTART=\"UDP\",\"192.168.123.69\",6300,6300\r\n");
//	printf("AT+CIPMODE=1\r\n");
//	printf("AT+CIPSEND\r\n");
//	
		
					u8 String1[] = "AT+CIPMUX=0\r\n";
					u8 String2[] = "AT+CIPSTART=\"UDP\",\"192.168.123.69\",6300,6300\r\n";
					u8 String3[] = "AT+CIPSEND=1\r\n";
					u8 String4[] = "T\r\n";
					u8 String5[] = "AT+CIPMODE=1\r\n";
					u8 String6[] = "AT+CIPSEND\r\n";
					USART3_Sned_Char_Buff(String1,14);
					delay_ms(100);
					USART3_Sned_Char_Buff(String2,47);
					delay_ms(100);	
					USART3_Sned_Char_Buff(String3,15);
					delay_ms(100);
					USART3_Sned_Char_Buff(String4,4);
					delay_ms(100);
					USART3_Sned_Char_Buff(String5,15);
					delay_ms(100);
					USART3_Sned_Char_Buff(String6,13);
					delay_ms(100);

}

void GPIO_Config(void)                            
{

		GPIO_InitTypeDef GPIO_InitStructure;    
    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);                   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        
    GPIO_Init(GPIOB, &GPIO_InitStructure);   
  
		GPIO_InitStructure.GPIO_Pin = 0; 
		GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_6 | GPIO_Pin_7); 	
		GPIO_Init(GPIOC, &GPIO_InitStructure);  
	
		GPIO_InitStructure.GPIO_Pin = 0; 
		GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_13); 	
		GPIO_Init(GPIOG, &GPIO_InitStructure); 
		  
		GPIO_InitStructure.GPIO_Pin = 0; 
		GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
		
		GPIO_InitStructure.GPIO_Pin = 0; 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 	     		//初始化串口三TX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
		GPIO_Init(GPIOB, &GPIO_InitStructure);   
		
		GPIO_InitStructure.GPIO_Pin = 0; 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  				//初始化串口三RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_Init(GPIOB, &GPIO_InitStructure); 	


  
}


void TIM_Config(void)
{
        TIM_TimeBaseInitTypeDef TIM_TImeBaseStructure;
        TIM_OCInitTypeDef  TIM_OCInitStructure;

    //??TIM2???????,???????????
        TIM_TImeBaseStructure.TIM_Prescaler=360-1;//??PWM???
        TIM_TImeBaseStructure.TIM_CounterMode=0;
        TIM_TImeBaseStructure.TIM_Period=4000;
        TIM_TimeBaseInit(TIM2,&TIM_TImeBaseStructure);


    //??PWM?????
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
        TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
        TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;   
    //?????
        TIM_OCInitStructure.TIM_Pulse=100;//3900-3500

        TIM_OC2Init(TIM2,&TIM_OCInitStructure);                                                                                                                                                                                                                                                                                                                                                            
        TIM_ForcedOC1Config(TIM2,TIM_ForcedAction_Active);
        TIM_Cmd(TIM2,ENABLE);
        TIM_CtrlPWMOutputs(TIM2,ENABLE);

}


void TIM1_PWM_Init(u16 arr,u16 psc)
{  
     GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);// 使能TIM1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能GPIO外设时钟使能


   //设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值    80K
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位


    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

    TIM_CtrlPWMOutputs(TIM1,ENABLE);    //MOE 主输出使能 

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能    

    TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器

    TIM_Cmd(TIM1, ENABLE);  //使能TIM1


}
void SetAngle(double Angle)
{
	double Value = (400.0/180.0)*Angle+100.0;
	
	
	TIM_SetCompare2(TIM2,Value);
}







void Motor_Init(u16 TIM2per, u16 TIM3per, u16 TIM3Compare1)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
  GPIO_InitTypeDef GPIO_InitStruct;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//使能定时器2的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//使能定时器3的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能复用IO时钟
  



GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
  GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_0|GPIO_Pin_1);//PA0
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStruct);
  
  //TIM2工作在单脉冲下
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//没有时钟分割
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
  TIM_TimeBaseInitStruct.TIM_Prescaler = 7200;//预分频值,每100us计数一次
  TIM_TimeBaseInitStruct.TIM_Period = TIM2per;//重装值
  TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
  
  TIM_SelectOnePulseMode(TIM2,TIM_OPMode_Single);//设置TIM2在单脉冲模式，且是单一的脉冲，在下一个更新事件停止
  TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);//使能定时器2的通道1预装载寄存器
	
  TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_OC1Ref);
	
	
	//TIM_SelectOnePulseMode(TIM2,TIM_OPMode_Single);//设置TIM2在单脉冲模式，且是单一的脉冲，在下一个更新事件停止
  TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);//使能定时器2的通道2预装载寄存器
	
  TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_OC2Ref);
  
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
  TIM_OCInitStruct.TIM_Pulse = 1;//比较捕获1的预装载值
  TIM_OC1Init(TIM2,&TIM_OCInitStruct);
	
	 TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
  TIM_OCInitStruct.TIM_Pulse = 1;//比较捕获1的预装载值
  TIM_OC2Init(TIM2,&TIM_OCInitStruct);
  
  TIM_Cmd(TIM2,DISABLE);//先不使能能TIM2
  
  
  //TIM3工作在从模式的门控模式下的PWM输出模式
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//没有时钟分割
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
  TIM_TimeBaseInitStruct.TIM_Prescaler = 720;//预分频值，10us计数一次
  TIM_TimeBaseInitStruct.TIM_Period = TIM3per;//重装值
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
  
  TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);//TIM3为门控模式
  TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);//使能TIM3的主从模式
  TIM_SelectInputTrigger(TIM3,TIM_TS_ITR1);//内部触发，从TIM2触发
  
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
  TIM_OCInitStruct.TIM_Pulse = TIM3Compare1;//比较捕获1的预装载值
  TIM_OC1Init(TIM3,&TIM_OCInitStruct);
  
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
  TIM_OCInitStruct.TIM_Pulse = TIM3Compare1;//比较捕获1的预装载值
  TIM_OC2Init(TIM3,&TIM_OCInitStruct);

  TIM_Cmd(TIM3,ENABLE);//使能TIM3
}

//设置PWM的输出
//Cycle:为周期,单位(us)
//Pulse_Num:为脉冲个数（小于3200）
void TIM2_TIM3_PWM(u16 Cycle, u16 Pulse_Num,char i )
{
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	if(i == 0)
	{
				TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
				TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
				TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
				TIM_OCInitStruct.TIM_Pulse = 0;//比较捕获1的预装载值
				TIM_OC2Init(TIM3,&TIM_OCInitStruct);
				TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
				TIM_OCInitStruct.TIM_OutputState = TIM_OutputNState_Disable;//OC1输出使能
				TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
				TIM_OCInitStruct.TIM_Pulse = 0;//比较捕获1的预装载值
				TIM_OC1Init(TIM3,&TIM_OCInitStruct);
	}
	else if(i == 1)
		{
	
				TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
				TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
				TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
				TIM_OCInitStruct.TIM_Pulse = 0;//比较捕获1的预装载值
				TIM_OC1Init(TIM3,&TIM_OCInitStruct);
				TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
				TIM_OCInitStruct.TIM_OutputState = TIM_OutputNState_Disable;//OC1输出使能
				TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
				TIM_OCInitStruct.TIM_Pulse = 0;//比较捕获1的预装载值
				TIM_OC2Init(TIM3,&TIM_OCInitStruct);
	}
	
	
  u16 TIM3per = 0;
  u32 Time = 0;
  //改变TIM3的重装值改变脉冲频率这里设置脉冲占空比为50%
  //改变TIM2的预分频器和重装值改变脉冲个数
    
  Time = Cycle * Pulse_Num;
  Time /= 100;              //预分频为7200,100us计数一次
  TIM3per = Cycle/10;       //预分频为720,10us计数一次
  
  TIM_SetAutoreload(TIM2, Time+1);//设置TIM2的重装值
  TIM_SetAutoreload(TIM3, TIM3per-1);//设置TIM3的重装值
  TIM_SetCompare1(TIM3,TIM3per/2);//设置占空比为50%
	
  TIM_Cmd(TIM2,ENABLE);//使能TIM2
}


int main()
{
	
		
		RCC_Config();																				//使能系统时钟
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		GPIO_Config();																			//配置GPIO
		GPIO_SetBits(GPIOG,GPIO_Pin_13);
		USART3_Init(115200);											

		printf_init();
		Ctrl_Init();
		delay_ms(1000);
		Init_Wifi();
		car = Car_Vector_P();																//
		Timerx_Init(280);
		
	//Motor_Init(0, 0, 0);
	//TIM2_TIM3_PWM(500, 400,0);

//		Ctrl_Init();
//    CAN1_Configuration();                               //CAN1初始化
//    delay_ms(500);                                      //刚开始要有足够的延时，确保驱动器已经初始化完成 
//    CAN_RoboModule_DRV_Reset(0,0);                      //对0组所有驱动器进行复位 
//    delay_ms(500);                                      //发送复位指令后的延时必须要有，等待驱动器再次初始化完成
//    CAN_RoboModule_DRV_Config(0,1,100,0);               //1号驱动器配置为100ms传回一次数据
//    delay_us(200);                                      //此处延时为了不让传回数据时候4个不一起传
//    CAN_RoboModule_DRV_Config(0,2,100,0);               //2号驱动器配置为100ms传回一次数据
//    delay_us(200);                                      //此处延时为了不让传回数据时候4个不一起传
//    CAN_RoboModule_DRV_Config(0,3,100,0);               //3号驱动器配置为100ms传回一次数据
//    delay_us(200);                                      //此处延时为了不让传回数据时候4个不一起传
//    CAN_RoboModule_DRV_Config(0,4,100,0);               //3号驱动器配置为100ms传回一次数据   
//    CAN_RoboModule_DRV_Mode_Choice(0,0,Velocity_Mode);  //0组的所有驱动器 都进入速度模式
//    delay_ms(500);                                      //发送模式选择指令后，要等待驱动器进入模式就绪。所以延时也不可以去掉。

//		short NowADC_To_Velocity_X = 0;												//本次转换后的速度   由于每次读取都不一样  所以只读取一次 存起来
//		short NowADC_To_Velocity_Y = 0;												//本次转换后的速度   由于每次读取都不一样  所以只读取一次 存起来
//		short NowADC_To_Velocity_P = 0;
		//Ctrl_Init();
	

	
		while(1)
		{
			
			
			
			
						

			

					
				//USART3_Sned_Char(a);
			//printf("%c",a);

//				delay_ms(100);
//			//abs(temp-ADC_To_Velocity(ADC_ConvertedValue[0]))
//			NowADC_To_Velocity_X = ADC_To_Velocity(ADC_ConvertedValue[1]);
//			NowADC_To_Velocity_Y = ADC_To_Velocity(ADC_ConvertedValue[2]);
//			NowADC_To_Velocity_P = ADC_To_Velocity(ADC_ConvertedValue[3]);
//			if(abs(Velocity-NowADC_To_Velocity_X)>200)				//Velocity-NowADC_To_Velocity 可以理解为 上一次的速度与本次速度的差，不能太大 波动太大对电机不好  就是要慢慢加减速的意思
//			{
//				if(Velocity-NowADC_To_Velocity_X<200)
//				{
//					Velocity += 100;
//				}
//				else
//				{
//					Velocity -= 100;
//				}
//			}else{
//				if	(abs(NowADC_To_Velocity_X) < 100)
//				{
//					Velocity = 0;
//				}
//				else
//				{
//					Velocity = NowADC_To_Velocity_X;
//				}
//				
//			}
//			
////        CAN_RoboModule_DRV_Velocity_Mode(0,1,MAX_PWM,Velocity);
////			printf("NowADC_To_Velocity = %d++++++++++++++++++\r\n",);
////			printf("Velocity = %d-----------------\r\n",Velocity);//
////			
////			Ctrl ctrl = Car_Vector();
////			
//			printf("%d\r\n",car.Velocity_LF);
//			printf("%d\r\n",car.Velocity_LB);
//			printf("%d\r\n",car.Velocity_RB);
//			printf("%d\r\n\r\n",car.Velocity_RF);

			//delay_ms(500);
//		CAN_RoboModule_DRV_Velocity_Mode(0,0,MAX_PWM,car.);
//		CAN_RoboModule_DRV_Velocity_Mode(0,1,MAX_PWM,Velocity);
//		CAN_RoboModule_DRV_Velocity_Mode(0,2,MAX_PWM,Velocity);
//		CAN_RoboModule_DRV_Velocity_Mode(0,3,MAX_PWM,Velocity);  //2-3
//			CAN_RoboModule_DRV_Velocity_Mode(0,1,MAX_PWM,car.Velocity_LF);
//			CAN_RoboModule_DRV_Velocity_Mode(0,2,MAX_PWM,-car.Velocity_LB);
//			CAN_RoboModule_DRV_Velocity_Mode(0,3,MAX_PWM,-car.Velocity_RB);
//			CAN_RoboModule_DRV_Velocity_Mode(0,4,MAX_PWM,car.Velocity_RF);

				
		}
			
	//TIM1_PWM_Init(899,0);//不分频。PWM频率=72000/(899+1)=80Khz 


}
























