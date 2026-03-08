#include "MahonyAHRS.h"

//-------------------------------------------------------------------------------------------
// Definitions
//---------------------------------------***********************************
float twoKi;        // 2 * integral gain (Ki)
float q0, q1, q2, q3;   // quaternion of sensor frame relative to auxiliary frame
float integralFBx, integralFBy, integralFBz;  // integral error terms scaled by Ki
float invSampleFreq;
float roll_mahony, pitch_mahony, yaw_mahony;
float GZ;
icm_param_t icm_data;
euler_param_t eulerAngle;
//ButterworthFilter Butter_GyroX,Butter_GyroY,Butter_GyroZ;
//*-*-*-**-----------------------------------------------------------------------------

#define twoKpDef    (2.0f * 1.0f)   // 2 * proportional gain
#define twoKiDef    (2.0f * 0.0f)   // 2 * integral gain

float Mahony_invSqrt(float x)
{
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    y = y * (1.5f - (halfx * y * y));
    return y;
}

void IMU_GetValue()
{
    Get_Gyro_ICM42688();
    Get_Acc_ICM42688();

    icm_data.acc_x = icm42688_acc_x;
    icm_data.acc_y = icm42688_acc_y;
    icm_data.acc_z = icm42688_acc_z;

    //陀螺仪角度转弧度
    icm_data.gyro_x = (icm42688_gyro_x- GyroOff_X) * M_PI / 180;
    icm_data.gyro_y = (icm42688_gyro_y- GyroOff_Y) * M_PI / 180;
    icm_data.gyro_z = (icm42688_gyro_z - GyroOff_Z) * M_PI / 180;
    GZ = icm42688_gyro_z - GyroOff_Z;
}

// IMU更新函数，仅使用陀螺仪和加速度计数据
void MahonyAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az)
{
    float recipNorm;
    float halfvx, halfvy, halfvz;
    float halfex, halfey, halfez;
    float qa, qb, qc, qd;

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
    {

        // Normalise accelerometer measurement
        recipNorm = Mahony_invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Estimated direction of gravity and vector perpendicular to magnetic flux
        halfvx = q1 * q3 - q0 * q2;
        halfvy = q0 * q1 + q2 * q3;
        halfvz = q0 * q0 - 0.5f + q3 * q3;

        // Error is sum of cross product between estimated and measured direction of gravity
        halfex = (ay * halfvz - az * halfvy);
        halfey = (az * halfvx - ax * halfvz);
        halfez = (ax * halfvy - ay * halfvx);

        // Compute and apply integral feedback if enabled
        if(twoKi > 0.0f)
        {
            integralFBx += twoKi * halfex  * invSampleFreq; // integral error scaled by Ki
            integralFBy += twoKi * halfey  * invSampleFreq;
            integralFBz += twoKi * halfez  * invSampleFreq;
            gx += integralFBx;  // apply integral feedback
            gy += integralFBy;
            gz += integralFBz;
        }
        else
        {
            integralFBx = 0.0f; // prevent integral windup
            integralFBy = 0.0f;
            integralFBz = 0.0f;
        }

        // Apply proportional feedback
        gx += twoKpDef * halfex;
        gy += twoKpDef * halfey;
        gz += twoKpDef * halfez;
    }

    // Integrate rate of change of quaternion
    gx *= (0.5f  * invSampleFreq);      // pre-multiply common factors
    gy *= (0.5f  * invSampleFreq);
    gz *= (0.5f  * invSampleFreq);

    qa = q0;
    qb = q1;
    qc = q2;
    qd = q3;

    q0 += (-qb * gx - qc * gy - qd * gz);
    q1 += (qa * gx + qc * gz - qd * gy);
    q2 += (qa * gy - qb * gz + qd * gx);
    q3 += (qa * gz + qb * gy - qc * gx);

    //归一化
    recipNorm = Mahony_invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
}

void QuaternionsToEulerAngle()
{
    roll_mahony = asinf(-2.0f * (q1*q3 - q0*q2));
    eulerAngle.roll = 57.29578f * roll_mahony;

    pitch_mahony = atan2f(q0*q1 + q2*q3, 0.5f - q1*q1 - q2*q2);
    eulerAngle.pitch = 57.29578f * pitch_mahony;

    yaw_mahony = atan2f(q1*q2 + q0*q3, 0.5f - q2*q2 - q3*q3);
    eulerAngle.yaw = 57.29578f * yaw_mahony;

}

// 计算横滚角、俯仰角和偏航角
void Mahony_GetAngles()
{
    IMU_GetValue();                 //获取原始数据并转换为实际数据

    MahonyAHRSupdateIMU(icm_data.gyro_x, icm_data.gyro_y, icm_data.gyro_z, icm_data.acc_x, icm_data.acc_y, icm_data.acc_z);            //计算四元数

    QuaternionsToEulerAngle();              //四元数转化为欧拉角
}

void Get_InitAngle(float ax, float ay, float az)
{
    // 归一化加速度计数据
    float recipNorm = Mahony_invSqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;

    // 计算初始横滚角和俯仰角 (弧度)
    float roll_init = atan2f(ay, az);
    float pitch_init = atan2f(-ax, sqrtf(ay*ay + az*az));

    // 计算半角
    float cr = cosf(roll_init * 0.5f);
    float sr = sinf(roll_init * 0.5f);
    float cp = cosf(pitch_init * 0.5f);
    float sp = sinf(pitch_init * 0.5f);

    // 设置初始四元数
    q0 = cr * cp;
    q1 = sr * cp;
    q2 = cr * sp;
    q3 = -sr * sp;

    // 归一化四元数
    recipNorm = Mahony_invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
}

void Mahony_Init(float sampleFrequency)
{
    twoKi = twoKiDef;   // 2 * integral gain (Ki)
    q0 = 1.0f;
    q1 = 0.0f;
    q2 = 0.0f;
    q3 = 0.0f;

    integralFBx = 0.0f;
    integralFBy = 0.0f;
    integralFBz = 0.0f;
    invSampleFreq = 1.0f / sampleFrequency; // 采样频率的倒数，用于积分计算

    Delay(100);                   //暂时用延时解决，陀螺仪刚开始上电的数据并不准确，后期加入陀螺仪状态分析
    IMU_GetValue();
    Get_InitAngle(icm_data.acc_x, icm_data.acc_y, icm_data.acc_z);            //先使用加速度计获取一次初始的角度

    //init_arcsin_table();
}
