/*
 * Car_Control.c
 *
 *  Created on: 2023��5��3��
 *      Author: Syhen_XX
 */

#include "math.h"
#include "zf_common_headfile.h"
#include "Car_Control.h"
#include "PID.h"
#include "Motor_Control.h"
#include "Mahony_Icm20602.h"

volatile uint8 count = 0;
volatile float pitch_out = 0;
volatile float roll_out = 0;
volatile float yaw_out = 0;
volatile float car_speed = 0;
volatile float car_rotate = 0;

uint8 car_check(void)
{
    if( fabsf(icm_now_ay) > 0.2 || fabsf(icm_now_az) > 0.2 )
        return 0;
    else
        return 1;
}

void car_banlance(void)
{
    // ��������1
    count++;
    // ���ٶȻ�PID���� 2ms
    pid_cala(&pitch_gyro, pitch_angle.Output - icm_now_gy + pitch_gyro_revise);
    pid_cala(&roll_gyro, roll_angle.Output - icm_now_gz + roll_gyro_revise);
    pid_cala(&yaw_gyro, car_rotate - icm_now_gx + yaw_gyro_revise);
    // �ǶȻ�PID���� 4ms
    if(count % 2 == 0)
    {
        pid_cala(&pitch_angle, pitch_velocity.Output - pitch + pitch_angle_revise);
        pid_cala(&roll_angle, roll_velocity.Output - roll + roll_angle_revise);

    }
    // �ٶȻ�PID���� 10ms count����
    if(count % 5 == 0)
    {
        banlance_wheel_1_get_encoder();
        banlance_wheel_2_get_encoder();
        moving_wheel_get_encoder();
        pid_cala(&pitch_velocity, 0 + (banlance_wheel_1_now_encoder - banlance_wheel_2_now_encoder)/2 + pitch_velocity_revise);
        pid_cala(&roll_velocity, car_speed + moving_wheel_now_encoder + roll_velocity_revise);
        count = 0;
    }
    // �������
    pitch_out = pitch_gyro.Output;
    roll_out = roll_gyro.Output;
    yaw_out = yaw_gyro.Output;
    // �޷�
    banlance_wheel_1_pwm = limit(pitch_out - yaw_out, 7999, -7999);
    banlance_wheel_2_pwm = limit(pitch_out + yaw_out, 7999, -7999);
    moving_wheel_pwm = limit(roll_out, 7999, -7999);
    // �����ж�
    if(pitch_out >= 0)
    {
        banlance_wheel_1_dir = CCW;
        banlance_wheel_2_dir = CW;
    }
    else if(pitch_out < 0)
    {
        banlance_wheel_1_dir = CW;
        banlance_wheel_2_dir = CCW;
    }
    if(roll_out >= 0)
        moving_wheel_dir = CW;
    else if(roll_out < 0)
        moving_wheel_dir = CCW;
    // �������
    banlance_wheel_1_control(banlance_wheel_1_dir, banlance_wheel_1_pwm);
    banlance_wheel_2_control(banlance_wheel_2_dir, banlance_wheel_2_pwm);
    moving_wheel_control(moving_wheel_dir, moving_wheel_pwm);
}

void car_control(int16 speed, float rotate)
{
    // �ٶȿ���     �Ƽ��ٶ�speed=250
    car_speed = speed;
    // ��ת����
    car_rotate = rotate;
}
