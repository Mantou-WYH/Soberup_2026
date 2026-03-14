/**************************************************************
 * 文件名:  motor.c
 * 创建时间: 2026年3月6日  下午5:00:30
 * 功能: 电机驱动与速度控制（PID闭环、路径偏差联合控制）
 **************************************************************/
#include "motor.h"

// 调试开关（0=关闭，1=开启）
#define DEBUG_PRINT 0

// 硬件参数宏定义（统一管理，方便调试）
#define PWM_FREQ_HZ        17000    // PWM频率
#define PWM_MAX            3000     // PWM输出上限
#define encoder_MAX        500     // PWM输出上限
#define PWM_MIN            0        // PWM输出下限
#define PID_OUTPUT_MAX     5000     // PID输出约束上限
#define PID_OUTPUT_MIN     -5000    // PID输出约束下限
#define STEER_SPEED_FACTOR 0.01f    // 转向速度补偿系数
#define LEFT_STEER_FACTOR  0.1f     // 左转速度调整系数
#define RIGHT_STEER_FACTOR 0.85f    // 右转速度调整系数

// 速度PID控制器（左/右电机）
IncPID pid_left = {6.0, 0.3f, 0.0f, 0, 0, 0, 0};
IncPID pid_right = {6.0f, 0.3f, 0.0f, 0, 0, 0, 0};

// 转向PD控制器（路径偏差控制）
SteeringPID turnPID = {1.5f, 0.0f, 0.05f, 0.0f, 0.0f, 0.0f};

int dis_R = 0,dis_L = 0;
int stop_flag = 0;

void negative_pressure_init(){
    gpio_init(P22_1,GPO , GPIO_LOW,GPO_PUSH_PULL);
    gpio_init(P23_1,GPO , GPIO_LOW,GPO_PUSH_PULL);
}

void negative_pressure_start(){
    gpio_set_level(P22_1, GPIO_LOW);
    gpio_set_level(P23_1, GPIO_HIGH);
}

void negative_pressure_stop(){
    gpio_set_level(P22_1, GPIO_LOW);
    gpio_set_level(P23_1, GPIO_LOW);
}

/**************************************************************
 * 功能: 电机初始化（GPIO/PWM/控制器）
 * 参数: 无
 * 说明: 初始化方向引脚为推挽输出，PWM频率17KHz，初始占空比0
 **************************************************************/
