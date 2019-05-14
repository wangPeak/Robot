//自启动分支
#include "main.h" 

short MAX_PWM = 5000;  																//用于限制驱动器电压的PWM   5000表示  以最大电压运行
extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];  	//存放读取到的ADC值
extern Ctrl car ;
extern char RX_OK ;


	
Ctrl car_A ; //自动控制向量
char Init = 0;   //初始化完成标志位


double QA = 0.0;  //设置角度
double A = 0.0;  //当前运动角度


char a = 0;			//切换电平
int PULSE_NOW = 0;//当前脉冲数
char a1 = 0;
int b1 = 0;
double R_NOW = 0;
short XW_NOW_X = 0;
short XW_NOW_Y = 0;
short XW_NOW_TIME = 0;

#define R  1050                //650//平移倒角半径
#define PI 3.1415926
#define PULSE_1 4000						//目标脉冲数_1
#define PULSE_2 4000						//目标脉冲数_2
#define Opposite  +										//         	+		-      	//取反方向
#define Opposite_XW  <								//					<		>				//取反限位   限位应与方向相对应



// TIM4 输出比较通道1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            GENERAL_TIM_CH1_PORT          GPIOB
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6

// TIM4 输出比较通道2
#define            GENERAL_TIM_CH2_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            GENERAL_TIM_CH2_PORT          GPIOB
#define            GENERAL_TIM_CH2_PIN           GPIO_Pin_7


//#define            uint16_t CCR1_Val             pwm1
#define            GENERAL_TIM                   TIM4
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM4
#define            GENERAL_TIM_Period            999
#define            GENERAL_TIM_Prescaler         1439


  uint16_t CCR1_Val = 25;
	uint16_t CCR2_Val = 4;
	
	#define            Speed         100
	
	long 	P1 = 0;  //
	long	P2 = 0;
	long	P3 = 0;
	char Mode = 0;  //表示四足运行状态
	char Init_1_Flag = 0;
	char Init_4_Flag = 0;
	char Init_5_Flag = 0;
	char Init_0_Flag = 0;
	short SD = 600;
	short Init_time = 0;
	long X = 0;
	long Y = 0;
	int Key = 0;
	long  JDZ = 0;
	int RST = 0;
	char make = 0;			//等待行至山顶区标志
void Init_1()	  												//初始化正转
{


							int P1C	= P1 % 64000;
							int P2C = P2 % 32000;
							int P3C = P3 % 32000;

							
							if(P2C<16000)
							{
								P2 -= P2C;
							}else
							{
								P2 += (32000 - P2C);
								
							}
						
								if(P3C<16000)
							{
								P3 -= P3C;
							}else
							{
								P3 += (32000 - P3C);
								
							}
							
				
							
							P1 -= P1C;
							if(P1>=0)
							{
								P1 += 32000;
							}
							else
							{
								P1 -= 32000;
							}
								
		
							
							CAN_RoboModule_DRV_Velocity_Position_Mode(0,1,MAX_PWM,200,P1);
							CAN_RoboModule_DRV_Velocity_Position_Mode(0,2,MAX_PWM,100,-P2);
							CAN_RoboModule_DRV_Velocity_Position_Mode(0,3,MAX_PWM,100,P3);
							delay_ms(1);
							Init_time = 0;
	
}
void Init_4_5()	  												//初始化左右转
{
							Init_1();
							P1 -= 8000;
							P2 += 8000;
							P3 -= 8000;
							CAN_RoboModule_DRV_Velocity_Position_Mode(0,1,MAX_PWM,200,P1);
							CAN_RoboModule_DRV_Velocity_Position_Mode(0,2,MAX_PWM,100,-P2);
							CAN_RoboModule_DRV_Velocity_Position_Mode(0,3,MAX_PWM,100,P3);
							delay_ms(1);
							Init_time = 0;
}
	
