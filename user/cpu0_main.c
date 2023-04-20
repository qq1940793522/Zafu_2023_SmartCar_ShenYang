/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          cpu0_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"

#include "isr_config.h"
#include "Motor_Control.h"
#include "Mahony_Icm20602.h"
#include "PID.h"

uint8 pit_state = 0;
uint8 count_1 = 0;
uint8 count_2 = 0;
uint8 count_3 = 0;
float pitch_out = 0;        //pitch(滚转角)PWM输出
float roll_out = 0;         //roll(俯仰角)PWM输出
float yaw_out = 0;          //yaw(偏航角)PWM输出

// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

// 工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
// 工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
// 然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
// 一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

// 对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用 interrupt_global_enable(0); 来开启中断嵌套
// 简单点说实际上进入中断后TC系列的硬件自动调用了 interrupt_global_disable(); 来拒绝响应任何的中断，因此需要我们自己手动调用 interrupt_global_enable(0); 来开启中断的响应。

// 本例程是开源库移植用空工程
// 本例程是开源库移植用空工程
// 本例程是开源库移植用空工程

// **************************** 代码区域 ****************************
int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口

    // 此处编写用户代码 例如外设初始化代码等

    motor_init();
    mahony_init();
    pit_ms_init(CCU60_CH0, 1);
    pitch_pid_init();
    roll_pid_init();

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码

        if(pit_state)
        {
            icm_value_cala();
            mahony_cala(icm_now_gz, icm_now_gy, -icm_now_gx, icm_now_az, icm_now_ay, -icm_now_ax);
            pit_state = 0;
            count_1++;
            count_2++;
            count_3++;
            if(count_1 % 2 == 0)
            {
                count_1 = 0;
                pid_cala(&pitch_gyro, pitch_angle.Output - icm_now_gy + pitch_gyro_revise);
                pid_cala(&roll_gyro, roll_angle.Output - icm_now_gz + roll_gyro_revise);
                //printf("%f,%f,%f\n",icm_now_gx, icm_now_gy, icm_now_gz);
                //printf("%f,%f,%f,%f\n",pitch_gyro.Pout, pitch_gyro.Dout, pitch_gyro.Output, pitch_out);
                //printf("%f,%f,%f,%f\n",roll_gyro.Pout, roll_gyro.Dout, roll_gyro.Output, roll_out);
            }
            if(count_2 % 5 == 0)
            {
                count_2 = 0;
                pid_cala(&pitch_angle, pitch_velocity.Output - pitch + pitch_angle_revise);
                pid_cala(&roll_angle, roll_velocity.Output - roll + roll_angle_revise);
                //printf("%f,%f,%f\n",pitch, roll, yaw);
                //printf("%f,%f,%f,%f\n", pitch_angle.Pout, pitch_angle.Dout, pitch_angle.Output, pitch_out);
                //printf("%f,%f,%f,%f\n", roll_angle.Pout, roll_angle.Dout, roll_angle.Output, roll_out);
            }
            if(count_3 % 10 == 0)
            {
                count_3 = 0;
                banlance_wheel_1_get_encoder();
                banlance_wheel_2_get_encoder();
                moving_wheel_get_encoder();
                pid_cala(&pitch_velocity, 0 - (banlance_wheel_2_now_encoder - banlance_wheel_1_now_encoder)/2 + pitch_velocity_revise);
                pid_cala(&roll_velocity, 0 + moving_wheel_now_encoder + roll_velocity_revise);
                //printf("%f,%f,%f,%f\n", pitch_velocity.Output, pitch_angle.Output, pitch_gyro.Output, pitch_out);
                //printf("%d,%d,%d\n",banlance_wheel_1_now_encoder, banlance_wheel_2_now_encoder, moving_wheel_now_encoder);
                //printf("%f,%f,%f,%f\n", pitch_velocity.Pout, pitch_velocity.Dout, pitch_velocity.Output, pitch_out);
                //printf("%f,%f,%f,%f\n", roll_velocity.Pout, roll_velocity.Dout, roll_velocity.Output, roll_out);
            }

            pitch_out = pitch_gyro.Output;
            roll_out = roll_gyro.Output;
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
            banlance_wheel_1_pwm = limit(pitch_out, 7999, -7999);
            banlance_wheel_2_pwm = limit(pitch_out, 7999, -7999);
            moving_wheel_pwm = limit(roll_out, 7999, -7999);
            banlance_wheel_1_control(banlance_wheel_1_dir, banlance_wheel_1_pwm);
            banlance_wheel_2_control(banlance_wheel_2_dir, banlance_wheel_2_pwm);
            moving_wheel_control(moving_wheel_dir, moving_wheel_pwm);
        }

        // 此处编写需要循环执行的代码
    }
}

IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH0);

    pit_state = 1;

}

#pragma section all restore
// **************************** 代码区域 ****************************
