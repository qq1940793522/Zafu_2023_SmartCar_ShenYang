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
* �ļ�����          cpu0_main
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
#pragma section all "cpu0_dsram"

#include "isr_config.h"
#include "Motor_Control.h"
#include "Mahony_Icm20602.h"
#include "PID.h"

uint8 pit_state = 0;
uint8 count_1 = 0;
uint8 count_2 = 0;
uint8 count_3 = 0;
float pitch_out = 0;        //pitch(��ת��)PWM���
float roll_out = 0;         //roll(������)PWM���
float yaw_out = 0;          //yaw(ƫ����)PWM���

// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��

// ���̵��뵽���֮��Ӧ��ѡ�й���Ȼ����refreshˢ��һ��֮���ٱ���
// ����Ĭ������Ϊ�ر��Ż��������Լ��һ�����ѡ��properties->C/C++ Build->Setting
// Ȼ�����Ҳ�Ĵ������ҵ�C/C++ Compiler->Optimization->Optimization level�������Ż��ȼ�
// һ��Ĭ���½����Ĺ��̶���Ĭ�Ͽ�2���Ż�����˴��Ҳ��������Ϊ2���Ż�

// ����TCϵ��Ĭ���ǲ�֧���ж�Ƕ�׵ģ�ϣ��֧���ж�Ƕ����Ҫ���ж���ʹ�� interrupt_global_enable(0); �������ж�Ƕ��
// �򵥵�˵ʵ���Ͻ����жϺ�TCϵ�е�Ӳ���Զ������� interrupt_global_disable(); ���ܾ���Ӧ�κε��жϣ������Ҫ�����Լ��ֶ����� interrupt_global_enable(0); �������жϵ���Ӧ��

// �������ǿ�Դ����ֲ�ÿչ���
// �������ǿ�Դ����ֲ�ÿչ���
// �������ǿ�Դ����ֲ�ÿչ���

// **************************** �������� ****************************
int core0_main(void)
{
    clock_init();                   // ��ȡʱ��Ƶ��<��ر���>
    debug_init();                   // ��ʼ��Ĭ�ϵ��Դ���

    // �˴���д�û����� ���������ʼ�������

    motor_init();
    mahony_init();
    pit_ms_init(CCU60_CH0, 1);
    pitch_pid_init();
    roll_pid_init();

    // �˴���д�û����� ���������ʼ�������
    cpu_wait_event_ready();         // �ȴ����к��ĳ�ʼ�����
    while (TRUE)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���

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

        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}

IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU60_CH0);

    pit_state = 1;

}

#pragma section all restore
// **************************** �������� ****************************