void Timer5_Init(u16 arr)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = (arr-1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值，计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
     
    TIM_ITConfig(  //使能或者失能指定的TIM中断
        TIM5, //TIM2
        TIM_IT_Update  |  //TIM 中断源
        TIM_IT_Trigger,   //TIM 触发中断源 
        ENABLE  //使能
        );
     
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM5中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM5, ENABLE);  //使能TIMx外设
                             
}
	
void Timer3_Init(u16 arr)
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
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
                             
}


void TIM3_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
        {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
					

					
					
					
				
					if(Init == 1)
					{
						if(Mode == 0)						//停止
						{
							
						}
						else if(Mode == 1)			//正转
						{
							if(Init_1_Flag == 0)
							{
								Init_1();
								Init_1_Flag = 1;
								Init_4_Flag = 0;
								Init_5_Flag = 0;
							}
							
							if(Init_time >= 200)
							{
								P1+=Speed*2;
								P2+=Speed;
								P3+=Speed;
							}
							
						}
						else if(Mode == 2)			//反转
						{
		
						}
						else if(Mode == 3)			//复位
						{
								Init_1();
								Init_1_Flag = 0;
								Init_4_Flag = 0;
								Init_5_Flag = 0;
						}
						else if(Mode == 4)		//左转
						{
							if(Init_4_Flag == 0)
							{
								Init_4_5();
								Init_1_Flag = 0;
								Init_4_Flag = 1;
								Init_5_Flag = 0;
							}
							if(Init_time >= 200)
							{
								P2 +=Speed;
								P3 -=Speed;
							}
						}
						else if(Mode == 5)		//右转
						{
							if(Init_5_Flag == 0)
							{
								Init_4_5();
								Init_1_Flag = 0;
								Init_4_Flag = 0;
								Init_5_Flag = 1;
							}
							if(Init_time >= 200)
							{
								P2 -=Speed;
								P3 +=Speed;
							}
							
						}
							
						
						
							CAN_RoboModule_DRV_Velocity_Position_Mode(0,1,MAX_PWM,SD*2,P1);
							CAN_RoboModule_DRV_Velocity_Position_Mode(0,2,MAX_PWM,SD,-P2);
							CAN_RoboModule_DRV_Velocity_Position_Mode(0,3,MAX_PWM,SD,P3);
						if(Init_time<=200)
						{
							Init_time++;
						}
					}
				

        
        }
}

void TIM5_IRQHandler(void)   //TIM5中断
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
        {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
					if(Init == 1 && RX_OK)
					{
							
							if(PULSE_NOW<=PULSE_1 && Get_IO(3))
							{
									GPIO_SetBits(GPIOC,GPIO_Pin_7);
									if(a == 0)
									{
											GPIO_SetBits(GPIOC,GPIO_Pin_6);
											a = 1;
									}
									else
									{
											GPIO_ResetBits(GPIOC,GPIO_Pin_6);
											PULSE_NOW++;
											a = 0;
									}

							}
							else if(PULSE_NOW >= 1 && !Get_IO(3))
							{
									GPIO_ResetBits(GPIOC,GPIO_Pin_7);
									if(a == 0)
									{
											GPIO_SetBits(GPIOC,GPIO_Pin_6);
											a = 1;
									}
									else
									{
											GPIO_ResetBits(GPIOC,GPIO_Pin_6);
											PULSE_NOW--;
											a = 0;
									}
							}
							//					else{
							//						TIM_Cmd(TIM5, DISABLE);
							//					}
					}
				
							
					
					
					
					
					

					
					
					
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

//					}
//					else if(b1 >= 1 && GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == Bit_SET)
//					{
//						
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
					
					
					
					

        
        }
}


void RCC_Config(void)
{
        //??GPIOA,TIM2
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
        //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
				//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
}


