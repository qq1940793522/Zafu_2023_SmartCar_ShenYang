/*
 * Mahony_Icm20602.c
 *
 *  Created on: 2023��4��20��
 *      Author: Syhen_XX
 */

#include "zf_common_headfile.h"
#include "Mahony_Icm20602.h"

//icm20602������ ���ٶ�
volatile float icm_now_ax = 0.0f, icm_now_ay = 0.0f, icm_now_az = 0.0f;
volatile float icm_now_gx = 0.0f, icm_now_gy = 0.0f, icm_now_gz = 0.0f;

//icm20602��������ƫ ���ٶ�
volatile float icm_offset_gx = 0.0f, icm_offset_gy = 0.0f, icm_offset_gz = 0.0f;

//mahony�㷨����
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
volatile float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;

//pitch:��ת��    yaw:ƫ����    roll:������
volatile float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;

/*
 * mahony��ʼ��
 * icm20602��ʼ�� ��ȡ���ٶ���ƫ
 * ����ֵ:��
 */
void mahony_init(void)
{
    //��ʼ��icm20602
    icm20602_init();
    //������ƫ
    for(uint8 i = 0; i < 200 ; i++)
    {
        icm20602_get_gyro();
        icm_offset_gx += icm20602_gyro_transition(icm20602_gyro_x);
        icm_offset_gy += icm20602_gyro_transition(icm20602_gyro_y);
        icm_offset_gz += icm20602_gyro_transition(icm20602_gyro_z);
        system_delay_ms(5);
    }
    icm_offset_gx /= 200;
    icm_offset_gy /= 200;
    icm_offset_gz /= 200;
}
/*
 * icm20602���ݻ�ȡ,���ٶȵ�ͨ�˲�,���ٶ�ȥ��ƫ
 * ����ֵ:��
 */
void icm_value_cala(void)
{
    icm20602_get_acc();
    icm20602_get_gyro();
    icm_now_ax = icm20602_acc_transition(icm20602_acc_x) * alpha + icm20602_acc_transition(icm20602_acc_x) * (1 - alpha);
    icm_now_ay = icm20602_acc_transition(icm20602_acc_y) * alpha + icm20602_acc_transition(icm20602_acc_y) * (1 - alpha);
    icm_now_az = icm20602_acc_transition(icm20602_acc_z) * alpha + icm20602_acc_transition(icm20602_acc_z) * (1 - alpha);
    icm_now_gx = (icm20602_gyro_transition(icm20602_gyro_x) - icm_offset_gx) * PI / 180;
    icm_now_gy = (icm20602_gyro_transition(icm20602_gyro_y) - icm_offset_gy) * PI / 180;
    icm_now_gz = (icm20602_gyro_transition(icm20602_gyro_z) - icm_offset_gz) * PI / 180;
}

/*
 * mahony����
 * gx:Gyro_x    gy:Gyro_y    gz:Gyro_z    ax:Acc_x    ay:Acc_y    az:Acc_z
 * ����ֵ:��
 */
void mahony_cala(float gx, float gy, float gz, float ax, float ay, float az)
{
    float recipNorm;
    float vx, vy, vz;
    float ex, ey, ez;

    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
    {

        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        vx = 2 * (q1 * q3 - q0 * q2);
        vy = 2 * (q0 * q1 + q2 * q3);
        vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

        ex = (ay * vz - az * vy);
        ey = (az * vx - ax * vz);
        ez = (ax * vy - ay * vx);


        integralFBx += mahony_Ki * ex * (0.5f / sampleFreq);
        integralFBy += mahony_Ki * ey * (0.5f / sampleFreq);
        integralFBz += mahony_Ki * ez * (0.5f / sampleFreq);
        gx += integralFBx;
        gy += integralFBy;
        gz += integralFBz;

        gx += mahony_Kp * ex;
        gy += mahony_Kp * ey;
        gz += mahony_Kp * ez;
    }

    gx *= (0.5f * (1.0f / sampleFreq));
    gy *= (0.5f * (1.0f / sampleFreq));
    gz *= (0.5f * (1.0f / sampleFreq));
    q0 += (-q1 * gx - q2 * gy - q3 * gz);
    q1 += (q0 * gx + q2 * gz - q3 * gy);
    q2 += (q0 * gy - q1 * gz + q3 * gx);
    q3 += (q0 * gz + q1 * gy - q2 * gx);

    recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;

    pitch = asin(2 * q0 * q2 - 2 * q1 * q3) * 180 / PI;                                      //��б��
    roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 180 / PI;      //������
}

/*
 * ţ�ٵ��������x^(-1/2)
 * ����:(float)x
 * ����ֵ:(float)y = x^(-1/2)
 */
float invSqrt(float x)
{
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}
