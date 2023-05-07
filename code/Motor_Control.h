/*
 * Motor_Control.h
 *
 *  Created on: 2023Äê4ÔÂ20ÈÕ
 *      Author: Syhen_XX
 */

#ifndef CODE_MOTOR_CONTROL_H_
#define CODE_MOTOR_CONTROL_H_

#define     CW                                  (1)
#define     CCW                                 (0)


#define     MOVING_WHEEL_PWM_FREQ               (17000)
#define     BANLANCE_WHEEL_PWM_FREQ             (20000)


#define     MOVING_WHEEL_INH_1_PIN              (P21_2)
#define     MOVING_WHEEL_INH_2_PIN              (P21_4)
#define     MOVING_WHEEL_PWM_1_PIN              (ATOM0_CH1_P21_3)
#define     MOVING_WHEEL_PWM_2_PIN              (ATOM0_CH3_P21_5)
#define     MOVING_WHEEL_ENCODER_TIM            (TIM4_ENCODER)
#define     MOVING_WHEEL_ENCODER_A_PIN          (TIM4_ENCODER_CH1_P02_8)
#define     MOVING_WHEEL_ENCODER_B_PIN          (TIM4_ENCODER_CH2_P00_9)


#define     BANLANCE_WHEEL_1_DIR_PIN            (P02_4)
#define     BANLANCE_WHEEL_1_PWM_PIN            (ATOM1_CH5_P02_5)
#define     BANLANCE_WHEEL_1_ENCODER_TIM        (TIM2_ENCODER)
#define     BANLANCE_WHEEL_1_ENCODER_A_PIN      (TIM2_ENCODER_CH1_P33_7)
#define     BANLANCE_WHEEL_1_ENCODER_B_PIN      (TIM2_ENCODER_CH2_P33_6)

#define     BANLANCE_WHEEL_2_DIR_PIN            (P02_6)
#define     BANLANCE_WHEEL_2_PWM_PIN            (ATOM1_CH7_P02_7)
#define     BANLANCE_WHEEL_2_ENCODER_TIM        (TIM5_ENCODER)
#define     BANLANCE_WHEEL_2_ENCODER_A_PIN      (TIM5_ENCODER_CH1_P10_3)
#define     BANLANCE_WHEEL_2_ENCODER_B_PIN      (TIM5_ENCODER_CH2_P10_1)

extern volatile int16 moving_wheel_now_encoder;
extern volatile int16 banlance_wheel_1_now_encoder;
extern volatile int16 banlance_wheel_2_now_encoder;
extern volatile uint16 moving_wheel_pwm;
extern volatile uint16 banlance_wheel_1_pwm;
extern volatile uint16 banlance_wheel_2_pwm;
extern volatile uint8 moving_wheel_dir;
extern volatile uint8 banlance_wheel_1_dir;
extern volatile uint8 banlance_wheel_2_dir;

void motor_init(void);
void moving_wheel_control(uint8 dir, uint16 pwm);
void banlance_wheel_1_control(uint8 dir, uint16 pwm);
void banlance_wheel_2_control(uint8 dir, uint16 pwm);
void moving_wheel_get_encoder(void);
void banlance_wheel_1_get_encoder(void);
void banlance_wheel_2_get_encoder(void);

#endif /* CODE_MOTOR_CONTROL_H_ */