void Init_Wifi(void)
{

	u8 String1[ ] = "AT+CIPMUX=0\r\n";
	u8 String2[ ] = "AT+CIPSTART=\"UDP\",\"192.168.123.169\",6300,6300,0\r\n";
	u8 String3[ ] = "AT+CIPSEND=1\r\n";
	u8 String5[ ] = "A\r\n";
	u8 String6[ ] = "AT+CIPMODE=1\r\n";
	USART3_Sned_Char_Buff(String1,14);
	delay_ms(200);
	USART3_Sned_Char_Buff(String2,50);
	delay_ms(500);
	USART3_Sned_Char_Buff(String3,15);
	delay_ms(200);
	USART3_Sned_Char_Buff(String5,4);
	delay_ms(200);
	USART3_Sned_Char_Buff(String6,15);
	delay_ms(200);

}

void GPIO_Config(void)                            
{

//		GPIO_InitTypeDef GPIO_InitStructure;    
//    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);                   
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        
//    GPIO_Init(GPIOB, &GPIO_InitStructure);   
//  
//		GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_6 | GPIO_Pin_7); 	
//		GPIO_Init(GPIOC, &GPIO_InitStructure);   
//		
//		GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_13); 	
//		GPIO_Init(GPIOG, &GPIO_InitStructure);   
//	
//		GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//		GPIO_Init(GPIOA, &GPIO_InitStructure);   
//		
	
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
		GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
		
		GPIO_InitStructure.GPIO_Pin = 0; 
		GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_8 | GPIO_Pin_15);     //定位限位开关 8 == X; 15 == Y;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOG, &GPIO_InitStructure); 
		


  
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


void SetAngle(double Angle)
{
	double Value = (400.0/180.0)*Angle+100.0;
	
	
	TIM_SetCompare2(TIM3,Value);
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
void TIM2_TIM3_PWM(u16 Cycle, u16 Pulse_Num,char i )
{
	//设置PWM的输出
	//Cycle:为周期,单位(us)
	//Pulse_Num:为脉冲个数（小于3200）
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



void TIM3_PWM_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef     GPIO_InitStrue;
    TIM_OCInitTypeDef     TIM_OCInitStrue;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;
    
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);        //使能TIM3和相关GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);// 使能GPIOB时钟(LED在BP5引脚),使能AFIO时钟(定时器3通道2需要重映射到BP5引脚)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_5;     // TIM_CH2
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // 复用推挽
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
    GPIO_Init(GPIOB,&GPIO_InitStrue);                //GPIO端口初始化设置
    
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
    
    TIM_TimeBaseInitStrue.TIM_Period=arr;    //设置自动重装载值
    TIM_TimeBaseInitStrue.TIM_Prescaler=psc;        //预分频系数
    TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up;    //计数器向上溢出
    TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1;        //时钟的分频因子，起到了一点点的延时作用，一般设为TIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStrue);        //TIM3初始化设置(设置PWM的周期)
    
    TIM_OCInitStrue.TIM_OCMode=TIM_OCMode_PWM2;        // PWM模式2:CNT>CCR时输出有效
    TIM_OCInitStrue.TIM_OCPolarity=TIM_OCPolarity_High;// 设置极性-有效为高电平
    TIM_OCInitStrue.TIM_OutputState=TIM_OutputState_Enable;// 输出使能
    TIM_OC2Init(TIM3,&TIM_OCInitStrue);        //TIM3的通道2PWM 模式设置

    TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);        //使能预装载寄存器
    
    TIM_Cmd(TIM3,ENABLE);        //使能TIM3
    
	}

static void GENERAL_TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // 输出比较通道1 GPIO 初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH1_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStructure);
	
	// 输出比较通道2 GPIO 初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH2_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH2_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH2_PORT, &GPIO_InitStructure);
	
	// 输出比较通道3 GPIO 初始化
	//RCC_APB2PeriphClockCmd(GENERAL_TIM_CH3_GPIO_CLK, ENABLE);
  //GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH3_PIN;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 // GPIO_Init(GENERAL_TIM_CH3_PORT, &GPIO_InitStructure);
	
	// 输出比较通道4 GPIO 初始化
	//RCC_APB2PeriphClockCmd(GENERAL_TIM_CH4_GPIO_CLK, ENABLE);
  //GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH4_PIN;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_Init(GENERAL_TIM_CH4_PORT, &GPIO_InitStructure);
}
 void GENERAL_TIM_Mode_Config(void)
{
  // 开启定时器时钟,即内部时钟CK_INT=72M
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);

