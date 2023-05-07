/*
 * PID.h
 *
 *  Created on: 2023年4月20日
 *      Author: Syhen_XX
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

#define PID_FLASH_SECTION_INDEX       (0)                                 // 存储PID数据用的扇区
#define PID_FLASH_PAGE_INDEX          (0)                                 // 存储PID数据用的页码 第一个页码

// PID结构体
typedef struct
{
    float Kp,Ki,Kd;//三个系数
    float Pout,Iout,Dout;
    float Error,LastError;//误差、上次误差
    float Integral,MaxIntegral;//积分、积分限幅
    float Output,MaxOutput;//输出、输出限幅
}PID;

// pitch滚转角PID
extern PID pitch_gyro;
extern PID pitch_angle;
extern PID pitch_velocity;

// roll俯仰角PID
extern PID roll_gyro;
extern PID roll_angle;
extern PID roll_velocity;

// 平衡PID
extern PID banlance;

// 旋转PID
extern PID rotate;

// pitch滚转角PID参数
extern volatile float pitch_gyro_kp, pitch_gyro_ki, pitch_gyro_kd;
extern volatile float pitch_angle_kp, pitch_angle_ki, pitch_angle_kd;
extern volatile float pitch_velocity_kp, pitch_velocity_ki, pitch_velocity_kd;
extern volatile float pitch_gyro_revise, pitch_angle_revise, pitch_velocity_revise;

// roll俯仰角PID参数
extern volatile float roll_gyro_kp, roll_gyro_ki, roll_gyro_kd;
extern volatile float roll_angle_kp, roll_angle_ki, roll_angle_kd;
extern volatile float roll_velocity_kp, roll_velocity_ki, roll_velocity_kd;
extern volatile float roll_gyro_revise, roll_angle_revise, roll_velocity_revise;

float limit(float input, float max, float min);extern                                   //
void pid_init(PID *pid, float kp, float ki, float kd, float maxint, float maxout);      //
void pid_cala(PID *pid, float error);                                                   //
void pitch_pid_init(void);                                                              //
void roll_pid_init(void);                                                               //
void pid_flash_init(void);                                                              //

#endif /* CODE_PID_H_ */
