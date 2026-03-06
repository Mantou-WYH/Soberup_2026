#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#define DIR_R1              (P11_11)
#define PWM_R1              (ATOM2_CH7_P11_12)
#define DIR_L1              (P11_9)
#define PWM_L1              (ATOM3_CH5_P11_10)


void Motor_Init();
void L_set_PWM(int Speed);
void R_set_PWM(int Speed);
void speed_control_L(int target_speed_L);
void speed_control_R(int target_speed_R);
void combined_control(int base_speed, float path_deviation);

#endif /* CODE_MOTOR_H_ */
