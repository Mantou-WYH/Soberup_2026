#ifndef CODE_TOOL_H_
#define CODE_TOOL_H_

void show_point_red(int x,int y);
void show_point_blue(int x,int y);
void show_point_yellow(int x,int y);
void show_point(int x,int y);

extern float dt;

// 增量式PID结构体（速度环）
typedef struct {
    float Kp;               // 比例系数
    float Ki;               // 积分系数
    float Kd;               // 微分系数
    int target;             // 目标值
    int error;              // 当前误差 e(k)
    int last_error;         // 上一次误差 e(k-1)
    int prev_error;    // 上上次误差 e(k-2)
    int last_output;        // 上一次最终PWM输出（增量PID核心）
} IncPID;

// 转向PID结构体（方向环）
typedef struct {
    float Kp;               // 比例系
    float Kd;               // 微分系数
    float GKD;              //陀螺仪阻尼
    float error;            // 当前偏差
    float last_error;       // 上一次偏差
    float prev_error;  // 上上次偏差
} SteeringPID;


typedef struct {
    float alpha; // 滤波器系数
    float previous_output; // 上一次的输出值
} LowPassFilter;

// 位置式PD控制
float steering_pd_control(SteeringPID *pid, float setpoint, float actual);

// 增量式PID控制
int IncPID_Calculate(IncPID *pid, int current_speed);

float calculateAngleDifference(float angle1, float angle2);

#define constrain(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))
#define max(a,b) a>b?a:b
#define min(a,b) a>b?b:a

#endif /* CODE_TOOL_H_ */
