/**************************************************************
 * 文件名:  line.c
 * 创建时间: 2026年2月18日 下午3:57:37
 * 描述:
 **************************************************************/
#include "maze.h"
#include "image.h"
#include "stdio.h"
#include "line.h"
#include "tool.h"
#include "zf_device_ips200.h"

int error=0;

Point target_L,target_R,target_M;


//-------------------------------------------------------------------------------------------------------------------
//  函数简介     计算 某个点与底边中点的连线 与 中点法线的夹角
//  返回类型     int
//  参数说明        x，y该点坐标
//  使用样例        get_angle（M.x,M.y）;
//---------------------------------------------------------------------------------------------------------------
int get_angle(int x,int y){
    x = (MT9V03X_1_W/2)-x;
    y = MT9V03X_1_H - y;
    int result = (int)(atan2f(x, y) * 180.0f / M_PI);
    return result;
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      在屏幕上可视化显示检测到的边界线
//  返回类型     void（无返回值）
//  参数说明        l 左侧边界点数，r 右侧边界点数
//  使用样例        Show_Line(get_L_point(), get_R_point());
//---------------------------------------------------------------------------------------------------------------

void Show_Line(){

    ips200_show_int(200,30,L_point,3);
    ips200_show_int(200,50,R_point,3);
    for(int i=0;i<L_point;i++){
        show_point_red(Side_L[i].x,Side_L[i].y);
        //ips200_draw_point(Side_M[i].x,Side_M[i].y,RGB565_YELLOW);
    }

    for(int i=0;i<R_point;i++){
        show_point_blue(Side_R[i].x,Side_R[i].y);
    }

}


//-------------------------------------------------------------------------------------------------------------------
//  函数简介      计算一组边界点的平均坐标
//  返回类型     Side（包含平均x,y坐标的结构体）
//  参数说明        sidein 边界点数组指针
//              min 起始索引
//              max 结束索引（不包括）
//  使用样例        Side avg = average_point(Side_L, 10, 20);
//---------------------------------------------------------------------------------------------------------------
Point average_point(Point *Pointin,int min,int max){
    Point sum;
    sum.x = 0;
    sum.y = 0;
    int count = 0;

    for(int i=min;i<max;i++){
        sum.x += Pointin[i].x;
        sum.y += Pointin[i].y;
        count +=  1;
    }

    sum.x = sum.x/count;
    sum.y = sum.y/count;
    return sum;
}

void caculate_error(){
    int front_point = 75;
    int average_range = 5;

    Point R,L,M;

    if (L_point>front_point){
        L = average_point(Side_L,front_point-average_range,front_point);

    }else if(L_point>40){
        L = average_point(Side_L,L_point-average_range,L_point);
    }else if(L_point>20){
        L = average_point(Side_L,15,20);
    }else{
        L = Side_L[L_point-1];
    }
    if (R_point>front_point){
        R = average_point(Side_R,front_point-average_range,front_point);
    }else if(R_point>40){
        R = average_point(Side_R,R_point-average_range,R_point);
    }else if(R_point>20){
        R = average_point(Side_R,15,20);
    }else{
        R = Side_R[R_point-1];
    }

    M.x = (L.x+R.x)/2;
    M.y = (L.y+R.y)/2;
    int M_result = get_angle(M.x,M.y);
    int L_result = get_angle(L.x,L.y);
    int R_result = get_angle(R.x,R.y);

    /*
    if(current_state== open_run){
        error = 0;
    }else if(current_circle_state==R_run){
        error = R_result;
    }else if(current_circle_state==L_run){
        error = L_result;
    }else if(current_circle_state==L_out){
        error = R_result;
    }else if(current_circle_state==R_out){
        error = L_result;
    }else{
        error = M_result;
    }
    */

    error = M_result;
}

int return_error(){
    return error;
}
