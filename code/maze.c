/**************************************************************
 * 文件名:  maze.c
 * 创建时间: 2026年1月31日 下午7:15:54
 * 描述:
 **************************************************************/
#include "stdio.h"
#include "image.h"
#include "zf_device_mt9v03x.h"
#include "string.h"
#include "maze.h"

const int dir_front[4][2]= {{0,  -1},
                            {1,  0},
                            {0,  1},
                            {-1, 0}};

const int dir_frontleft[4][2] = {{-1, -1},
                                 {1,  -1},
                                 {1,  1},
                                 {-1, 1}};

const int dir_frontright[4][2] ={{1,  -1},
                                 {1,  1},
                                 {-1, 1},
                                 {-1, -1}};


Point Side_L[200],Side_R[200],StarM;
Point Max_Lx,Max_Ly,Max_Rx,Max_Ry;
uint8_t visited[MT9V03X_H][MT9V03X_W];
int L_point,R_point;

typedef struct {
    int fx;
    int fy;
    int fsx;
    int fsy;
    int step;
    int dir;
    int turn;
}maze;

void findline_twoside(Point L, Point R, int *Lnum, int *Rnum, int MAX_step) {
    Max_Lx.x = MT9V03X_W - 1;
    Max_Rx.x = 0;
    Max_Ly.y = MT9V03X_H - 1;
    Max_Ry.y = MT9V03X_H - 1;

    maze L_maze = {0}, R_maze = {0};
    L_maze.step = 0;
    R_maze.step = 0;
    uint8_t L_stop = 0;
    uint8_t R_stop = 0;
    uint8_t touch = 0;

    memset(visited, 0, sizeof(visited));
    visited[L.y][L.x] = 1;
    visited[R.y][R.x] = 1;

    if (safe_access_binimg(L.x, L.y) == 0 || safe_access_binimg(R.x, R.y) == 0) {
        return;
    }

    while (L_maze.step + *Lnum < MAX_step &&
           R_maze.step + *Rnum < MAX_step &&
           L.y >= SAFE_MARGIN_Y && L.y < MT9V03X_H &&
           R.y >= SAFE_MARGIN_Y && R.y < MT9V03X_H &&
           L.x > 0 && L.x < MT9V03X_W - 1 &&
           R.x > 0 && R.x < MT9V03X_W - 1 &&
           L_maze.turn < 4 && R_maze.turn < 4) {

        // ========== 左手移动 ==========
        int front_Lx = L.x + dir_front[L_maze.dir][0];
        int front_Ly = L.y + dir_front[L_maze.dir][1];
        int frontleft_x = L.x + dir_frontleft[L_maze.dir][0];
        int frontleft_y = L.y + dir_frontleft[L_maze.dir][1];

        int front_L_value = safe_access_binimg(front_Lx, front_Ly);
        int frontleft_value = safe_access_binimg(frontleft_x, frontleft_y);

        if (front_L_value == 0) {
            L_maze.dir = (L_maze.dir + 1) % 4;
            L_maze.turn++;
        } else if (frontleft_value == 0) {
            L.x += dir_front[L_maze.dir][0];
            L.y += dir_front[L_maze.dir][1];
            Side_L[L_maze.step + *Lnum].x = L.x;
            Side_L[L_maze.step + *Lnum].y = L.y;
            L_maze.step++;
            L_maze.turn = 0;
        } else {
            L.x += dir_frontleft[L_maze.dir][0];
            L.y += dir_frontleft[L_maze.dir][1];
            L_maze.dir = (L_maze.dir + 3) % 4;
            L_maze.step++;
            Side_L[L_maze.step + *Lnum].x = L.x;
            Side_L[L_maze.step + *Lnum].y = L.y;
            L_maze.turn = 0;
        }
        if (Max_Lx.x > L.x) {
            Max_Lx.x = L.x;
            Max_Lx.y = L.y;
        }
        if (Max_Ly.y > L.y) {
            Max_Ly.y = L.y;
            Max_Ly.x = L.x;
        }
        if (visited[L.y][L.x] == 1) {
            if (touch == 1) {
                touch = 0;
                break;
            } else {
                touch++;
            }
        } else {
            visited[L.y][L.x] = 1;
        }

        // ========== 右手移动 ==========
        int front_x = R.x + dir_front[R_maze.dir][0];
        int front_y = R.y + dir_front[R_maze.dir][1];
        int frontright_x = R.x + dir_frontright[R_maze.dir][0];
        int frontright_y = R.y + dir_frontright[R_maze.dir][1];

        int front_value = safe_access_binimg(front_x, front_y);
        int frontright_value = safe_access_binimg(frontright_x, frontright_y);

        if (front_value == 0) {
            R_maze.dir = (R_maze.dir + 3) % 4;
            R_maze.turn++;
        } else if (frontright_value == 0) {
            R.x += dir_front[R_maze.dir][0];
            R.y += dir_front[R_maze.dir][1];
            Side_R[R_maze.step + *Rnum].x = R.x;
            Side_R[R_maze.step + *Rnum].y = R.y;
            R_maze.step++;
            R_maze.turn = 0;
        } else {
            R.x += dir_frontright[R_maze.dir][0];
            R.y += dir_frontright[R_maze.dir][1];
            R_maze.dir = (R_maze.dir + 1) % 4;
            R_maze.step++;
            Side_R[R_maze.step + *Rnum].x = R.x;
            Side_R[R_maze.step + *Rnum].y = R.y;
            R_maze.turn = 0;
        }
        if (Max_Rx.x < R.x) {
            Max_Rx.x = R.x;
            Max_Rx.y = R.y;
        }
        if (Max_Ry.y > R.y) {
            Max_Ry.y = R.y;
            Max_Ry.x = R.x;
        }
        if (visited[R.y][R.x] == 1) {
            if (touch == 1) {
                touch = 0;
                break;
            } else {
                touch++;
            }
        } else {
            visited[R.y][R.x] = 1;
        }
    }

    *Lnum += L_maze.step;
    *Rnum += R_maze.step;
}

