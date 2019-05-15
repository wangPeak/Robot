#include "ctrl.h"
#include "delay.h"
#include "printf.h"
extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];  	//存放读取到的ADC值
short Init_X = 0;
short Init_Y = 0;
short Init_P = 0;
void Ctrl_Init(void)
{
	ADCx_Init();
	short init_X[3]={0,0,0};
	short init_Y[3]={0,0,0};
	short init_P[3]={0,0,0};
	delay_ms(50);
	for(int i = 0;i<3;i++)
	{
		init_X[i] = ADC_To_Velocity(ADC_ConvertedValue[1]);
		init_Y[i] = ADC_To_Velocity(ADC_ConvertedValue[2]);
		init_P[i] = ADC_To_Velocity(ADC_ConvertedValue[3]);
		delay_ms(50);
	}
	Init_X = -((init_X[0]+init_X[1]+init_X[2])/3);
	Init_Y = (init_Y[0]+init_Y[1]+init_Y[2])/3;
	Init_P = (init_P[0]+init_P[1]+init_P[2])/3;

	if(Init_X > 100 || Init_Y > 100)
	{
			printf("Error !!! Init_X == %d ,Init_Y == %d \r\n",Init_X,Init_Y);
	}
			printf("Init_X == %d ,Init_Y == %d \r\n",Init_X,Init_Y);
}

Ctrl Y_Vector()
{
	Ctrl ctrl;
	short Y = (ADC_To_Velocity(ADC_ConvertedValue[2])-Init_Y);
	ctrl.Velocity_LF = -Y;
	ctrl.Velocity_LB = Y;
	ctrl.Velocity_RF = Y;
	ctrl.Velocity_RB = -Y;
	ctrl.Y = Y;
	return ctrl;
}
	Ctrl X_Vector()
{
	Ctrl ctrl;  
	short X = (-ADC_To_Velocity(ADC_ConvertedValue[1])-Init_X);
	ctrl.Velocity_LB = X;
	ctrl.Velocity_LF = X;
	ctrl.Velocity_RB = X;
	ctrl.Velocity_RF = X;
	ctrl.X = X;
	
	return ctrl;
}
Ctrl P_Vector()
{
	Ctrl ctrl;  
	short P = (ADC_To_Velocity(ADC_ConvertedValue[3])-Init_P);
	ctrl.Velocity_LB = -P;
	ctrl.Velocity_LF = P;
	ctrl.Velocity_RB = -P;
	ctrl.Velocity_RF = P;
	ctrl.P = P;
	return ctrl;
}

Ctrl Car_Vector(void)
{
	Ctrl X = X_Vector();
	Ctrl Y = Y_Vector();
	Ctrl Car;
	if((X.Velocity_RF >= 0 && Y.Velocity_RF >= 0) || (X.Velocity_RF < 0 && Y.Velocity_RF < 0))
	{
		Car.Velocity_LF = X.Velocity_LF+Y.Velocity_LF;
		Car.Velocity_RF = abs(X.Velocity_RF) > abs(Y.Velocity_RF) ? X.Velocity_RF : Y.Velocity_RF;
		Car.Velocity_LB = abs(X.Velocity_LB) > abs(Y.Velocity_LB) ? X.Velocity_LB : Y.Velocity_LB;
		Car.Velocity_RB = X.Velocity_RB+Y.Velocity_RB;
	}else
	{
		Car.Velocity_LF = abs(X.Velocity_LF) > abs(Y.Velocity_LF) ? X.Velocity_LF : Y.Velocity_LF;
		Car.Velocity_RF = X.Velocity_RF+Y.Velocity_RF;
		Car.Velocity_LB = X.Velocity_LB+Y.Velocity_LB;
		Car.Velocity_RB = abs(X.Velocity_RB) > abs(Y.Velocity_RB) ? X.Velocity_RB : Y.Velocity_RB;
	}
	
	Car.X = X.X;
	Car.Y = Y.Y;
	
	return Car;
		
	
}
Ctrl Car_Vector_P(void)
{
	Ctrl P = P_Vector();
	Ctrl Car = Car_Vector();
	Ctrl temp;
	temp.Velocity_LB = Car.Velocity_LB+P.Velocity_LB;
	temp.Velocity_RB = Car.Velocity_RB+P.Velocity_RB;
	temp.Velocity_LF = Car.Velocity_LF+P.Velocity_LF;
	temp.Velocity_RF = Car.Velocity_RF+P.Velocity_RF;
	temp.X = Car.X;
	temp.Y = Car.Y;
	temp.P = P.P;
	return temp;
	
	
		
	
}
//Ctrl Car_Rotation(void)
//{
//	
//}

//Ctrl CTRL_To_Velocity()  //用于得到控制电机的速度
//{
//		short _X = 0;
//		short _Y = 0;
//	
//		_X = ADC_To_Velocity(ADC_ConvertedValue[1]);
//		_Y = ADC_To_Velocity(ADC_ConvertedValue[2]);
//}
