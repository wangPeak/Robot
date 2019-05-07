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
}Ctrl;


void Ctrl_Init(void);  //初始化控制  
Ctrl Y_Vector(void);
Ctrl X_Vector(void);
Ctrl Car_Vector(void);
Ctrl Car_Vector_P(void);
Ctrl P_Vector_S(int S);       //手动得到旋转向量大小为   S
Ctrl Vector_S_X(int S);				//手动输入X向量
Ctrl Vector_S_Y(int S);				//手动输入Y向量
void Car_Vector_S(int S);			//在原有的向量上加入旋转向量 S

Ctrl Vector_S_X_Y(int x,int y);   //手动输入X与Y并得到向量的叠加
Ctrl Vector_S_X_Y_N(int x,int y);   //手动输入X与Y的向量
//Ctrl CTRL_To_Velocity();  //用于得到控制电机的速度
//struct Vector
//{
//	short int LB;//左前
//	short int Rb;//右前
//	
//}


#endif 

