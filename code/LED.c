/**************************************************************
 * 文件名:  LED.c
 * 创建时间: 2026年3月7日 下午4:17:01
 * 描述:
 **************************************************************/
#include "LED.h"


void LED_init(){
    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);          // 初始化 LED1 输出 默认高电平 推挽输出模式
    gpio_init(LED2, GPO, GPIO_HIGH, GPO_PUSH_PULL);         // 初始化 LED2 输出 默认高电平 推挽输出模式
    gpio_init(LED3, GPO, GPIO_HIGH, GPO_PUSH_PULL);          // 初始化 LED3 输出 默认高电平 推挽输出模式
    gpio_init(LED4, GPO, GPIO_HIGH, GPO_PUSH_PULL);         // 初始化 LED4 输出 默认高电平 推挽输出模式
}

// LED1 控制函数
void LED1_on(void)
{
    gpio_set_level(LED1, GPIO_LOW);   // 点亮 LED1（低电平点亮）
}
void LED1_off(void)
{
    gpio_set_level(LED1, GPIO_HIGH);  // 熄灭 LED1（高电平熄灭）
}

// LED2 控制函数
void LED2_on(void)
{
    gpio_set_level(LED2, GPIO_LOW);   // 点亮 LED2（低电平点亮）
}
void LED2_off(void)
{
    gpio_set_level(LED2, GPIO_HIGH);  // 熄灭 LED2（高电平熄灭）
}

// LED3 控制函数
void LED3_on(void)
{
    gpio_set_level(LED3, GPIO_LOW);   // 点亮 LED3（低电平点亮）
}
void LED3_off(void)
{
    gpio_set_level(LED3, GPIO_HIGH);  // 熄灭 LED3（高电平熄灭）
}

// LED4 控制函数
void LED4_on(void)
{
    gpio_set_level(LED4, GPIO_LOW);   // 点亮 LED4（低电平点亮）
}
void LED4_off(void)
{
    gpio_set_level(LED4, GPIO_HIGH);  // 熄灭 LED4（高电平熄灭）
}
