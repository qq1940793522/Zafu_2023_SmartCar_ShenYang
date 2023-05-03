/*
 * Car_Control.c
 *
 *  Created on: 2023Äê5ÔÂ3ÈÕ
 *      Author: Syhen_XX
 */

/*
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
 */


