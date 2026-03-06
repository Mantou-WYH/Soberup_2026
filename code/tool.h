#ifndef CODE_TOOL_H_
#define CODE_TOOL_H_

void show_point_red(int x,int y);
void show_point_blue(int x,int y);
void show_point_yellow(int x,int y);

extern float dt;

typedef struct {
    float Kp, Ki, Kd;
    int target;         // 目标速度
    int last_error;     // 上次误差
    int prev_error;     // 上上次误差
    int last_output;    // 上次输出
} IncPID;

typedef struct {
    float Kp, Ki, Kd;
    float error, last_error, last_last_error;
} SteeringPID;


typedef struct {
    float alpha; // 滤波器系数
    float previous_output; // 上一次的输出值
} LowPassFilter;

// 位置式PD控制
float steering_pd_control(SteeringPID *pid, float setpoint, float actual);

// 增量式PID控制
int IncPID_Calculate(IncPID *pid, int current_speed);

#define constrain(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))
#define max(a,b) a>b?a:b
#define min(a,b) a>b?b:a

#endif /* CODE_TOOL_H_ */
