#ifndef __CTRL_H
#define	__CTRL_H


#include "adc.h"

typedef struct {
	short Velocity_RF;//右前
	short Velocity_LF;//左前
	short Velocity_RB;//右后
	short Velocity_LB;//左后
	short X;
	short Y;
	short P;
}Ctrl;


void Ctrl_Init(void);  //初始化控制  
Ctrl Y_Vector(void);
Ctrl X_Vector(void);
Ctrl Car_Vector(void);
Ctrl Car_Vector_P(void);
//Ctrl CTRL_To_Velocity();  //用于得到控制电机的速度
//struct Vector
//{
//	short int LB;//左前
//	short int Rb;//右前
//	
//}



#endif 

