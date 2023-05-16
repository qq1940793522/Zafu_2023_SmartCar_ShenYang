/*
 * Motor_Control.c
 *
 *  Created on: 2023��4��20��
 *      Author: Syhen_XX
 */

#include "zf_common_headfile.h"
#include "Motor_Control.h"

volatile int16 moving_wheel_now_encoder = 0;
volatile int16 banlance_wheel_1_now_encoder = 0;
volatile int16 banlance_wheel_2_now_encoder = 0;
volatile uint16 moving_wheel_pwm = 0;
volatile uint16 banlance_wheel_1_pwm = 0;
volatile uint16 banlance_wheel_2_pwm = 0;
volatile uint8 moving_wheel_dir = 0;
volatile uint8 banlance_wheel_1_dir = 0;
volatile uint8 banlance_wheel_2_dir = 0;

/*
 * �н��ֵ��DIR��PWM�ͱ�������ʼ��
 * ����ֵ:��
 */
void motor_init(void)
{
    //�н��ֵ����ʼ��
    gpio_init(MOVING_WHEEL_INH_1_PIN, GPO, 0, GPO_PUSH_PULL);
    gpio_init(MOVING_WHEEL_INH_2_PIN, GPO, 0, GPO_PUSH_PULL);
    pwm_init(MOVING_WHEEL_PWM_1_PIN, MOVING_WHEEL_PWM_FREQ, 0);
    pwm_init(MOVING_WHEEL_PWM_2_PIN, MOVING_WHEEL_PWM_FREQ, 0);
    encoder_quad_init(MOVING_WHEEL_ENCODER_TIM, MOVING_WHEEL_ENCODER_A_PIN, MOVING_WHEEL_ENCODER_B_PIN);
    //����1��ʼ��
    gpio_init(BANLANCE_WHEEL_1_DIR_PIN, GPO, 0, GPO_PUSH_PULL);
    pwm_init(BANLANCE_WHEEL_1_PWM_PIN, BANLANCE_WHEEL_PWM_FREQ, 10000);
    encoder_quad_init(BANLANCE_WHEEL_1_ENCODER_TIM, BANLANCE_WHEEL_1_ENCODER_A_PIN, BANLANCE_WHEEL_1_ENCODER_B_PIN);
    //����2��ʼ��
    gpio_init(BANLANCE_WHEEL_2_DIR_PIN, GPO, 0, GPO_PUSH_PULL);
    pwm_init(BANLANCE_WHEEL_2_PWM_PIN, BANLANCE_WHEEL_PWM_FREQ, 10000);
    encoder_quad_init(BANLANCE_WHEEL_2_ENCODER_TIM, BANLANCE_WHEEL_2_ENCODER_A_PIN, BANLANCE_WHEEL_2_ENCODER_B_PIN);
}

/*
 * �н��ֵ�����ƺ���
 * dir:����(CW/CCW)       pwm:PWM�ź�(0~9999)
 * ����ֵ: ��
 */
void moving_wheel_control(uint8 dir, uint16 pwm)
{
    if(dir)
    {
        gpio_set_level(MOVING_WHEEL_INH_2_PIN, 1);
        pwm_set_duty(MOVING_WHEEL_PWM_1_PIN, 0);
        gpio_set_level(MOVING_WHEEL_INH_1_PIN, 1);
        pwm_set_duty(MOVING_WHEEL_PWM_2_PIN, pwm);
    }
    else if(!dir)
    {
        gpio_set_level(MOVING_WHEEL_INH_1_PIN, 1);
        pwm_set_duty(MOVING_WHEEL_PWM_2_PIN, 0);
        gpio_set_level(MOVING_WHEEL_INH_2_PIN, 1);
        pwm_set_duty(MOVING_WHEEL_PWM_1_PIN, pwm);
    }

}

/*
 * ������1������ƺ���
 * dir:����(CW/CCW)       pwm:PWM�ź�(0~9999)
 * ����ֵ: ��
 */
void banlance_wheel_1_control(uint8 dir, uint16 pwm)
{
    gpio_set_level(BANLANCE_WHEEL_1_DIR_PIN, dir);
    pwm_set_duty(BANLANCE_WHEEL_1_PWM_PIN, 10000 - pwm);
}

/*
 * ������2������ƺ���
 * dir:����(CW/CCW)       pwm:PWM�ź�(0~9999)
 * ����ֵ: ��
 */
void banlance_wheel_2_control(uint8 dir, uint16 pwm)
{
    gpio_set_level(BANLANCE_WHEEL_2_DIR_PIN, dir);
    pwm_set_duty(BANLANCE_WHEEL_2_PWM_PIN, 10000 - pwm);
}

/*
 * �н��ֵ����������ȡ����
 * ����ֵ: ��
 */
void moving_wheel_get_encoder(void)
{
    moving_wheel_now_encoder = encoder_get_count(MOVING_WHEEL_ENCODER_TIM);
    encoder_clear_count(MOVING_WHEEL_ENCODER_TIM);
}

/*
 * ������1�����������ȡ����
 * ����ֵ: ��
 */
void banlance_wheel_1_get_encoder(void)
{
    banlance_wheel_1_now_encoder = encoder_get_count(BANLANCE_WHEEL_1_ENCODER_TIM);
    encoder_clear_count(BANLANCE_WHEEL_1_ENCODER_TIM);
}

/*
 * ������2�����������ȡ����
 * ����ֵ: ��
 */
void banlance_wheel_2_get_encoder(void)
{
    banlance_wheel_2_now_encoder = encoder_get_count(BANLANCE_WHEEL_2_ENCODER_TIM);
    encoder_clear_count(BANLANCE_WHEEL_2_ENCODER_TIM);
}

void motor_stop(void)
{
    pwm_set_duty(MOVING_WHEEL_PWM_1_PIN, 0);
    pwm_set_duty(MOVING_WHEEL_PWM_2_PIN, 0);
    pwm_set_duty(BANLANCE_WHEEL_1_PWM_PIN, 10000);
    pwm_set_duty(BANLANCE_WHEEL_2_PWM_PIN, 10000);
}