void Motor_Init(void)
{
    // 右电机方向引脚初始化（默认高电平）
    gpio_init(DIR_R1, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    // 右电机PWM引脚初始化（17KHz，占空比0）
    pwm_init(PWM_R1, PWM_FREQ_HZ, 0);

    // 左电机方向引脚初始化（默认高电平）
    gpio_init(DIR_L1, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    // 左电机PWM引脚初始化（17KHz，占空比0）
    pwm_init(PWM_L1, PWM_FREQ_HZ, 0);

    // 初始化转向PID控制器误差值
    turnPID.error = turnPID.last_error = turnPID.prev_error = 0.0f;


    /////////负压初始化////////
    negative_pressure_init();

}



/**************************************************************
 * 功能: 设置左电机PWM值
 * 参数: Speed - PWM值（正=正转，负=反转）
 * 说明: 限制PWM范围，根据正负控制方向引脚
 **************************************************************/
void L_set_PWM(int Speed)
{
    // 限制PWM输出范围
    int pwm = constrain(Speed, PWM_MIN, PWM_MAX);

    // 控制电机方向
    if (pwm > 0)
    {
        gpio_set_level(DIR_L1, GPIO_HIGH);  // 正转（高电平）
        pwm_set_duty(PWM_L1, pwm);
    }
    else if (pwm < 0)
    {
        gpio_set_level(DIR_L1, GPIO_LOW);   // 反转（低电平）
        pwm_set_duty(PWM_L1, -pwm);
    }
    else
    {
        pwm_set_duty(PWM_L1, 0);            // 停止（占空比0）
    }
}

/**************************************************************
 * 功能: 设置右电机PWM值
 * 参数: Speed - PWM值（正=正转，负=反转）
 **************************************************************/
void R_set_PWM(int Speed)
{
    // 限制PWM输出范围
    int pwm = constrain(Speed, PWM_MIN, PWM_MAX);

    // 控制电机方向
    if (pwm > 0)
    {
        gpio_set_level(DIR_R1, GPIO_HIGH);  // 正转（高电平）
        pwm_set_duty(PWM_R1, pwm);
    }
    else if (pwm < 0)
    {
        gpio_set_level(DIR_R1, GPIO_LOW);   // 反转（低电平）
        pwm_set_duty(PWM_R1, -pwm);
    }
    else
    {
        pwm_set_duty(PWM_R1, 0);            // 停止（占空比0）
    }
}

/**************************************************************
 * 功能: 左电机速度PID闭环控制
 * 参数: target_speed_L - 左电机目标速度
 * 说明: 读取编码器反馈，计算PID，输出PWM到电机
 **************************************************************/
void speed_control_L(int target_speed_L)
{
    // 校验目标速度（避免异常值）
    if (target_speed_L < 0) target_speed_L = 0;

    // 设置PID目标值
    pid_left.target = target_speed_L;

    // 读取左编码器实际速度
    int left_speed = Get_Encoder_L();

    dis_L += left_speed;

    // 增量式PID计算
    int left_pwm = IncPID_Calculate(&pid_left, left_speed);

    // 约束PID输出范围（硬件保护）
    left_pwm = constrain(left_pwm, PID_OUTPUT_MIN, PID_OUTPUT_MAX);

    // 输出PWM到左电机
    //L_set_PWM(left_pwm);

    //ips200_show_int(0,120,dis_L,10);

    // 调试输出（目标速度、实际速度）
    #if DEBUG_PRINT
    printf("%d, %d\n", target_speed_L, left_speed);
    #endif
}

/**************************************************************
 * 功能: 右电机速度PID闭环控制
 * 参数: target_speed_R - 右电机目标速度
 **************************************************************/
void speed_control_R(int target_speed_R)
{
    // 校验目标速度（避免异常值）
    if (target_speed_R < 0) target_speed_R = 0;

    // 设置PID目标值
    pid_right.target = target_speed_R;

    // 读取右编码器实际速度
    int right_speed = Get_Encoder_R();

    dis_R += right_speed;

    // 增量式PID计算
    int right_pwm = IncPID_Calculate(&pid_right, right_speed);

    // 约束PID输出范围（硬件保护）
    right_pwm = constrain(right_pwm, PID_OUTPUT_MIN, PID_OUTPUT_MAX);

    // 输出PWM到右电机
    //R_set_PWM(right_pwm);

    //ips200_show_int(0,140,dis_R,10);

    // 调试输出（目标速度、实际速度）
    #if DEBUG_PRINT
    printf("%d, %d\n", target_speed_R, right_speed);
    #endif
}

/**************************************************************
 * 功能: 转向控制（路径偏差转PWM补偿量）
 * 参数: path_deviation - 路径偏差值（单位：像素/毫米）
 * 返回: 转向PWM补偿量
 * 说明: 调用位置式PD控制器计算转向补偿
 **************************************************************/
static float steering_control(float path_deviation)
{
    // 路径偏差的目标值为0（沿中心线行驶）
    float steer_pwm = steering_pd_control(&turnPID, 0.0f, path_deviation);

    // 约束转向补偿范围（避免过度转向）
    steer_pwm = constrain(steer_pwm, -100.0f, 100.0f);

    return steer_pwm;
}

/**************************************************************
 * 功能: 联合控制（基础速度+路径偏差）
 * 参数: base_speed - 基础行驶速度
 *       path_deviation - 路径偏差（正=偏右，负=偏左）
 * 说明: 根据路径偏差调整左右电机速度，实现纠偏
 **************************************************************/
void combined_control(int base_speed, float path_deviation)
{
    // 校验基础速度（避免异常值）
    //printf("%d\n",base_speed);
    base_speed = constrain(base_speed, 0, encoder_MAX);


    // 1. 计算转向补偿量（PD控制）
    float steer_pwm = steering_control(path_deviation);

    // 2. 计算速度调整系数
    float speed_k = steer_pwm * STEER_SPEED_FACTOR;
    int left_speed, right_speed;

    // 3. 根据转向补偿调整左右电机速度
    if (steer_pwm >= 0)
    {
        // 偏右：左电机加速，右电机减速
        left_speed = (int)(base_speed * (1 + speed_k * LEFT_STEER_FACTOR));
        right_speed = (int)(base_speed * (1 - speed_k * RIGHT_STEER_FACTOR));
    }
    else
    {
        // 偏左：左电机减速，右电机加速
        left_speed = (int)(base_speed * (1 - (-speed_k) * RIGHT_STEER_FACTOR));
        right_speed = (int)(base_speed * (1 + (-speed_k) * LEFT_STEER_FACTOR));
    }

    // 4. 速度约束（避免超量程）
    left_speed = constrain(left_speed, 0, PWM_MAX);
    right_speed = constrain(right_speed, 0, PWM_MAX);

    // 5. 执行速度闭环控制
    if(stop_flag == 0){
        speed_control_L(left_speed);
        speed_control_R(right_speed);
    }else{
        speed_control_L(0);
        speed_control_R(0);
    }


    // 调试输出（转向补偿、左右目标速度）

}

void car_stop(){
    stop_flag = 1;
}


