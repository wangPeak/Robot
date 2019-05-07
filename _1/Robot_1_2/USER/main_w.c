#include "main.h"
int main2()
{
	
		short Velocity = 0;
	while(1)
	{
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    CAN1_Configuration();                               //CAN1初始化
    
    delay_ms(500);                                      //刚开始要有足够的延时，确保驱动器已经初始化完成 
    CAN_RoboModule_DRV_Reset(0,0);                      //对0组所有驱动器进行复位 
    
    delay_ms(500);                                      //发送复位指令后的延时必须要有，等待驱动器再次初始化完成
    
    CAN_RoboModule_DRV_Config(0,1,100,0);               //1号驱动器配置为100ms传回一次数据
    delay_us(200);                                      //此处延时为了不让传回数据时候4个不一起传
    CAN_RoboModule_DRV_Config(0,2,100,0);               //2号驱动器配置为100ms传回一次数据
    delay_us(200);                                      //此处延时为了不让传回数据时候4个不一起传
    CAN_RoboModule_DRV_Config(0,3,100,0);               //3号驱动器配置为100ms传回一次数据
    delay_us(200);                                      //此处延时为了不让传回数据时候4个不一起传
    CAN_RoboModule_DRV_Config(0,4,100,0);               //3号驱动器配置为100ms传回一次数据

    
    CAN_RoboModule_DRV_Mode_Choice(0,0,Velocity_Mode);  //0组的所有驱动器 都进入速度模式
    delay_ms(500);                                      //发送模式选择指令后，要等待驱动器进入模式就绪。所以延时也不可以去掉。
    

    while(1) //测试加速
    {
        delay_ms(100);
        //CAN_RoboModule_DRV_Velocity_Mode(0,1,MAX_PWM,Velocity);

        Velocity += 200;
        if(Velocity > 3000)
        {
            break;
        }
    }
    
    while(1) //测试减速
    {
        delay_ms(100);
        
        //CAN_RoboModule_DRV_Velocity_Mode(0,1,MAX_PWM,Velocity);
        Velocity -= 200;
        if(Velocity < -3000)
        {
            break;
        }
    }
//		CAN_RoboModule_DRV_Velocity_Mode(0,1,MAX_PWM,-2000);
//		delay_ms(100);
//    CAN_RoboModule_DRV_Velocity_Mode(0,1,MAX_PWM,-1000);
//    delay_ms(100);
//		CAN_RoboModule_DRV_Velocity_Mode(0,1,MAX_PWM,-500);
//		delay_ms(100);
//		CAN_RoboModule_DRV_Velocity_Mode(0,1,MAX_PWM,0);
    while(1);
	}
}
























