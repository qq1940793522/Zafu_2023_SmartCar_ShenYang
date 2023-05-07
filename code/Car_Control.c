/*
 * Car_Control.c
 *
 *  Created on: 2023年5月3日
 *      Author: Syhen_XX
 */

#include "zf_common_headfile.h"
#include "Car_Control.h"
#include "PID.h"
#include "Motor_Control.h"
#include "Mahony_Icm20602.h"

volatile uint8 count = 0;
volatile float pitch_out = 0;
volatile float roll_out = 0;
volatile float yaw_out = 0;

void car_banlance(void)
{
    // 计数器加1
    count++;
    // 角速度环PID计算 2ms
    pid_cala(&pitch_gyro, pitch_angle.Output - icm_now_gy + pitch_gyro_revise);
    pid_cala(&roll_gyro, roll_angle.Output - icm_now_gz + roll_gyro_revise);
    // 角度环PID计算 4ms
    if(count % 2 == 0)
    {
        pid_cala(&pitch_angle, pitch_velocity.Output - pitch + pitch_angle_revise);
        pid_cala(&roll_angle, roll_velocity.Output - roll + roll_angle_revise);
    }
    // 速度环PID计算 10ms count清零
    if(count % 5 == 0)
    {
        banlance_wheel_1_get_encoder();
        banlance_wheel_2_get_encoder();
        moving_wheel_get_encoder();
        pid_cala(&pitch_velocity, 0 - (banlance_wheel_2_now_encoder - banlance_wheel_1_now_encoder)/2 + pitch_velocity_revise);
        pid_cala(&roll_velocity, 0 + moving_wheel_now_encoder + roll_velocity_revise);
        count = 0;
    }
    // 输出传递
    pitch_out = pitch_gyro.Output;
    roll_out = roll_gyro.Output;
    // 限幅
    banlance_wheel_1_pwm = limit(pitch_out, 7999, -7999);
    banlance_wheel_2_pwm = limit(pitch_out, 7999, -7999);
    moving_wheel_pwm = limit(roll_out, 7999, -7999);
    // 方向判定
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
    // 电机控制
    banlance_wheel_1_control(banlance_wheel_1_dir, banlance_wheel_1_pwm);
    banlance_wheel_2_control(banlance_wheel_2_dir, banlance_wheel_2_pwm);
    moving_wheel_control(moving_wheel_dir, moving_wheel_pwm);
}

void car_control(uint8 speed, uint8 rotate)
{

}