uint8_t First_Line_M(int n,int y){ //最长白列为起始点
    int sum = 0;
    int sum_max = 0;
    StarM.x = MT9V03X_W / 2;  // 默认取图像中心x坐标
    StarM.y = y;   // 默认取最底部y坐标

    for(int i=0;i<MT9V03X_W-1;i++){
        for(int j=y;j>0;j--){
            if(safe_access_binimg(i, j) == 0) break;
            sum++;

        }
        if(sum>sum_max){
            sum_max = sum;
            StarM.x = i;
            StarM.y = y;

        }
        sum = 0;
    }
    Side_L[n].x = StarM.x;
    Side_L[n].y = StarM.y;
    Side_R[n].x = StarM.x;
    Side_R[n].y = StarM.y;
    //ips200_show_int(0,120,sum_max,3);
    if(sum_max<1) return 0;





    for(uint16_t i=StarM.x;i>2;i--){
        if(safe_access_binimg(i-1,StarM.y)==0){
            if(safe_access_binimg(i-2,StarM.y)==0){
                Side_L[n].x = i;
                break;
            }
        }
    }

    for(uint16_t i=StarM.x;i<MT9V03X_W-2;i++){
        if(safe_access_binimg(i+1,StarM.y)==0){
            if(safe_access_binimg(i+2,StarM.y)==0){
                Side_R[n].x = i;
                break;
            }
        }
    }
    return 1;

}

/**
 * 在指定行 y 上，寻找左右边界点 L 和 R
 * 要求：
 *   - L：第一个“黑→白”跳变，满足 [黑, 黑, 白, 白]
 *   - R：第一个“白→黑”跳变，满足 [白, 白, 黑, 黑]
 *
 * @param y: 扫描的行号
 * @param L: 输出左侧边界点（黑→白）
 * @param R: 输出右侧边界点（白→黑）
 * @return: 0 = 成功；-1 = 未找到有效边界
 */
int find_line_edges_at_row(int y, Point *L, Point *R) {
    if (y < 0 || y >= MT9V03X_H) {
        return -1;
    }

    int found_L = 0, found_R = 0;

    // ====== 寻找左侧 L：从左到右找 [黑, 黑, 白, 白] ======
    for (int x = 2; x < MT9V03X_W - 2; x++) {
        uint8_t p0 = safe_access_binimg(x - 2, y); // 黑
        uint8_t p1 = safe_access_binimg(x - 1, y); // 黑
        uint8_t p2 = safe_access_binimg(x,     y); // 白
        uint8_t p3 = safe_access_binimg(x + 1, y); // 白

        if (p0 == 0 && p1 == 0 && p2 != 0 && p3 != 0) {
            L->x = x;      // 可选：也可设为 x-1 或 (x-1+x)/2
            L->y = y;
            found_L = 1;
            break;
        }
    }

    // ====== 寻找右侧 R：从右到左找 [白, 白, 黑, 黑] ======
    for (int x = MT9V03X_W - 3; x >= 2; x--) {
        uint8_t p0 = safe_access_binimg(x - 1, y); // 白
        uint8_t p1 = safe_access_binimg(x,     y); // 白
        uint8_t p2 = safe_access_binimg(x + 1, y); // 黑
        uint8_t p3 = safe_access_binimg(x + 2, y); // 黑

        if (p0 != 0 && p1 != 0 && p2 == 0 && p3 == 0) {
            R->x = x;      // 可选：也可设为 x+1
            R->y = y;
            found_R = 1;
            break;
        }
    }

    // 确保 L 在 R 左侧，且都找到
    if (found_L && found_R && L->x < R->x) {
        return 0;
    } else {
        return -1;
    }
}

