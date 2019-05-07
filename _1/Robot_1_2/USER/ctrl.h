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
}Ctrl;


void Ctrl_Init(void);  //��ʼ������  
Ctrl Y_Vector(void);
Ctrl X_Vector(void);
Ctrl Car_Vector(void);
Ctrl Car_Vector_P(void);
Ctrl P_Vector_S(int S);       //�ֶ��õ���ת������СΪ   S
Ctrl Vector_S_X(int S);				//�ֶ�����X����
Ctrl Vector_S_Y(int S);				//�ֶ�����Y����
void Car_Vector_S(int S);			//��ԭ�е������ϼ�����ת���� S

Ctrl Vector_S_X_Y(int x,int y);   //�ֶ�����X��Y���õ������ĵ���
Ctrl Vector_S_X_Y_N(int x,int y);   //�ֶ�����X��Y������
//Ctrl CTRL_To_Velocity();  //���ڵõ����Ƶ�����ٶ�
//struct Vector
//{
//	short int LB;//��ǰ
//	short int Rb;//��ǰ
//	
//}


#endif 

