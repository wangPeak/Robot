#include "ctrl.h"
#include "delay.h"
#include "printf.h"
extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];  	//存放读取到的ADC值

Ctrl car;             //控制全局变量

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
	short temp = (ADC_To_Velocity(ADC_ConvertedValue[2])-Init_Y);
	ctrl.Velocity_LF = -temp;
	ctrl.Velocity_LB = temp;
	ctrl.Velocity_RF = temp;
	ctrl.Velocity_RB = -temp;
	return ctrl;
}
	Ctrl X_Vector()
{
	Ctrl ctrl;  
	short temp = (-ADC_To_Velocity(ADC_ConvertedValue[1])-Init_X);
	ctrl.Velocity_LB = temp;
	ctrl.Velocity_LF = temp;
	ctrl.Velocity_RB = temp;
	ctrl.Velocity_RF = temp;
	
	return ctrl;
}


Ctrl Vector_S_Y(int S)				 //手动输入Y向量
{
	Ctrl ctrl;

	ctrl.Velocity_LF = -S;
	ctrl.Velocity_LB = S;
	ctrl.Velocity_RF = S;
	ctrl.Velocity_RB = -S;
	return ctrl;
}

	Ctrl Vector_S_X(int S)  //手动输入X向量
{
	Ctrl ctrl;  

	ctrl.Velocity_LB = S;
	ctrl.Velocity_LF = S;
	ctrl.Velocity_RB = S;
	ctrl.Velocity_RF = S;
	
	return ctrl;
}

Ctrl P_Vector()
{
	Ctrl ctrl;  
	short temp = (ADC_To_Velocity(ADC_ConvertedValue[3])-Init_P);
	ctrl.Velocity_LB = temp;
	ctrl.Velocity_LF = -temp;
	ctrl.Velocity_RB = temp;
	ctrl.Velocity_RF = -temp;
	
	return ctrl;
}

Ctrl P_Vector_S(int S )  //手动输入旋转向量
{
	Ctrl ctrl;  
	ctrl.Velocity_LB = S;
	ctrl.Velocity_LF = -S;
	ctrl.Velocity_RB = S;
	ctrl.Velocity_RF = -S;
	
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
	
	
	
	return Car;
		
	
}


Ctrl Vector_S_X_Y(int x,int y)   //手动输入X与Y的向量
{
	Ctrl X = Vector_S_X(x);
	Ctrl Y = Vector_S_Y(y);
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

	return Car;

}

Ctrl Vector_S_X_Y_N(int x,int y)   //手动输入X与Y的向量
{
	Ctrl X = Vector_S_X(x);
	Ctrl Y = Vector_S_Y(y);
	Ctrl Car;
	if((X.Velocity_RF >= 0 && Y.Velocity_RF >= 0) || (X.Velocity_RF < 0 && Y.Velocity_RF < 0))
	{
		Car.Velocity_LF = X.Velocity_LF + Y.Velocity_LF;
		Car.Velocity_RF = X.Velocity_RF + Y.Velocity_RF;
		Car.Velocity_LB = X.Velocity_LB + Y.Velocity_LB;
		Car.Velocity_RB = X.Velocity_RB + Y.Velocity_RB;
	}else
	{
		Car.Velocity_LF = X.Velocity_LF + Y.Velocity_LF;
		Car.Velocity_RF = X.Velocity_RF + Y.Velocity_RF;
		Car.Velocity_LB = X.Velocity_LB + Y.Velocity_LB;
		Car.Velocity_RB = X.Velocity_RB + Y.Velocity_RB;
	}

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
	return temp;
	
	
		
	
}
void Car_Vector_S(int S )
{
	if(abs(car.Velocity_LB)>=MAX_Velocity || abs(car.Velocity_RB)>=MAX_Velocity  || abs(car.Velocity_LF)>=MAX_Velocity ||   abs(car.Velocity_RF)>=MAX_Velocity)
	{
		printf("MAX_Velocity\r\n");
		return ;
	}
	Ctrl P = P_Vector_S(S);
	car.Velocity_LB = car.Velocity_LB+P.Velocity_LB;
	car.Velocity_RB = car.Velocity_RB+P.Velocity_RB;
	car.Velocity_LF = car.Velocity_LF+P.Velocity_LF;
	car.Velocity_RF = car.Velocity_RF+P.Velocity_RF;
	return ;
	
	
		
	
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
