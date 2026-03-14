/*
 * menu.c V3
 * OUC 沧海一粟ovo
 * 带有flash读写功能的多级菜单 - 适配IncPID与SteeringPID参数
 *
 * 2025.7.2
 */

#include "zf_common_headfile.h"
#include <string.h>
#include <math.h>

// 限制数据范围宏
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

int8 set_f = 0;
int32 Save=0;
KEY_STATE key = 0;
MENU menu = MENU1_1;
float params[40]={0}; //用于转换投递flash的数组
uint8 subject_id = 1; // 默认科目1
float basic_speed = 0; // 基础速度

//-------------------------------------------------------------------------------------------------------------------
// 按键扫描
//-------------------------------------------------------------------------------------------------------------------
void key_scan(void)
{
  if(!gpio_get_level(KEY_UP))
   {
        system_delay_ms(20);
        while(!gpio_get_level(KEY_UP)) ;
        system_delay_ms(20);
        key=UP;
        return;
  }
  if(!gpio_get_level(KEY_DOWN))
   {
        system_delay_ms(20);
        while(!gpio_get_level(KEY_DOWN)) ;
        system_delay_ms(20);
        key=DOWN;
        return;
  }
  if(!gpio_get_level(KEY_RIGHT))
   {
        system_delay_ms(20);
        while(!gpio_get_level(KEY_RIGHT)) ;
        system_delay_ms(20);
        key=RIGHT;
        return;
  }
  if(!gpio_get_level(KEY_LEFT))
   {
        system_delay_ms(20);
        while(!gpio_get_level(KEY_LEFT)) ;
        system_delay_ms(20);
        key=LEFT;
        return;
  }
  if(!gpio_get_level(KEY_PRESS))
   {
        system_delay_ms(20);
        while(!gpio_get_level(KEY_PRESS)) ;
        system_delay_ms(20);
        key=PRESS;
        return;
  }
  key = EMPTY;
}

//-------------------------------------------------------------------------------------------------------------------
// 菜单初始化 (读取Flash写入到相应的新结构体中)
//-------------------------------------------------------------------------------------------------------------------
void menu_init(void)
{
  // --- 读取速度环参数 ---
  memset(params,0,40*sizeof(float));
  param_r_flash(SPEED_PAGE);
  basic_speed = params[0];

  memcpy(&pid_left, &params[1], sizeof(IncPID));
  memcpy(&pid_right, &params[1 + sizeof(IncPID)/4], sizeof(IncPID));

  // --- 读取转向环参数 ---
  memset(params,0,40*sizeof(float));
  param_r_flash(DIR_PAGE);
  memcpy(&turnPID, params, sizeof(SteeringPID));
}

//-------------------------------------------------------------------------------------------------------------------
// 菜单显示主循环
//-------------------------------------------------------------------------------------------------------------------
void menu_display(void)
{
  while(!set_f)
  {
   key_scan();
   switch (menu)
   {
   case MENU1_1:
    menu1();
    break;
   case MENU2_1:
    menu2_1(); // 发车
    break;
   case MENU2_2:
    menu2_2(); // 转向环调试
    break;
   case MENU2_3:
    menu2_3(); // 速度环调试
    break;
   case MENU2_4:
    menu2_4(); // 图像显示
    break;
   case MENU2_5:
    menu2_5(); // 科目选择
    break;
   }
  }
  ips200_clear(); // 发车后清屏
}

