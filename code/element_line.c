/**************************************************************
 * 文件名:  element_line.c
 * 创建时间: 2026年2月20日 下午7:41:59
 * 描述:
 **************************************************************/
#include "image.h"
#include "tool.h"
#include "maze.h"
#include "zf_device_ips200.h"

int element_line_L=0,element_line_R=0,element_line_U=0,element_line_B=0;

Point L_edge[10],R_edge[10],U_edge[10],B_edge[10],M_edge[10];


//-------------------------------------------------------------------------------------------------------------------
//  函数简介      在指定高度（Y坐标）的水平方向检测跳变点（黑到白）
//  返回类型     uint8_t（检测到的跳变点数量或最大值）
//  参数说明        img 图像数据
//              hight 检测的Y坐标高度
//              max 当前最大值（用于比较）
//              *edge 存储检测到的边界点
//  使用样例        line_W(img, 50, 0, U_edge);
//---------------------------------------------------------------------------------------------------------------

uint8_t line_W(int hight,int max,Point *edge){
    int count=0;
    for(int i=20;i<MT9V03X_1_W-20;i++){
        if(safe_access_binimg(i-2,hight)==0){
            if(safe_access_binimg(i-1,hight)==0){
                if(safe_access_binimg(i,hight)==255){
                    if(safe_access_binimg(i+1,hight)==255){
                        if(safe_access_binimg(i+2,hight)==255){
                            count++;
                        }
                    }
                }
            }
        }
    }

    if(count>max){
        int count_point = 0;
        for(int i=20;i<MT9V03X_1_W-20;i++){
            if(safe_access_binimg(i-2,hight)==0){
                if(safe_access_binimg(i-1,hight)==0){
                    if(safe_access_binimg(i,hight)==255){
                        if(safe_access_binimg(i+1,hight)==255){
                            if(safe_access_binimg(i+2,hight)==255){
                                edge[count_point].x = i;
                                edge[count_point].y = hight;
                                count_point++;
                            }
                        }
                    }
                }
            }
        }
    }
    return count>max?count:max;
}
//-------------------------------------------------------------------------------------------------------------------
//  函数简介      在指定X坐标的垂直方向检测跳变点（黑到白）
//  返回类型     uint8_t（检测到的跳变点数量）
//  参数说明        img 图像数据
//              x 检测的X坐标位置
//  使用样例        int count = line_H(img_data, 100);
//---------------------------------------------------------------------------------------------------------------
uint8_t line_H(int x,int max,Point *edge){
    int count=0;
    for(int i=30+2;i<MT9V03X_1_H-1;i++){
        if(safe_access_binimg(x,i-2)==0){
            if(safe_access_binimg(x,i-1)==0){
                if(safe_access_binimg(x,i)==255){
                    if(safe_access_binimg(x,i+1)==255){
                        if(safe_access_binimg(x,i+2)==255){
                            count++;
                        }
                    }
                }
            }
        }
    }

    if(count>max){
        int count_point = 0;
        for(int i=30+2;i<MT9V03X_1_H-1;i++){
            if(safe_access_binimg(x,i-2)==0){
                if(safe_access_binimg(x,i-1)==0){
                    if(safe_access_binimg(x,i)==255){
                        if(safe_access_binimg(x,i+1)==255){
                            if(safe_access_binimg(x,i+2)==255){
                                edge[count_point].x = x;
                                edge[count_point].y = i;
                                count_point++;
                            }
                        }
                    }
                }
            }
        }
    }



    return count>max?count:max;
}

int element_L(){
    int count = 0;
    for(int i=0;i<=5;i++){
        count = line_H(20+i*3,count,L_edge);
    }
    return count;
}

int element_R(){
    int count = 0;
    for(int i=0;i<=5;i++){
        count = line_H(MT9V03X_1_W-20-i*3,count,R_edge);
    }
    return count;
}

int element_U(){
    int count = 0;
    for(int i=0;i<=5;i++){
        count = line_W(20+i*5,count,U_edge);
    }
    return count;
}

int element_B(){
    int count = 0;
    for(int i=0;i<=5;i++){
        count = line_W(MT9V03X_1_H-40-i*4,count,B_edge);
    }
    return count;
}

void show_element_line(){
    //ips200_show_int(0,140,element_line_M,2);
    ips200_show_int(0,160,element_line_L,2);
    ips200_show_int(0,180,element_line_R,2);
    ips200_show_int(0,200,element_line_U,2);
    ips200_show_int(0,220,element_line_B,2);
}

void element_update(){
    element_line_L = element_L();
    element_line_R = element_R();
    element_line_U = element_U();
    element_line_B = element_B();
    //show_element_line();
    //show_element_first_point();
    //show_circie_line();
}
