/**************************************************************
 * 文件名:  initialize.c
 * 创建时间: 2026年3月6日 下午6:13:05
 * 描述:
 **************************************************************/
#include "zf_common_headfile.h"


void init_mian(void){
    ips200_init(IPS200_TYPE_SPI);
    ips200_show_string(0, 0, "mt9v03x init.");
    while(1)
    {
        if(mt9v03x_double_init(mt9v03x_1))
            ips200_show_string(0, 80, "mt9v03x reinit.");
        else
            break;
        system_delay_ms(500);                                                   // 短延时快速闪灯表示异常
    }
    ips200_show_string(0, 16, "init success.");
    Motor_Init();
    Encoder_Init();
    voltage_detect_init();
    LED_init();

    pit_ms_init(CCU60_CH0,5);
}
