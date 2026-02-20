/**************************************************************
 * 文件名:  tool.c
 * 创建时间: 2026年2月19日 下午9:55:57
 * 描述:
 **************************************************************/
#include "zf_common_headfile.h"

void show_point_red(int x,int y){
    if(x>0 && x<MT9V03X_1_W&&y>0&&y<MT9V03X_1_H){
        ips200_draw_point((uint16)x,(uint16)y,RGB565_RED);
    }
}

void show_point_blue(int x,int y){
    if(x>0 && x<MT9V03X_1_W&&y>0&&y<MT9V03X_1_H){
        ips200_draw_point((uint16)x,(uint16)y,RGB565_BLUE);
    }
}

void show_point_yellow(int x,int y){
    if(x>0 && x<MT9V03X_1_W&&y>0&&y<MT9V03X_1_H){
        ips200_draw_point((uint16)x,(uint16)y,RGB565_YELLOW);
    }
}
