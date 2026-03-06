/**************************************************************
 * 文件名:  motor.c
 * 创建时间: 2026年3月6日 下午5:00:30
 * 描述:
 **************************************************************/
#include "motor.h"
#include "encoder.h"
#include "tool.h"
#include "zf_driver_gpio.h"
#include "zf_driver_pwm.h"
#include "tool.h"
//速度环PID
IncPID pid_left = {11,0.5,0,0,  0,0,0}, pid_right = {11,0.5,0,0,0,0,0};
SteeringPID turnPID = {6.5, 0, 2.0};       // 转向外环：方向控制


void Motor_Init(void){
    gpio_init(DIR_R1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                           // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_R1, 17000, 0);                                                 // PWM 通道初始化频率 17KHz 占空比初始为 0
    gpio_init(DIR_L1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                           // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_L1, 17000, 0);                                                 // PWM 通道初始化频率 17KHz 占空比初始为 0
    turnPID.error = turnPID.last_error = turnPID.last_last_error = 0;
}

void L_set_PWM(int Speed){

    if(Speed>4000) Speed = 4000;
    if(Speed<0) Speed = 0;
    if(Speed>=0){
        gpio_set_level(DIR_L1, GPIO_HIGH);                                    // DIR输出高电平
        pwm_set_duty(PWM_L1, Speed);
    }


}

void R_set_PWM(int Speed){
    if(Speed>4000) Speed = 4000;
    if(Speed<0) Speed = 0;
    if(Speed>=0){
        gpio_set_level(DIR_R1, GPIO_HIGH);                                    // DIR输出高电平
        pwm_set_duty(PWM_R1, Speed);
    }
}

void speed_control_L(int target_speed_L) {
   // 设置目标速度
    pid_left.target = target_speed_L;


    // 获取编码器实际速度（需确保单位一致）
    int left_speed = Get_Encoder_L();


    // 增量PID计算
    int left_pwm = IncPID_Calculate(&pid_left, left_speed);


    // 输出限幅（根据硬件调整）
    left_pwm = constrain(left_pwm, -5000, 5000);

    // 驱动电机
    L_set_PWM(left_pwm);
    printf("%d,%d\n",target_speed_L,left_speed);
}

void speed_control_R(int target_speed_R) {
   // 设置目标速度
    pid_right.target = target_speed_R;


    // 获取编码器实际速度（需确保单位一致）
    int right_speed = Get_Encoder_R();


    // 增量PID计算
    int right_pwm = IncPID_Calculate(&pid_right, right_speed);


    // 输出限幅（根据硬件调整）
    right_pwm = constrain(right_pwm, -5000, 5000);

    //printf("R:%d \n",right_speed);
    // 驱动电机
    R_set_PWM(right_pwm);

    //printf("%d,%d\n",target_speed_R,right_speed);

    //ips200_show_int(30,160,right_pwm,3);

}



void combined_control(int base_speed, float path_deviation) {
    // 1. 转向控制：获取转向偏差量
    float steer_pwm = steering_control(path_deviation);

    // 2. 非对称差速策略
    float speed_k = steer_pwm * 0.01f;
    int left_speed, right_speed;



    if(steer_pwm >= 0) {
        // 左转：左轮小幅加速，右轮大幅减速
        left_speed = base_speed * (1 + speed_k * 0.1f);
        right_speed = base_speed * (1 - speed_k * 0.85f);
    } else {
        // 右转：左轮大幅减速，右轮小幅加速
        left_speed = base_speed * (1 - (-speed_k) * 0.85f);
        right_speed = base_speed * (1 + (-speed_k) * 0.1f);
    }

    // 3. 速度环控制
    speed_control_L(left_speed);
    speed_control_R(right_speed);
    //ips200_show_int(0,0,(int)steer_pwm,3);

}
