/*
 * SystemInit.h
 *
 *  Created on: 2023��5��23��
 *      Author: 29079
 */

#ifndef CODE_SYSTEMINIT_H_
#define CODE_SYSTEMINIT_H_



//#ifndef _SystemInit_h
//#define _SystemInit_h

#include "zf_common_headfile.h"

/*******************************************************************************
* Function Name: Struct typedef
* Description  : ������ ����Ϊ �ṹ��� �ײ㶨��
*******************************************************************************/

typedef enum    //��·״̬ ö���嶨��
{
    Outroad =-1,    //�뿪������Ҳ�������Ϊ����̬
    Carport =0,     //����
    Road,           //ֱ��
    Ramp   =2,     //�µ�
    CurveL,         //�����
    CurveR  =4,     //�����
    Cross,         //ʮ��
    Junction=6,     //����
    CircleL,        //Բ����
    CircleR =8,     //Բ����
    BackL,           //����
    Keep=10,         //����״̬
    BackR           //����
} road_state;

typedef struct{ //�ٶ�״̬���� �ṹ�嶨��
    float Max,Min;  //�ٶȷ�Χ
    float Set;    //�ٶȵ�λ
    float element;
    float real[2],goal[2];  //0ȡΪ���� 1ȡΪ����

} Speed_Manage;

typedef struct{ //����״̬ �ṹ�嶨��
    road_state state;   //������������
    road_state Old_state;   //������������
    Speed_Manage V;     //�����ٶȹ���
    float Pitch;        //������
    float Roll;         //�����
    float Yaw;          //ƫ���� ���Կ��� ������ʻ����
} Car_State;

typedef struct{ //����ϵͳPID
    //��� ���ϴ���� ������D����� ������I���
    float Error,Er_former,Er_D,Er_I;
    float P,D,I;    //PID ����
    float Out;      //pid�������
    float goal,real;//����Ŀ�꣬ʵ��״̬
    //һ��� out = P*Error + D*Er_D + I*Er_I
    //һ��� Er_D = Error - Er_former
    //һ��� Error = real - goal
} PID_Base;

typedef struct{ //ͼ����� �ṹ�� ����
    road_state class;   //ͼ���������
    short   mid  [ROW_H];   //����
    short   right[ROW_H];   //�ұ߽� 187����
    short   left [ROW_H];   //��߽� 0����
    float   Mid_Out;    //�����Ȩֵ����
    uint8   width[ROW_H];   //�������
    uint8   no_left [ROW_H];//1��ʾ���� 2��ʾ��������
    uint8   no_right[ROW_H];//1��ʾ�Ҷ��� 2��ʾ��������
    short   begin;  //ÿ֡ͼ�� Ѳ�ߵ���ʼ��
    uint8   lastline; //��֡ �����ߵ����һ��
    uint8   ToBe;

    uint16  Block_White[6]; //��ÿ�������еİ׵������м���
    uint8   Black_End[5];   //�������У�������߽�ֹ��
    short   deltaL,deltaR;  //����
    uint8   MissL_Tag,MissR_Tag;    //���Ҷ��������0��ʾ����û���ߣ�1��ʾ�ײ��ȶ��ߣ�2��ʾ�м��ȶ���
    int Left_Max,Right_Min;

    uint8   Junction_Tag;   //����ѭ����־��ֻ�ھ��������ߺ�������ʱ��ı�״̬��
    uint8   Carport_Enable; //������ʹ��
    uint8   CircleL_Enable; //�󻷼��ʹ��
    uint8   CircleR_Enable; //�һ����ʹ��
    uint8   Circle_In_Enable; //�뻷���ʹ��
    uint8   Circle_Mid_Enable; //�뻷�м���ʹ��
    uint8   Circle_Out_Enable; //�������ʹ��
    uint8   Back_In_Enable; //���������ʹ��
    uint8   Back_Out_Enable; //���������ʹ��
    uint8   Ramp_Out_Enable; //������ʹ��
    uint8   Junction_Enable;        //ʶ��ʹ��
    uint8   Circle_Find_Enable;     //ʶ��ʹ��
    uint8   Back_Find_Enable;       //ʶ��ʹ��
    uint8   Carport_Num;    //����ѭ��������0��ʾ���⡢1��ʾ�ڶ���ѭ����2��ʾ���
    uint8   Carport_Direction;  //���복�ⷽ��
    road_state Mid_class;   //����������
} Image_Info;






/*** �����ⲿ���� ***/
extern Car_State C_Car;

extern PID_Base Steer_Con;
extern PID_Base Left_M_Con;
extern PID_Base Righ_M_Con;

extern Image_Info Image_Shot;
extern uint8 Car_Go_Tag;

/*** �����ⲿ���� ***/
//extern void system_Cpu0_init(void);
//extern void system_Cpu1_init(void);
extern void Image_Info_Init(void);

//������
extern float Wave_list[6];
extern char  Send_Wave_Enable;
extern char  Send_Image_Enable;
//������
extern void Send_Wave(float val0,float val1,float val2,
            float val3,float val4,float val5);// ��vofa��λ�� justfloat,һ�δ�6��
extern void Send_Image(void);
extern void Camera_Graph_Show( uint8 i);

#endif






