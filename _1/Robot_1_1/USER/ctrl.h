#ifndef __CTRL_H
#define	__CTRL_H


#include "adc.h"

typedef struct {
	short Velocity_RF;//��ǰ
	short Velocity_LF;//��ǰ
	short Velocity_RB;//�Һ�
	short Velocity_LB;//���
	short X;
	short Y;
	short P;
}Ctrl;


void Ctrl_Init(void);  //��ʼ������  
Ctrl Y_Vector(void);
Ctrl X_Vector(void);
Ctrl Car_Vector(void);
Ctrl Car_Vector_P(void);
//Ctrl CTRL_To_Velocity();  //���ڵõ����Ƶ�����ٶ�
//struct Vector
//{
//	short int LB;//��ǰ
//	short int Rb;//��ǰ
//	
//}



#endif 

