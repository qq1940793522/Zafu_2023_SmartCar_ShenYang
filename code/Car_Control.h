/*
 * Car_Control.h
 *
 *  Created on: 2023Äê5ÔÂ5ÈÕ
 *      Author: CJR
 */

#ifndef CODE_CAR_CONTROL_H_
#define CODE_CAR_CONTROL_H_

extern volatile uint8 count;
extern volatile float pitch_out;
extern volatile float roll_out;
extern volatile float yaw_out;
extern volatile float car_speed;

uint8 car_check(void);
void car_banlance(void);
void car_control(int16 speed, float rotate);

#endif /* CODE_CAR_CONTROL_H_ */
