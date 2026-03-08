/**************************************************************
 * 文件名:  tool.c
 * 创建时间: 2026年2月19日  上午9:55:57
 * 功能: 通用工具函数（PID计算、滤波、图形显示等）
 **************************************************************/
#include "zf_common_headfile.h"
#include "tool.h"

// 全局时间步长（PID计算用）
float dt = 0.05f;

// 调试开关（0=关闭，1=开启）
#define DEBUG_PRINT 0

//float GZ;
/**************************************************************
 * 功能: 显示红色像素点
 * 参数: x/y - 像素坐标
 * 说明: 坐标范围保护，避免越界
 **************************************************************/
void show_point_red(int x, int y)
{
    if (x > 0 && x < MT9V03X_1_W && y > 0 && y < MT9V03X_1_H)
    {
        ips200_draw_point((uint16)x, (uint16)y, RGB565_RED);
    }
}

/**************************************************************
 * 功能: 显示蓝色像素点
 * 参数: x/y - 像素坐标
 **************************************************************/
void show_point_blue(int x, int y)
{
    if (x > 0 && x < MT9V03X_1_W && y > 0 && y < MT9V03X_1_H)
    {
        ips200_draw_point((uint16)x, (uint16)y, RGB565_BLUE);
    }
}

/**************************************************************
 * 功能: 显示黄色像素点
 * 参数: x/y - 像素坐标
 **************************************************************/
void show_point_yellow(int x, int y)
{
    if (x > 0 && x < MT9V03X_1_W && y > 0 && y < MT9V03X_1_H)
    {
        ips200_draw_point((uint16)x, (uint16)y, RGB565_YELLOW);
    }
}

/**************************************************************
 * 功能: 位置式PD控制器（转向控制专用）
 * 参数: pid - PD控制器结构体指针
 *       setpoint - 目标值（路径偏差目标）
 *       actual - 实际值（当前路径偏差）
 * 返回: PD控制输出值（转向PWM补偿量）
 * 公式: output = Kp*e(t) + Kd*[e(t)-e(t-1)]/dt
 **************************************************************/
float steering_pd_control(SteeringPID *pid, float setpoint, float actual)
{
    // 计算当前误差
    float current_error = setpoint - actual;

    // 计算微分项（误差变化率）
    float derivative = (current_error - pid->last_error) / dt;

    // 更新误差历史
    pid->prev_error = pid->last_error;
    pid->last_error = current_error;
    pid->error = current_error;

    // PD输出计算
    float output = pid->Kp * current_error + pid->Kd * derivative + pid->GKD * GZ ;

    return output;
}

/**************************************************************
 * 功能: 增量式PID计算（电机速度控制专用）
 * 参数: pid - 增量式PID控制器结构体指针
 *       current_speed - 当前实际速度（编码器反馈）
 * 返回: 本次PID增量输出（PWM调整量）
 * 公式: Δu(t) = Kp*[e(t)-e(t-1)] + Ki*e(t) + Kd*[e(t)-2e(t-1)+e(t-2)]
 **************************************************************/
int IncPID_Calculate(IncPID *pid, int current_speed)
{
    // 校验PID结构体指针
    if (pid == NULL)
    {
        return 0;
    }

    // 计算当前误差（目标速度 - 实际速度）
    int error = pid->target - current_speed;

    // 计算PID各分项（保留浮点精度，最后转int）
    float p_term = pid->Kp * (error - pid->last_error);
    float i_term = pid->Ki * error;
    float d_term = pid->Kd * (error - 2 * pid->last_error + pid->prev_error);

    // 计算增量值
    int increment = (int)(p_term + i_term + d_term);

    // 计算本次输出（上次输出 + 增量）
    int output = pid->last_output + increment;

    // 更新PID历史数据
    pid->prev_error = pid->last_error;
    pid->last_error = error;
    pid->last_output = output;

    return output;
}
