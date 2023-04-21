/*********************************************************************************************************************
* TC264 Opensourec Library ����TC264 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC264 ��Դ���һ����
*
* TC264 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          cpu1_main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.8.0
* ����ƽ̨          TC264D
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#pragma section all "cpu1_dsram"

#include "isr_config.h"
#include "Motor_Control.h"
#include "Mahony_icm20602.h"
#include "PID.h"

// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU1��RAM��


// ���̵��뵽���֮��Ӧ��ѡ�й���Ȼ����refreshˢ��һ��֮���ٱ���
// ����Ĭ������Ϊ�ر��Ż��������Լ��һ�����ѡ��properties->C/C++ Build->Setting
// Ȼ�����Ҳ�Ĵ������ҵ�C/C++ Compiler->Optimization->Optimization level�������Ż��ȼ�
// һ��Ĭ���½����Ĺ��̶���Ĭ�Ͽ�2���Ż�����˴��Ҳ��������Ϊ2���Ż�

// ����TCϵ��Ĭ���ǲ�֧���ж�Ƕ�׵ģ�ϣ��֧���ж�Ƕ����Ҫ���ж���ʹ�� enableInterrupts(); �������ж�Ƕ��
// �򵥵�˵ʵ���Ͻ����жϺ�TCϵ�е�Ӳ���Զ������� disableInterrupts(); ���ܾ���Ӧ�κε��жϣ������Ҫ�����Լ��ֶ����� enableInterrupts(); �������жϵ���Ӧ��


// **************************** �������� ****************************test
void core1_main(void)
{
    disable_Watchdog();                     // �رտ��Ź�
    interrupt_global_enable(0);             // ��ȫ���ж�

    // �˴���д�û����� ���������ʼ�������

    ips200_init(IPS200_TYPE_PARALLEL8);
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);

    // �˴���д�û����� ���������ʼ�������
    cpu_wait_event_ready();                 // �ȴ����к��ĳ�ʼ�����
    while (TRUE)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���

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

        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}
#pragma section all restore
