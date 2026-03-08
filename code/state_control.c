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

state car_state=normal;

void state_update(){
    switch(car_state){
        case normal:{
            if(((element_line_U > 0) + (element_line_L > 0) + (element_line_R > 0)) == 2){
                car_state = fork;
            }
            break;
        }
        case fork:{
            if(((element_line_U > 0) + (element_line_L > 0) + (element_line_R > 0)) != 2){
                car_state = normal;
                fork_num++;
            }
            break;
        }

    }
}

void show_state(){
    switch(car_state){
        case normal:{
            ips200_show_string(0,140,"normal");
            break;
        }
        case fork:{
            ips200_show_string(0,140,"fork");
            break;
        }

    }

}
