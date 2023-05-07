/*
 * Car_Control.h
 *
 *  Created on: 2023Äê5ÔÂ5ÈÕ
 *      Author: CJR
 */

#ifndef CODE_CAR_CONTROL_H_
#define CODE_CAR_CONTROL_H_

extern volatile float pitch_out;
extern volatile float roll_out;
extern volatile float yaw_out;

void car_banlance(void);
void car_control(uint8 speed, uint8 rotate);

#endif /* CODE_CAR_CONTROL_H_ */
