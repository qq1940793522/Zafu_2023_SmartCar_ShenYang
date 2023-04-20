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

//Pitch����     ʹ�ò���PID    ֻ��KP     5300    0.2     0.14
volatile float pitch_gyro_kp = 5300, pitch_gyro_ki = 0, pitch_gyro_kd = 0;
volatile float pitch_angle_kp = 0.2, pitch_angle_ki = 0, pitch_angle_kd = 0;
volatile float pitch_velocity_kp = 0.13, pitch_velocity_ki = 0, pitch_velocity_kd = 0;
volatile float pitch_gyro_revise = 0, pitch_angle_revise = -2.0, pitch_velocity_revise = 0;

//Roll����      ʹ�ò���PID    ֻ��KP     1       2500    0.0025
volatile float roll_gyro_kp = 1, roll_gyro_ki = 0, roll_gyro_kd = 0;
volatile float roll_angle_kp = 2500, roll_angle_ki = 0, roll_angle_kd = 0;
volatile float roll_velocity_kp = 0.0025, roll_velocity_ki = 0, roll_velocity_kd = 0;
volatile float roll_gyro_revise = 0, roll_angle_revise = 0.2, roll_velocity_revise = 0;

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
    pid_init(&roll_gyro, roll_gyro_kp, roll_gyro_ki, roll_gyro_kd, 0, 9999);
    pid_init(&roll_angle, roll_angle_kp, roll_angle_ki, roll_angle_kd, 0, 9999);
    pid_init(&roll_velocity, roll_velocity_kp, roll_velocity_ki, roll_velocity_kd, 0, 9999);
}
