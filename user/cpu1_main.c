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
* 文件名称          cpu1_main
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
#pragma section all "cpu1_dsram"

#include "isr_config.h"
#include "Motor_Control.h"
#include "Mahony_icm20602.h"
#include "PID.h"

// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU1的RAM中


// 工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
// 工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
// 然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
// 一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

// 对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用 enableInterrupts(); 来开启中断嵌套
// 简单点说实际上进入中断后TC系列的硬件自动调用了 disableInterrupts(); 来拒绝响应任何的中断，因此需要我们自己手动调用 enableInterrupts(); 来开启中断的响应。


// **************************** 代码区域 ****************************
void core1_main(void)
{
    disable_Watchdog();                     // 关闭看门狗
    interrupt_global_enable(0);             // 打开全局中断

    // 此处编写用户代码 例如外设初始化代码等

    ips200_init(IPS200_TYPE_PARALLEL8);
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();                 // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码

        ips200_show_string(20, 320-16*12, "Pitch");
        ips200_show_float(10, 320-16*11, pitch, 2, 4);
        ips200_show_string(105, 320-16*12, "Roll");
        ips200_show_float(100, 320-16*11, roll, 2, 4);
        ips200_show_string(185, 320-16*12, "Yaw");
        ips200_show_float(175, 320-16*11, yaw, 2, 4);

        ips200_show_string(15, 320-16*10, "Mov_Wheel:");
        ips200_show_float(130, 320-16*10, roll_gyro.Output, 5, 1);
        if(moving_wheel_now_encoder > 0)
            ips200_show_string(200, 320-16*10, "CW ");
        else
            ips200_show_string(200, 320-16*10, "CCW");

        ips200_show_string(15, 320-16*9, "Banl_Wheel_1:");
        ips200_show_float(130, 320-16*9, pitch_gyro.Output, 5, 1);
        if(banlance_wheel_1_now_encoder > 0)
            ips200_show_string(200, 320-16*9, "CW ");
        else
            ips200_show_string(200, 320-16*9, "CCW");

        ips200_show_string(15, 320-16*8, "Banl_Wheel_2:");
        ips200_show_float(130, 320-16*8, pitch_gyro.Output, 5, 1);
        if(banlance_wheel_2_now_encoder > 0)
            ips200_show_string(200, 320-16*8, "CW ");
        else
            ips200_show_string(200, 320-16*8, "CCW");

        ips200_show_string(15, 320-16*7, "Moving_Wheel_Encoder:");
        ips200_show_int(185, 320-16*7, moving_wheel_now_encoder, 3);
        ips200_show_string(15, 320-16*6, "Banl_Wheel_1_Encoder:");
        ips200_show_int(185, 320-16*6, banlance_wheel_1_now_encoder, 3);
        ips200_show_string(15, 320-16*5, "Banl_Wheel_2_Encoder:");
        ips200_show_int(185, 320-16*5, banlance_wheel_2_now_encoder, 3);

        ips200_show_string(15, 320-16*4, "Acc-X");
        ips200_show_string(100, 320-16*4, "Acc-Y");
        ips200_show_string(180, 320-16*4, "Acc-Z");
        ips200_show_float(10, 320-16*3, icm_now_ax, 1, 4);
        ips200_show_float(90, 320-16*3, icm_now_ay, 1, 4);
        ips200_show_float(170, 320-16*3, icm_now_az, 1, 4);        
        
        ips200_show_string(10, 320-16*2, "Gyro-X");
        ips200_show_string(95, 320-16*2, "Gyro-Y");
        ips200_show_string(175, 320-16*2, "Gyro-Z");
        ips200_show_float(10, 320-16*1, icm_now_gx, 1, 4);
        ips200_show_float(90, 320-16*1, icm_now_gy, 1, 4);
        ips200_show_float(170, 320-16*1, icm_now_gz, 1, 4);

        // 此处编写需要循环执行的代码
    }
}
#pragma section all restore
