/**************************************************************
 * 文件名:  tool.c
 * 创建时间: 2026年2月19日 下午9:55:57
 * 描述:
 **************************************************************/
#include "zf_common_headfile.h"
#define dt 0.05

void show_point_red(int x,int y){
    if(x>0 && x<MT9V03X_1_W&&y>0&&y<MT9V03X_1_H){
        ips200_draw_point((uint16)x,(uint16)y,RGB565_RED);
    }
}

void show_point_blue(int x,int y){
    if(x>0 && x<MT9V03X_1_W&&y>0&&y<MT9V03X_1_H){
        ips200_draw_point((uint16)x,(uint16)y,RGB565_BLUE);
    }
}

void show_point_yellow(int x,int y){
    if(x>0 && x<MT9V03X_1_W&&y>0&&y<MT9V03X_1_H){
        ips200_draw_point((uint16)x,(uint16)y,RGB565_YELLOW);
    }
}


//-------------------------------------------------------------------------------------------------------------------
//  函数简介      PID控制器初始化
//  返回类型     void
//  使用样例     PID_Init();
//  说明：       清零所有PID控制器的历史误差，避免初次计算时的积分积累
//-------------------------------------------------------------------------------------------------------------------
void PID_Init() {
    // 清零历史状态
    //turnPID.error = turnPID.last_error = turnPID.last_last_error = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      位置式PD控制器
//  返回类型     float（计算得到的控制量）
//  参数说明        pid PD控制器结构体指针
//              setpoint 目标值
//              actual 实际测量值
//  使用样例        float control = steering_pd_control(&turnPID, target_angle, actual_angle);
//  说明：       实现PD控制公式：output = Kp * e(t) + Kd * de(t)/dt
//              使用中心差分法计算微分项
//-------------------------------------------------------------------------------------------------------------------
float steering_pd_control(SteeringPID *pid, float setpoint, float actual) {
    float new_error = setpoint - actual;
    float derivative = (new_error - pid->last_error) / dt;  // 加入时间因子
    pid->last_last_error = pid->last_error;
    pid->last_error = new_error;
    pid->error = new_error;

    return (pid->Kp * new_error + pid->Kd * derivative);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      增量式PID控制器
//  返回类型     int（计算得到的控制量增量）
//  参数说明        pid PID控制器结构体指针
//              current_speed 当前测量值
//  使用样例        int speed_adjust = IncPID_Calculate(&motorPID, current_speed);
//  公式说明：   Δu(t) = Kp·[e(t)-e(t-1)] + Ki·e(t) + Kd·[e(t)-2e(t-1)+e(t-2)]
//-------------------------------------------------------------------------------------------------------------------
int IncPID_Calculate(IncPID *pid, int current_speed) {
    int error = pid->target - current_speed;


    // 计算PID各项
    int p_term = (int)pid->Kp * (error - pid->last_error);
    int i_term = (int)pid->Ki * error;
    int d_term = (int)pid->Kd * (error - 2 * pid->last_error + pid->prev_error);

    // 计算控制量增量
    int increment = p_term + i_term + d_term;

    // 计算本次输出
    int output = pid->last_output + increment;

    // 更新历史误差和输出
    pid->prev_error = pid->last_error;
    pid->last_error = error;
    pid->last_output = output;

    return output;
}
