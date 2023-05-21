/*
 * PID.h
 *
 *  Created on: 2023��4��20��
 *      Author: Syhen_XX
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

#define PID_FLASH_SECTION_INDEX       (0)                                 // �洢PID�����õ�����
#define PID_FLASH_PAGE_INDEX          (0)                                 // �洢PID�����õ�ҳ�� ��һ��ҳ��

// PID�ṹ��
typedef struct
{
    float Kp,Ki,Kd;//����ϵ��
    float Pout,Iout,Dout;
    float Error,LastError;//���ϴ����
    float Integral,MaxIntegral;//���֡������޷�
    float Output,MaxOutput;//���������޷�
}PID;

// pitch��ת��PID
extern PID pitch_gyro;
extern PID pitch_angle;
extern PID pitch_velocity;

// roll������PID
extern PID roll_gyro;
extern PID roll_angle;
extern PID roll_velocity;

extern PID yaw_gyro;
extern PID yaw_angle;
extern PID yaw_velocity;

// pitch��ת��PID����
extern volatile float pitch_gyro_kp, pitch_gyro_ki, pitch_gyro_kd;
extern volatile float pitch_angle_kp, pitch_angle_ki, pitch_angle_kd;
extern volatile float pitch_velocity_kp, pitch_velocity_ki, pitch_velocity_kd;
extern volatile float pitch_gyro_revise, pitch_angle_revise, pitch_velocity_revise;

// roll������PID����
extern volatile float roll_gyro_kp, roll_gyro_ki, roll_gyro_kd;
extern volatile float roll_angle_kp, roll_angle_ki, roll_angle_kd;
extern volatile float roll_velocity_kp, roll_velocity_ki, roll_velocity_kd;
extern volatile float roll_gyro_revise, roll_angle_revise, roll_velocity_revise;

// PID����
extern volatile float yaw_gyro_kp, yaw_gyro_ki, yaw_gyro_kd;
extern volatile float yaw_angle_kp, yaw_angle_ki, yaw_angle_kd;
extern volatile float yaw_velocity_kp, yaw_velocity_ki, yaw_velocity_kd;
extern volatile float yaw_gyro_revise, yaw_angle_revise, yaw_velocity_revise;

float limit(float input, float max, float min);extern
void pid_init(PID *pid, float kp, float ki, float kd, float maxint, float maxout);
void pid_cala(PID *pid, float error);
void pitch_pid_init(void);
void roll_pid_init(void);
void yaw_pid_init(void);

#endif /* CODE_PID_H_ */
