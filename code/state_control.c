/**************************************************************
 * 文件名:  state_control.c
 * 创建时间: 2026年2月24日 下午6:53:14
 * 描述:
 **************************************************************/

#include "element_line.h"
#include "state_control.h"
#include "line.h"
#include "element_line.h"
#include "zf_device_ips200.h"
#include "maze.h"
#include "MahonyAHRS.h"

state car_state=normal;

int fork_type[10] = {1};
int current_angle = 0;

void state_update(){
    switch(car_state){
        case normal:{
            if((((element_line_U > 0) + (element_line_L > 0) + (element_line_R > 0)) == 2)&&trend.slope>0.5){
                if(fork_type[fork_num]==0){
                    car_state = fork_straight;
                }else{
                    car_state = fork_turn;
                    current_angle = eulerAngle.yaw;
                }
            }
            break;
        }
        case fork_straight:{
            if(((element_line_U > 0) + (element_line_L > 0) + (element_line_R > 0)) < 2){
                car_state = normal;
                fork_num++;
            }
            break;
        }
        case fork_turn:{
            if(abs(eulerAngle.yaw-current_angle)>60){
                car_state = normal;
                fork_num++;
            }
            break;
        }

    }
    //ips200_show_int(0,120,((element_line_U > 0) + (element_line_L > 0) + (element_line_R > 0)),1);
}

void show_state(){
    switch(car_state){
        case normal:{
            ips200_show_string(0,140,"normal");
            break;
        }
        case fork_turn:{
            ips200_show_string(0,140,"fork_turn");
            break;
        }
        case fork_straight:{
            ips200_show_string(0,140,"fork_straight");
            break;
        }

    }

}
