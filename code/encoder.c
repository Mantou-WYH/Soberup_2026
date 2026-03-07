/**************************************************************
 * 文件名:  encoder.c
 * 创建时间: 2026年3月6日 下午3:20:21
 * 描述:
 **************************************************************/
#include "encoder.h"
#include "zf_driver_encoder.h"

void Encoder_Init(void){
    encoder_dir_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B);
    encoder_dir_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B);
}


int Get_Encoder_L() {
    static float filtered_right = 0;  // 静态变量保存滤波后的值

    // 获取原始值并清零计数器
    int raw;
    raw = (int)encoder_get_count(ENCODER_1);
    encoder_clear_count(ENCODER_1);

    // 应用低通滤波公式
    filtered_right = FILTER_ALPHA * raw + (1 - FILTER_ALPHA) * filtered_right;

    // 返回整数值（四舍五入）
    return -(int)(filtered_right + 0.5f);
}

int Get_Encoder_R() {
    static float filtered_left = 0;  // 静态变量保存滤波后的值

    // 获取原始值并清零计数器
    int raw;
    raw = (int)encoder_get_count(ENCODER_2);
    encoder_clear_count(ENCODER_2);

    // 应用低通滤波公式
    filtered_left = FILTER_ALPHA * raw + (1 - FILTER_ALPHA) * filtered_left;

    // 返回整数值（四舍五入）
    return (int)(filtered_left + 0.5f);
}
