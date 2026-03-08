#ifndef __MahonyAHRS_H
#define __MahonyAHRS_H


#include "icm42688.h"

#define Mahony_PIT  CCU60_CH1

#define GyroOff_X 0.6132f
#define GyroOff_Y -0.6845f
#define GyroOff_Z -0.1585f             //整定好的角速度零飘值
#define Delay(time)     system_delay_ms(time)

#define M_PI 3.14159f

typedef struct
{
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float acc_x;
    float acc_y;
    float acc_z;
} icm_param_t;

typedef struct
{
    float pitch;
    float roll;
    float yaw;
} euler_param_t;

typedef enum
{
    IMU_STATE_UNSTABLE = 0,   // 传感器未稳定
    IMU_STATE_STATIC,         // 静止状态
    IMU_STATE_DYNAMIC         // 运动状态
} imu_state_t;              //IMU状态标志

extern icm_param_t icm_data;
extern euler_param_t eulerAngle;
extern float GZ;
void Mahony_Init(float sampleFrequency);
void MahonyAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);
void Mahony_GetAngles(void);

#endif

