/*
 * PID.c
 *
 *  Created on: 2023��4��20��
 *      Author: Syhen_XX
 */

#include "zf_common_headfile.h"
#include "PID.h"

PID pitch_gyro;
PID pitch_angle;
PID pitch_velocity;

PID roll_gyro;
PID roll_angle;
PID roll_velocity;

PID yaw_gyro;
PID yaw_angle;
PID yaw_velocity;

// Pitch��ת�ǲ���       ʹ�ô���PID
volatile float pitch_gyro_kp = 5200.0,          pitch_gyro_ki = 0,              pitch_gyro_kd = 10;
volatile float pitch_angle_kp = 0.2,            pitch_angle_ki = 0,             pitch_angle_kd = 1;
volatile float pitch_velocity_kp = 0.14,        pitch_velocity_ki = 0,          pitch_velocity_kd = 0;
volatile float pitch_gyro_revise = 0,           pitch_angle_revise = -1.5,      pitch_velocity_revise = -2;

// Roll�����ǲ���        ʹ�ô���PID
volatile float roll_gyro_kp = 10.0,             roll_gyro_ki = 0,               roll_gyro_kd = 1;
volatile float roll_angle_kp = 160.0,           roll_angle_ki = 0,              roll_angle_kd = 10;
volatile float roll_velocity_kp = 0.007,        roll_velocity_ki = 0,           roll_velocity_kd = 0;
volatile float roll_gyro_revise = 0,            roll_angle_revise = 0.0,        roll_velocity_revise = 0;

// yawƫ���ǲ���         ʹ�ô���PID
volatile float yaw_gyro_kp = 5500.0,            yaw_gyro_ki = 0,                yaw_gyro_kd = 0;
volatile float yaw_angle_kp = 0.3,             yaw_angle_ki = 0,               yaw_angle_kd = 0;
volatile float yaw_velocity_kp = 0.1,             yaw_velocity_ki = 0,            yaw_velocity_kd = 0;
volatile float yaw_gyro_revise = 0,             yaw_angle_revise = 0.0,         yaw_velocity_revise = 0;

/*
 * �޷�����
 * input:����(float)    max:���ֵ(float)     min:��Сֵ(float)
 * ����ֵ:�޷��������
*/
float limit(float input, float max, float min)
{
    return ((input)<(min)?(min):((input)>(max)?(max):(input)));
}

/*
 * PID��ʼ��
 * *pid:PID�ṹ��    maxint:�����޷�    maxout:����޷�
 * ����ֵ:��
 */
void pid_init(PID *pid, float kp, float ki, float kd, float maxint, float maxout)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->MaxIntegral = maxint;
    pid->MaxOutput = maxout;
}

/*
 * PID����
 * *pid:PID�ṹ��    error:���
 * ����ֵ:��
 */
void pid_cala(PID *pid, float error)
{
    pid->Integral += error;
    pid->Pout = pid->Kp * error;
    pid->Iout = pid->Ki * pid->Integral;
    limit(pid->Iout, pid->MaxIntegral, -pid->MaxIntegral);
    pid->Dout = pid->Kd * (error - pid->LastError);
    pid->Output = pid->Pout + pid->Iout + pid->Dout;
    pid->LastError = error;
    limit(pid->Output, pid->MaxOutput, -pid->MaxOutput);
}

/*
 * pitch��ת��PID��ʼ��
 * ����ֵ:��
 */
void pitch_pid_init(void)
{
    // pitch��ת��PID��ʼ��
    pid_init(&pitch_gyro, pitch_gyro_kp, pitch_gyro_ki, pitch_gyro_kd, 0, 9999);
    pid_init(&pitch_angle, pitch_angle_kp, pitch_angle_ki, pitch_angle_kd, 0, 9999);
    pid_init(&pitch_velocity, pitch_velocity_kp, pitch_velocity_ki, pitch_velocity_kd, 0, 9999);
}

/*
 * roll������PID��ʼ��
 * ����ֵ:��
 */
void roll_pid_init(void)
{
    // roll������PID��ʼ��
    pid_init(&roll_gyro, roll_gyro_kp, roll_gyro_ki, roll_gyro_kd, 0, 9999);
    pid_init(&roll_angle, roll_angle_kp, roll_angle_ki, roll_angle_kd, 0, 9999);
    pid_init(&roll_velocity, roll_velocity_kp, roll_velocity_ki, roll_velocity_kd, 0, 9999);
}

/*
 * yawƫ����PID��ʼ��
 * ����ֵ:��
 */
void yaw_pid_init(void)
{
    // yawƫ����PID��ʼ��
    pid_init(&yaw_gyro, yaw_gyro_kp, yaw_gyro_ki, yaw_gyro_kd, 0, 9999);
    pid_init(&yaw_angle, yaw_angle_kp, yaw_angle_ki, yaw_angle_kd, 0, 9999);
    pid_init(&yaw_velocity, yaw_velocity_kp, yaw_velocity_ki, yaw_velocity_kd, 0, 9999);
}