//-------------------------------------------------------------------------------------------------------------------
// 一级菜单
//-------------------------------------------------------------------------------------------------------------------
void menu1(void)
{
  uint8 line_index;
  ips200_show_string (48, 16*1, "1.Start Run");    //发车
  ips200_show_string (48, 16*2, "2.Dir Loop");     //转向环调试
  ips200_show_string (48, 16*3, "3.Speed Loop");   //速度环调试
  ips200_show_string (48, 16*4, "4.Image Disp");   //图像显示
  ips200_show_string (48, 16*5, "5.Subject Sel");  //科目选择

  if(key == DOWN)
  {
    menu1_flag++;
    menu1_flag > 4 ? menu1_flag = 0 : menu1_flag;
    ips200_clear();
  }
  else if(key == UP)
  {
    menu1_flag--;
    menu1_flag < 0 ? menu1_flag = 4 : menu1_flag;
    ips200_clear();
  }

  line_index = menu1_flag + 1;
  ips200_show_string(0, 16*line_index, "--->");

  if(key == PRESS)
  {
    switch(menu1_flag)
    {
     case 0: menu = MENU2_1; break;
     case 1: menu = MENU2_2; break;
     case 2: menu = MENU2_3; break;
     case 3: menu = MENU2_4; break;
     case 4: menu = MENU2_5; break;
    }
    menu2_flag = 0; // 进入二级菜单时重置光标
    ips200_clear();
  }
}

//-------------------------------------------------------------------------------------------------------------------
// 参数高亮显示函数 (ips200)
//-------------------------------------------------------------------------------------------------------------------
void show_parameter_with_highlight(uint16_t x, uint16_t y, float value,
                                  uint8_t digit_pos, uint8_t int_digits)
{
    char str[20];
    if (fabsf(value - (int)value) < 0.001f) {
        sprintf(str, "%d.00", (int)value);
    } else if (fabsf(value * 10 - (int)(value * 10)) < 0.001f) {
        sprintf(str, "%.1f0", value);
    } else {
        sprintf(str, "%.2f", value);
    }

    uint8_t total_chars = strlen(str);
    uint8_t decimal_index = 0;

    for (uint8_t i = 0; i < total_chars; i++) {
        if (str[i] == '.') {
            decimal_index = i;
            break;
        }
    }

    uint8_t highlight_index = 0;
    if (digit_pos < int_digits) {
        highlight_index = digit_pos;
    } else if (digit_pos == int_digits) {
        highlight_index = decimal_index;
    } else {
        highlight_index = decimal_index + 1 + (digit_pos - int_digits - 1);
        if (highlight_index >= total_chars) highlight_index = total_chars - 1;
    }

    for (uint8_t i = 0; i < total_chars; i++) {
        if (i == highlight_index) {
            ips200_set_color(RGB565_RED, RGB565_WHITE);
        } else {
            ips200_set_color(RGB565_BLACK, RGB565_WHITE);
        }
        ips200_show_char(x + i * 6, y, str[i]);
    }
    ips200_set_color(RGB565_BLACK, RGB565_WHITE);
}

//-------------------------------------------------------------------------------------------------------------------
// 步进调节参数
//-------------------------------------------------------------------------------------------------------------------
float adjust_parameter(float value, uint8_t digit, int8_t direction, uint8_t int_digits)
{
    int is_integer_part = (digit < int_digits);
    float step = 0.0f;

    if (is_integer_part) {
        step = powf(10.0f, int_digits - digit - 1);
    } else if (digit > int_digits) {
        step = powf(10.0f, -(digit - int_digits));
    }

    float new_value = value + (direction * step);
    new_value = CLAMP(new_value, 0.0f, 999.99f);

    return new_value;
}

//-------------------------------------------------------------------------------------------------------------------
// 获取整数位数
//-------------------------------------------------------------------------------------------------------------------
uint8_t get_integer_digits(float value)
{
    if (value < 0) value = -value;
    int integer_part = (int)value;

    if (integer_part == 0) return 1;

    uint8_t digits = 0;
    while (integer_part > 0) {
        digits++;
        integer_part /= 10;
    }
    return digits;
}

