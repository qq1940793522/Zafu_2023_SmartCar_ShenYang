/*
 * Menu.h
 *
 *  Created on: 2023年5月5日
 *      Author: CJR
 */

#ifndef CODE_MENU_H_
#define CODE_MENU_H_
#include "zf_common_headfile.h"

#define     MENU_ENCODER_TIM            (TIM6_ENCODER)
#define     MENU_ENCODER_A_PIN          (TIM6_ENCODER_CH1_P20_3)
#define     MENU_ENCODER_B_PIN          (TIM6_ENCODER_CH2_P20_0)
#define     MENU_KEY_PIN                (P20_2)                     //编码器旋钮引脚


void display_arrow_mark(uint16 y,const uint16 color);
void menu_init(void);
void menu_run(void);

void set_run(void);
void set_speed(void);
void set_p(void);
void set_d(void);


#endif /* CODE_MENU_H_ */
