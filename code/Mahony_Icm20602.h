/*
 * Mahony_Icm20602.h
 *
 *  Created on: 2023Äê4ÔÂ20ÈÕ
 *      Author: Syhen_XX
 */

#ifndef CODE_MAHONY_ICM20602_H_
#define CODE_MAHONY_ICM20602_H_

#define alpha       (0.3f)
#define sampleFreq  (1000.0f)
#define mahony_Kp   (12.0f)
#define mahony_Ki   (0.0f)

extern volatile float icm_now_ax, icm_now_ay, icm_now_az;
extern volatile float icm_now_gx, icm_now_gy, icm_now_gz;
extern volatile float icm_offset_gx, icm_offset_gy, icm_offset_gz;

extern volatile float q0, q1, q2, q3;
extern volatile float integralFBx, integralFBy, integralFBz;
extern volatile float pitch, yaw, roll;

void mahony_init(void);
void icm_value_cala(void);
void mahony_cala(float gx, float gy, float gz, float ax, float ay, float az);
float invSqrt(float x);

#endif /* CODE_MAHONY_ICM20602_H_ */
