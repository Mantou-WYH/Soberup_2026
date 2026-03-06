#ifndef CODE_ENCODER_H_
#define CODE_ENCODER_H_

#define ENCODER_1                   (TIM4_ENCODER)
#define ENCODER_1_A                 (TIM4_ENCODER_CH1_P02_8)
#define ENCODER_1_B                 (TIM4_ENCODER_CH2_P33_5)

#define ENCODER_2                   (TIM6_ENCODER)
#define ENCODER_2_A                 (TIM6_ENCODER_CH1_P20_3)
#define ENCODER_2_B                 (TIM6_ENCODER_CH2_P20_0)

#define FILTER_ALPHA 0.15f  // 滤波系数 (0.1-0.3之间

void Encoder_Init();
int Get_Encoder_R();
int Get_Encoder_L();

#endif /* CODE_ENCODER_H_ */
