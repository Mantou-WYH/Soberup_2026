/*
 * menu.h
 *
 * OUC 沧海一粟ovo
 * 带有flash读写功能的多级菜单
 * 2026.3.14
 */
#ifndef _menu_h
#define  _menu_h

#include "zf_common_headfile.h"


void key_scan(void);//按键状态扫描
void menu_display(void);
void menu1(void);
void menu2_1(void);
void menu2_2(void);
void menu2_3(void);
void menu2_4(void);
void menu2_5(void);
void menu_init(void);

void param_r_flash(uint8 page);
void param_w_flash(uint8 page);
float adjust_parameter(float value, uint8_t digit, int8_t direction, uint8_t int_digits);
uint8_t get_integer_digits(float value);
void show_parameter_with_highlight(uint16_t x, uint16_t y, float value, uint8_t digit_pos, uint8_t int_digits);

void screen_init(void);
void screen_show_orin(void);
void navigation_state_change(void);


extern int8 set_f;
extern uint8 subject_id; // 保存当前选中的科目ID


static int8 menu1_flag; //一级菜单状态
static int8 menu2_flag; //二级菜单状态

//按键宏定义
#define KEY_DOWN                 (P11_2)
#define KEY_UP                   (P20_7)
#define KEY_RIGHT                (P20_6)
#define KEY_LEFT                 (P11_3)
#define KEY_PRESS                (P15_8)

#define SPEED_PAGE             1  //速度环存放页
#define DIR_PAGE               2 //转向环
#define IMAGE_PAGE             3  //图像参数存放页 (原曝光)

typedef enum
{
 //一级菜单
 MENU1_1 = 1,
 //二级菜单
 MENU2_1 = 2,
 MENU2_2 = 3,
 MENU2_3 = 4,
 MENU2_4 = 5,
 MENU2_5 = 6,
}MENU;    //用于定位菜单位置

typedef enum
{
  EMPTY = 0,
  PRESS = 1,
  UP = 2,
  DOWN = 3,
  LEFT = 4,
  RIGHT = 5,
}KEY_STATE; //全向开关状态

#endif
