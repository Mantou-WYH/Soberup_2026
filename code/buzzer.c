/**************************************************************
 * 文件名:  buzzer.c
 * 创建时间: 2026年3月7日 下午4:39:09
 * 描述:
 **************************************************************/
#include "zf_driver_gpio.h"

#define BUZZER_PIN              (P33_10)


void buzzer_init(){
    gpio_init(BUZZER_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);                        // 初始化 BUZZER_PIN 输出 默认低电平 推挽输出模式
}

void buzzer_on(){
    gpio_set_level(BUZZER_PIN, GPIO_HIGH);
}

void buzzer_off(){
    gpio_set_level(BUZZER_PIN, GPIO_LOW);
}
