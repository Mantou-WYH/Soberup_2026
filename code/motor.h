#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_common_headfile.h"
// 硬件引脚定义
#define DIR_R1              (P11_11)
#define PWM_R1              (ATOM2_CH7_P11_12)
#define DIR_L1              (P11_9)
#define PWM_L1              (ATOM3_CH5_P11_10)


// ************************* 全局变量声明 *************************
extern IncPID pid_left, pid_right;
extern SteeringPID turnPID;


// ************************* 函数声明 *************************
void Motor_Init(void);
void L_set_PWM(int Speed);
void R_set_PWM(int Speed);
void speed_control_L(int target_speed_L);
void speed_control_R(int target_speed_R);
void combined_control(int base_speed, float path_deviation);
int IncPID_Calculate(IncPID *pid, int actual);
float steering_control(float path_deviation);
void car_stop();
void negative_pressure_init();
void negative_pressure_start();
void negative_pressure_stop();



#endif /* CODE_MOTOR_H_ */
