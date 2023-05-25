/*
 * SystemInit.h
 *
 *  Created on: 2023年5月23日
 *      Author: 29079
 */

#ifndef CODE_SYSTEMINIT_H_
#define CODE_SYSTEMINIT_H_



//#ifndef _SystemInit_h
//#define _SystemInit_h

#include "zf_common_headfile.h"

/*******************************************************************************
* Function Name: Struct typedef
* Description  : 将对象 抽象为 结构体的 底层定义
*******************************************************************************/

typedef enum    //道路状态 枚举体定义
{
    Outroad =-1,    //离开赛道，也可以理解为过渡态
    Carport =0,     //车库
    Road,           //直道
    Ramp   =2,     //坡道
    CurveL,         //左弯道
    CurveR  =4,     //右弯道
    Cross,         //十字
    Junction=6,     //三叉
    CircleL,        //圆环左
    CircleR =8,     //圆环右
    BackL,           //往返
    Keep=10,         //保持状态
    BackR           //往返
} road_state;

typedef struct{ //速度状态管理 结构体定义
    float Max,Min;  //速度范围
    float Set;    //速度挡位
    float element;
    float real[2],goal[2];  //0取为左轮 1取为右轮

} Speed_Manage;

typedef struct{ //车体状态 结构体定义
    road_state state;   //车辆所处环境
    road_state Old_state;   //车辆所处环境
    Speed_Manage V;     //车辆速度管理
    float Pitch;        //俯仰角
    float Roll;         //横滚角
    float Yaw;          //偏航角 可以看作 车辆行驶方向
} Car_State;

typedef struct{ //控制系统PID
    //误差 和上次误差 和输入D的误差 和输入I误差
    float Error,Er_former,Er_D,Er_I;
    float P,D,I;    //PID 参数
    float Out;      //pid控制输出
    float goal,real;//控制目标，实际状态
    //一般的 out = P*Error + D*Er_D + I*Er_I
    //一般的 Er_D = Error - Er_former
    //一般的 Error = real - goal
} PID_Base;

typedef struct{ //图像参数 结构体 定义
    road_state class;   //图像所属类别
    short   mid  [ROW_H];   //中线
    short   right[ROW_H];   //右边界 187最右
    short   left [ROW_H];   //左边界 0最左
    float   Mid_Out;    //输出的权值中线
    uint8   width[ROW_H];   //赛道宽度
    uint8   no_left [ROW_H];//1表示左丢线 2表示收敛丢线
    uint8   no_right[ROW_H];//1表示右丢线 2表示收敛丢线
    short   begin;  //每帧图像 巡线的起始点
    uint8   lastline; //本帧 有中线的最后一行
    uint8   ToBe;

    uint16  Block_White[6]; //对每块区域中的白点数进行计数
    uint8   Black_End[5];   //划定五列，求其黑线截止行
    short   deltaL,deltaR;  //增量
    uint8   MissL_Tag,MissR_Tag;    //左右丢线情况：0表示大体没丢线，1表示底层先丢线，2表示中间先丢线
    int Left_Max,Right_Min;

    uint8   Junction_Tag;   //三叉循环标志，只在经过斑马线和往返的时候改变状态。
    uint8   Carport_Enable; //车库检测使能
    uint8   CircleL_Enable; //左环检测使能
    uint8   CircleR_Enable; //右环检测使能
    uint8   Circle_In_Enable; //入环检测使能
    uint8   Circle_Mid_Enable; //入环中间检测使能
    uint8   Circle_Out_Enable; //出环检测使能
    uint8   Back_In_Enable; //出往返检测使能
    uint8   Back_Out_Enable; //出往返检测使能
    uint8   Ramp_Out_Enable; //车库检测使能
    uint8   Junction_Enable;        //识别使能
    uint8   Circle_Find_Enable;     //识别使能
    uint8   Back_Find_Enable;       //识别使能
    uint8   Carport_Num;    //车库循环次数，0表示出库、1表示第二次循环、2表示入库
    uint8   Carport_Direction;  //出入车库方向
    road_state Mid_class;   //中线输出类别
} Image_Info;






/*** 对象外部声明 ***/
extern Car_State C_Car;

extern PID_Base Steer_Con;
extern PID_Base Left_M_Con;
extern PID_Base Righ_M_Con;

extern Image_Info Image_Shot;
extern uint8 Car_Go_Tag;

/*** 函数外部声明 ***/
//extern void system_Cpu0_init(void);
//extern void system_Cpu1_init(void);
extern void Image_Info_Init(void);

//调试用
extern float Wave_list[6];
extern char  Send_Wave_Enable;
extern char  Send_Image_Enable;
//调试用
extern void Send_Wave(float val0,float val1,float val2,
            float val3,float val4,float val5);// 用vofa上位机 justfloat,一次传6个
extern void Send_Image(void);
extern void Camera_Graph_Show( uint8 i);

#endif






