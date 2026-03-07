#ifndef CODE_LED_H_
#define CODE_LED_H_

#include "zf_driver_gpio.h"

#define LED1                    (P20_9)
#define LED2                    (P20_8)
#define LED3                    (P21_5)
#define LED4                    (P21_4)

// LED1 控制函数
void LED1_on(void);
void LED1_off(void);

// LED2 控制函数
void LED2_on(void);
void LED2_off(void);

// LED3 控制函数
void LED3_on(void);
void LED3_off(void);

// LED4 控制函数
void LED4_on(void);
void LED4_off(void);

#endif /* CODE_LED_H_ */