//-------------------------------------------------------------------------------------------------------------------
// 子菜单1：发车
//-------------------------------------------------------------------------------------------------------------------
void menu2_1(void)
{
    ips200_show_string(48, 16*2, "Ready to Go!");
    ips200_show_string(48, 16*4, "PRESS -> Run");
    ips200_show_string(48, 16*5, "LEFT  -> Back");

    if (key == PRESS) {
        set_f = 1; // 退出菜单循环，开始主程序
    } else if (key == LEFT) {
        menu = MENU1_1;
        ips200_clear();
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 子菜单2：转向环调试 (调节 turnPID 的 Kp, Kd, GKD)
//-------------------------------------------------------------------------------------------------------------------
void menu2_2(void)
{
    static uint8_t edit_mode = 0;
    static uint8_t digit_pos = 0;
    static float last_value = 0;
    static uint8_t int_digits = 0;
    static uint8_t total_digits = 0;

    // 显示名称
    ips200_show_string(48, 16*0, "Kp");
    ips200_show_string(48, 16*1, "Kd");
    ips200_show_string(48, 16*2, "Gkd");

    if (!edit_mode) {
        for (uint8_t i = 0; i < 3; i++) {
            float value = (i == 0) ? turnPID.Kp :
                          (i == 1) ? turnPID.Kd :
                          turnPID.GKD;

            char str[20];
            if (fabsf(value - (int)value) < 0.001f) {
                sprintf(str, "%d.00", (int)value);
            } else if (fabsf(value * 10 - (int)(value * 10)) < 0.001f) {
                sprintf(str, "%.1f0", value);
            } else {
                sprintf(str, "%.2f", value);
            }
            ips200_show_string(100, 16*i, str);
        }

        if (key == DOWN) {
            menu2_flag = (menu2_flag + 1) % 3;
            ips200_clear();
        }
        else if (key == UP) {
            menu2_flag = (menu2_flag + 2) % 3;
            ips200_clear();
        }
        else if (key == PRESS) {
            edit_mode = 1;
            digit_pos = 0;

            float current_value = (menu2_flag == 0) ? turnPID.Kp :
                                  (menu2_flag == 1) ? turnPID.Kd :
                                  turnPID.GKD;

            last_value = current_value;
            int_digits = get_integer_digits(current_value);
            total_digits = int_digits + 3;
        }
        else if (key == LEFT) {
            // 退出并默认保存Flash
            menu = MENU1_1;
            memset(params, 0, 40*sizeof(float));
            memcpy(params, &turnPID, sizeof(SteeringPID));
            param_w_flash(DIR_PAGE);

            ips200_show_string(100, 16*6, "SAVE");
            system_delay_ms(500);

            ips200_clear();
            return;
        }

        ips200_show_string(0, 16*menu2_flag, "--->");
    }
    else {
        float *current_param = NULL;
        switch (menu2_flag) {
            case 0: current_param = &turnPID.Kp; break;
            case 1: current_param = &turnPID.Kd; break;
            case 2: current_param = &turnPID.GKD; break;
        }

        int_digits = get_integer_digits(*current_param);
        total_digits = int_digits + 3;

        for (uint8_t i = 0; i < 3; i++) {
            float value = (i == 0) ? turnPID.Kp :
                          (i == 1) ? turnPID.Kd :
                          turnPID.GKD;

            if (i == menu2_flag) {
                show_parameter_with_highlight(100, 16*i, value, digit_pos, int_digits);
            } else {
                char str[20];
                if (fabsf(value - (int)value) < 0.001f) {
                    sprintf(str, "%d.00", (int)value);
                } else if (fabsf(value * 10 - (int)(value * 10)) < 0.001f) {
                    sprintf(str, "%.1f0", value);
                } else {
                    sprintf(str, "%.2f", value);
                }
                ips200_show_string(100, 16*i, str);
            }
        }

        if (key == UP) {
            if (digit_pos != int_digits) {
                *current_param = adjust_parameter(*current_param, digit_pos, 1, int_digits);
            }
        }
        else if (key == DOWN) {
            if (digit_pos != int_digits) {
                *current_param = adjust_parameter(*current_param, digit_pos, -1, int_digits);
            }
        }
        else if (key == RIGHT) {
            digit_pos = (digit_pos + 1) % total_digits;
        }
        else if (key == LEFT) {
            digit_pos = (digit_pos - 1 + total_digits) % total_digits;
        }
        else if (key == PRESS) {
            edit_mode = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 子菜单3：速度环调试 (调节 basic_speed 及左右电机的 Kp, Ki)
//-------------------------------------------------------------------------------------------------------------------
void menu2_3(void)
{
    static uint8_t edit_mode = 0;
    static uint8_t digit_pos = 0;
    static float last_value = 0;
    static uint8_t int_digits = 0;
    static uint8_t total_digits = 0;

    // 显示名称
    ips200_show_string(48, 16*0, "speed");
    ips200_show_string(48, 16*1, "l_kp");
    ips200_show_string(48, 16*2, "l_ki");
    ips200_show_string(48, 16*3, "r_kp");
    ips200_show_string(48, 16*4, "r_ki");

    if (!edit_mode) {
        for (uint8_t i = 0; i < 5; i++) {
            float value = (i == 0) ? basic_speed :
                          (i == 1) ? pid_left.Kp :
                          (i == 2) ? pid_left.Ki :
                          (i == 3) ? pid_right.Kp :
                          pid_right.Ki;

            char str[20];
            if (fabsf(value - (int)value) < 0.001f) {
                sprintf(str, "%d.00", (int)value);
            } else if (fabsf(value * 10 - (int)(value * 10)) < 0.001f) {
                sprintf(str, "%.1f0", value);
            } else {
                sprintf(str, "%.2f", value);
            }
            ips200_show_string(100, 16*i, str);
        }

        if (key == DOWN) {
            menu2_flag = (menu2_flag + 1) % 5;
            ips200_clear();
        }
        else if (key == UP) {
            menu2_flag = (menu2_flag + 4) % 5;
            ips200_clear();
        }
        else if (key == PRESS) {
            edit_mode = 1;
            digit_pos = 0;

            float current_value = (menu2_flag == 0) ? basic_speed :
                                  (menu2_flag == 1) ? pid_left.Kp :
                                  (menu2_flag == 2) ? pid_left.Ki :
                                  (menu2_flag == 3) ? pid_right.Kp :
                                  pid_right.Ki;

            last_value = current_value;
            int_digits = get_integer_digits(current_value);
            total_digits = int_digits + 3;
        }
        else if (key == LEFT) {
            // 退出并默认保存Flash
            menu = MENU1_1;
            memset(params, 0, 40*sizeof(float));
            params[0] = basic_speed;
            memcpy(&params[1], &pid_left, sizeof(IncPID));
            memcpy(&params[1 + sizeof(IncPID)/4], &pid_right, sizeof(IncPID));
            param_w_flash(SPEED_PAGE);

            ips200_show_string(100, 16*6, "SAVE");
            system_delay_ms(500);

            ips200_clear();
            return;
        }
        ips200_show_string(0, 16*menu2_flag, "--->");
    }
    else {
        float *current_param = NULL;
        switch (menu2_flag) {
            case 0: current_param = &basic_speed; break;
            case 1: current_param = &pid_left.Kp; break;
            case 2: current_param = &pid_left.Ki; break;
            case 3: current_param = &pid_right.Kp; break;
            case 4: current_param = &pid_right.Ki; break;
        }

        int_digits = get_integer_digits(*current_param);
        total_digits = int_digits + 3;

        for (uint8_t i = 0; i < 5; i++) {
            float value = (i == 0) ? basic_speed :
                          (i == 1) ? pid_left.Kp :
                          (i == 2) ? pid_left.Ki :
                          (i == 3) ? pid_right.Kp :
                          pid_right.Ki;

            if (i == menu2_flag) {
                show_parameter_with_highlight(100, 16*i, value, digit_pos, int_digits);
            } else {
                char str[20];
                if (fabsf(value - (int)value) < 0.001f) {
                    sprintf(str, "%d.00", (int)value);
                } else if (fabsf(value * 10 - (int)(value * 10)) < 0.001f) {
                    sprintf(str, "%.1f0", value);
                } else {
                    sprintf(str, "%.2f", value);
                }
                ips200_show_string(100, 16*i, str);
            }
        }

        if (key == UP) {
            if (digit_pos != int_digits) {
                *current_param = adjust_parameter(*current_param, digit_pos, 1, int_digits);
            }
        }
        else if (key == DOWN) {
            if (digit_pos != int_digits) {
                *current_param = adjust_parameter(*current_param, digit_pos, -1, int_digits);
            }
        }
        else if (key == RIGHT) {
            digit_pos = (digit_pos + 1) % total_digits;
        }
        else if (key == LEFT) {
            digit_pos = (digit_pos - 1 + total_digits) % total_digits;
        }
        else if (key == PRESS) {
            edit_mode = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 子菜单4：图像显示
//-------------------------------------------------------------------------------------------------------------------
void menu2_4(void)
{

    // TODO: 预留放图像显示的函数
    if(mt9v03x_finish_flag_1)
        {
            binarization(otsuThreshold_fast());
            ips200_displayimage03x((const uint8 *)mt9v03x_image_bin, MT9V03X_1_W, MT9V03X_1_H);                       // 显示原始图像
//            ips200_show_gray_image(0, 0, (const uint8 *)mt9v03x_image_1, MT9V03X_1_W, MT9V03X_1_H, 240, 180, 64);     // 显示二值化图像


            search_line_main();
            caculate_error();

            element_update();
            state_update();
            show_state();
            show_line();

            mt9v03x_finish_flag_1 = 0;
        }

    if(key == LEFT)
    {
        menu = MENU1_1;
        ips200_clear();
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 子菜单5：科目选择
//-------------------------------------------------------------------------------------------------------------------
void menu2_5(void)
{
    ips200_show_string(48, 16*1, "Subject 1");
    ips200_show_string(48, 16*2, "Subject 2");
    ips200_show_string(48, 16*3, "Subject 3");

    // 显示当前选中项的指示符
    ips200_show_string(0, 16*(menu2_flag + 1), "--->");

    // 右上角提示当前已生效科目
    char str[20];
    sprintf(str, "Cur:%d", subject_id);
    ips200_show_string(140, 0, str);

    if(key == DOWN)
    {
        menu2_flag++;
        menu2_flag > 2 ? menu2_flag = 0 : menu2_flag;
        ips200_clear();
    }
    else if(key == UP)
    {
        menu2_flag--;
        menu2_flag < 0 ? menu2_flag = 2 : menu2_flag;
        ips200_clear();
    }
    else if(key == PRESS)
    {
        // 确认选中并保存
        subject_id = menu2_flag + 1;
        ips200_show_string(140, 16*6, "OK!");
        system_delay_ms(300);
        ips200_clear();
    }
    else if(key == LEFT)
    {
        menu = MENU1_1;
        ips200_clear();
    }
}

void param_w_flash(uint8 page)
{
    // 参数含义: sector恒写0, 页码page, 数据源地址, 长度(单位是 uint32 即 float 的个数)
    flash_write_page(0, page, (const uint32 *)params, 40);
}


void param_r_flash(uint8 page)
{
    uint8 need_init = 0;

    // 1. 检查Flash对应页是否有数据 (返回1说明有数据)
    if (flash_check(0, page) == 1)
    {
        // 2. 有数据则读取出来
        flash_read_page(0, page, (uint32 *)params, 40);

        // 3. 检查数据是否合法 (防止擦除后未写入时读出全0xFF导致NaN)
        if (isnan(params[0]) || isnan(params[1]))
        {
            need_init = 1;
        }
    }
    else
    {
        // 返回0说明没有数据
        need_init = 1;
    }

    // 4. 如果需要初始化，根据传入的page，把原本在代码里的默认值重新打包写入Flash修复
    if (need_init)
    {
        memset(params, 0, 40 * sizeof(float));

        switch (page)
        {
            case SPEED_PAGE:
                params[0] = basic_speed;
                memcpy(&params[1], &pid_left, sizeof(IncPID));
                memcpy(&params[1 + sizeof(IncPID)/4], &pid_right, sizeof(IncPID));
                break;

            case DIR_PAGE:
                memcpy(params, &turnPID, sizeof(SteeringPID));
                break;

            case IMAGE_PAGE:
                // TODO: 如果后续有图像参数，在这里填补默认值打包逻辑
                break;
        }

        // 写回Flash进行修复初始化
        param_w_flash(page);
    }
}