/*--------------------时基结构体初始化-------------------------*/
	// 配置周期，这里配置为100K
	
 
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_Period;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_Prescaler;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

	/*--------------------输出比较结构体初始化-------------------*/	
	// 占空比配置
	//uint16_t CCR1_Val = 60;
	//uint16_t CCR2_Val = 4;
	//uint16_t CCR3_Val = 3;
	//uint16_t CCR4_Val = 2;
	
	
	// 配置为PWM模式1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// 输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 输出通道电平极性配置	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	// 输出比较通道 1
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	// 输出比较通道 2
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
	TIM_OC2Init(GENERAL_TIM, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	// 输出比较通道 3
	///TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
	//TIM_OC3Init(GENERAL_TIM, &TIM_OCInitStructure);
	//TIM_OC3PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	// 输出比较通道 4
//	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
//	TIM_OC4Init(GENERAL_TIM, &TIM_OCInitStructure);
	//TIM_OC4PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	// 使能计数器
	TIM_Cmd(GENERAL_TIM, ENABLE);
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_GPIO_Config();
	GENERAL_TIM_Mode_Config();	
 
}

void OPEN1(void)
{
  //GENERAL_TIM_GPIO_Config();  
	TIM_SetCompare1(TIM4, 45);
	
}
void OPEN2(void)
{
  //GENERAL_TIM_GPIO_Config();  
	TIM_SetCompare2(TIM4, 45);
	
}
void CLOSE1(void)
{
	//GENERAL_TIM_GPIO_Config();	
	TIM_SetCompare1(TIM4, 25);

}
void CLOSE2(void)
{
	//GENERAL_TIM_GPIO_Config();	
	TIM_SetCompare2(TIM4, 25);

}
int main()
{
	
		
		RCC_Config();																				//使能系统时钟
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		GPIO_Config();																			//配置GPIO
		printf_init();	
		Timer3_Init(100);																		//驱动中断
		//Timer5_Init(5);																	//步进驱动定时器中断中断初始化	
		//GENERAL_TIM_Init();																	//PWM 初始化
    CAN1_Configuration();                               //CAN1初始化
    delay_ms(500);                                      //刚开始要有足够的延时，确保驱动器已经初始化完成 
    CAN_RoboModule_DRV_Reset(0,0);                      //对0组所有驱动器进行复位 
    delay_ms(500);                                      //发送复位指令后的延时必须要有，等待驱动器再次初始化完成
    CAN_RoboModule_DRV_Config(0,1,250,0);               //1号驱动器配置为100ms传回一次数据
    delay_us(200);                                      //此处延时为了不让传回数据时候4个不一起传
    CAN_RoboModule_DRV_Config(0,2,250,0);               //2号驱动器配置为100ms传回一次数据
    delay_us(200);                                      //此处延时为了不让传回数据时候4个不一起传
    CAN_RoboModule_DRV_Config(0,3,250,0);               //3号驱动器配置为100ms传回一次数据
    delay_us(200);                                      //此处延时为了不让传回数据时候4个不一起传
   // CAN_RoboModule_DRV_Config(0,4,250,0);               //3号驱动器配置为100ms传回一次数据   
    CAN_RoboModule_DRV_Mode_Choice(0,0,Velocity_Position_Mode);  //0组的所有驱动器 都进入位置模式
    delay_ms(500);                                      //发送模式选择指令后，要等待驱动器进入模式就绪。所以延时也不可以去掉。
								P1 = 0;
								P2 = 16000;
								P3 = 16000;
								CAN_RoboModule_DRV_Velocity_Position_Mode(0,1,MAX_PWM,200,P1);
								CAN_RoboModule_DRV_Velocity_Position_Mode(0,2,MAX_PWM,100,-P2);
								CAN_RoboModule_DRV_Velocity_Position_Mode(0,3,MAX_PWM,100,P3);
								Init_0_Flag = 1;
								Init_1_Flag = 0;
								Init_4_Flag = 0;
								Init_5_Flag = 0;
		Init = 1;
		
		while(1)
		{
			

				switch(Key)
				{
					
					case 0:
					{
						
						if(Init_0_Flag == 0)
						{
							Mode = 3;
							JDZ = P1;
						}
						break;
					}
					case 1:																			//直行
					{
						Mode = 1;
						if(((P1-JDZ)/64000) >= 9)
						{
							X = P3;
							Key = 2;
						}
						break;
					}
					
					case 2:																		//左转
					{
						Mode = 4;
						Y = P3 - X;
						if((abs(Y)/32000)>=4)
						{
							JDZ = P1;
							Key = 3;
						}

						break;
					}
					
					case 3:																//直行跨沙丘
					{
						Mode = 1;
						if(((P1-JDZ)/64000) >= 4)
						{
							X = P3;
							Key = 4;

						}
						break;
					}
					
					case 4:															//左转
					{
						Mode = 4;
						Y = P3 - X;
						if((abs(Y)/32000)>=4)
						{
							Key = 5;
							JDZ = P1;
						}

						break;
					}
					
					case 5:													//直行
					{
						Mode = 1;
						if(((P1-JDZ)/64000) >= 4)
						{
							X = P3;
							Key = 6;

						}
						break;
					}
					case 6:													//右转
					{
						Mode = 5;
						Y = P3 - X;
						if((abs(Y)/32000)>=8)
						{
							Key = 7;
							JDZ = P1;
						}
						break;
					}
					
						case 7:												//直行
					{
						Mode = 1;
						if(((P1-JDZ)/64000) >= 9)
						{
							X = P3;
							Key = 0;
							make = 1;

						}
						break;
					}
					case 8:														//RST
					{
						Mode = 1;
						if(((P1-JDZ)/64000) >= 2)
						{
							X = P3;
							Key = RST;

						}
						break;
					}
					case 9:														//行至山顶区
					{
						Mode = 1;
						if(((P1-JDZ)/64000) >= 9)
						{
							X = P3;
							Key = 10;

						}
						break;
					}
					case 10:													//右转
					{
						Mode = 5;
						Y = P3 - X;
						if((abs(Y)/32000)>=8)
						{
							GPIO_SetBits(GPIOB,GPIO_Pin_13);
							JDZ = P1;
							delay_ms(1000);
							Key = 0;
						}
						break;
					}
					

						
					
					
					
					
					
					
					
				}
		//Mode 	= 0;

						if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4))			//启动
					{
						delay_ms(1);
						if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4))
						{
								Init_0_Flag = 0;
								make = 0;
								Key = 1;
						}
					}
					else if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5))			//停止
					{
						delay_ms(1);
						if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5))
						{
								Key = 0;
						}
					}
					else if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6))  //复位1
					{
						delay_ms(1);
						if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6))
						{
								
								JDZ = P1;
								RST = 2;
								Key = 8;
								make = 0;
						}
					}
					else if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7))			//复位2
					{
						delay_ms(1);
						if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7))
						{
								JDZ = P1;
								RST = 4;
								Key = 8;
								make = 0;
						}
					}
					if(!GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8))    //行至山顶区
					{
						if(make == 1)
						{
								delay_ms(1);
								if(!GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8))
								{
										JDZ = P1;
										Key = 9;
								}
						}
						
					}
					if(!GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_15) && Init_0_Flag == 1)    //令牌启动
					{
						
								delay_ms(500);
								if(!GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8))
								{
										Init_0_Flag = 0;
										make = 0;
										Key = 1;
								}
						
						
					}

							


			//printf("t\r\n ");
//			printf("%d\r\n",car.Velocity_LF);
//			printf("%d\r\n",car.Velocity_LB);
//			printf("%d\r\n",car.Velocity_RB);
//			printf("%d\r\n\r\n",car.Velocity_RF);
			//delay_ms(100);
				
		}
		
}


























