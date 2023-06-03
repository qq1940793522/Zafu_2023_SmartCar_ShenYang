/*
 * Menu.h
 *
 *  Created on: 2023��5��5��
 *      Author: CJR
 */

#ifndef CODE_MENU_H_
#define CODE_MENU_H_
#include "zf_common_headfile.h"

#define     MENU_ENCODER_TIM            (TIM6_ENCODER)
#define     MENU_ENCODER_A_PIN          (TIM6_ENCODER_CH1_P20_3)
#define     MENU_ENCODER_B_PIN          (TIM6_ENCODER_CH2_P20_0)
#define     MENU_KEY_PIN                (P21_7)                     //��������ť����


//���º�������䶯
void display_arrow_mark(uint16 y,const uint16 color);
void menu_run(void);//�ͺ���key_scannerһ�����while�����
void clear_arrow_mark(void);

//�˵����޸���Ҫ�����º��������޸�
void menu_init(void);

//���ܺ���(���԰� ���ȡ�ĺ�����)
void set_run(void);
void set_speed(void);
void set_p(void);
void set_d(void);


#endif /* CODE_MENU_H_ */
