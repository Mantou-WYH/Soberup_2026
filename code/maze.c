/**************************************************************
 * 文件名:  maze.c
 * 创建时间: 2026年1月31日 下午7:15:54
 * 描述:
 **************************************************************/
#include "stdio.h"
#include "image.h"
#include "zf_device_mt9v03x_double.h"
#include "string.h"
#include "maze.h"
#include "line.h"
#include "zf_device_ips200.h"
#include "motor.h"

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
uint8_t L_visited[MT9V03X_1_H][MT9V03X_1_W],R_visited[MT9V03X_1_H][MT9V03X_1_W];
int L_point,R_point;
uint8_t touch = 0;
DistTrend trend;
int count_outside = 0;




void findline_twoside(Point L, Point R, int *Lnum, int *Rnum, int MAX_step) {
    Max_Lx.x = MT9V03X_1_W - 1;
    Max_Rx.x = 0;
    Max_Ly.y = MT9V03X_1_H - 1;
    Max_Ry.y = MT9V03X_1_H - 1;

    maze L_maze = {0}, R_maze = {0};
    L_maze.step = 0;
    R_maze.step = 0;


    memset(L_visited, 0, sizeof(L_visited));
    memset(R_visited, 0, sizeof(R_visited));
    L_visited[L.y][L.x] = 1;
    R_visited[R.y][R.x] = 1;

    if (safe_access_binimg(L.x, L.y) == 0 || safe_access_binimg(R.x, R.y) == 0) {
        return;
    }

    while (L_maze.step + *Lnum < MAX_step &&
           R_maze.step + *Rnum < MAX_step &&
           L.y >= SAFE_MARGIN_Y && L.y < MT9V03X_1_H &&
           R.y >= SAFE_MARGIN_Y && R.y < MT9V03X_1_H &&
           L.x > 0 && L.x < MT9V03X_1_W - 1 &&
           R.x > 0 && R.x < MT9V03X_1_W - 1 &&
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
        if (R_visited[L.y][L.x] == 1) {
            touch = 1;
            ips200_show_int(0,120,1,1);
            break;
        } else {
            L_visited[L.y][L.x] = 1;
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
        if (L_visited[R.y][R.x] == 1) {
            touch = 1;
            break;
        } else {
            R_visited[R.y][R.x] = 1;
        }
    }

    *Lnum += L_maze.step;
    *Rnum += R_maze.step;
}

uint8_t First_Line_M(uint8_t n,uint8_t y){ //最长白列为起始点
    int sum = 0;
    int sum_max = 0;
    StarM.x = MT9V03X_1_W / 2;  // 默认取图像中心x坐标
    StarM.y = y;   // 默认取最底部y坐标

    for(int i=0;i<MT9V03X_1_W-1;i++){
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





    for(int i=StarM.x;i>2;i--){
        if(safe_access_binimg(i-1,StarM.y)==0){
            if(safe_access_binimg(i-2,StarM.y)==0){
                Side_L[n].x = i;
                break;
            }
        }
    }

    for(int i=StarM.x;i<MT9V03X_1_W-2;i++){
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
    if (y < 0 || y >= MT9V03X_1_H) {
        return -1;
    }

    int found_L = 0, found_R = 0;

    // ====== 寻找左侧 L：从左到右找 [黑, 黑, 白, 白] ======
    for (int x = 2; x < MT9V03X_1_W - 2; x++) {
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
    for (int x = MT9V03X_1_W - 3; x >= 2; x--) {
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

/**
 * @brief  在 Side_L / Side_R 中，按索引逐点配对，
 *         计算每对点的水平距离，再做最小二乘线性拟合。
 *
 * @param  Lnum     Side_L 有效点数（即 L_point）
 * @param  Rnum     Side_R 有效点数（即 R_point）
 * @param  result   输出：趋势结果结构体指针
 * @return  0  成功
 *         -1  有效点对不足，无法计算
 */
int calc_side_distance_trend(int Lnum, int Rnum, DistTrend *result)
{
    if (!result) return -1;

    /* ---------- 初始化输出 ---------- */
    result->slope       = 0.0f;
    result->avg_dist    = 0.0f;
    result->first_dist  = 0.0f;
    result->last_dist   = 0.0f;
    result->valid_pairs = 0;

    if (Lnum <= 0 || Rnum <= 0) return -1;

    /* ---------- Step1: 逐点配对，计算水平距离 ---------- */
    float dist_buf[MAX_MATCH_POINTS];
    int   pair_count = 0;
    float sum_dist   = 0.0f;

    int max_i = (Lnum < Rnum) ? Lnum : Rnum;
    if (max_i > MAX_MATCH_POINTS) max_i = MAX_MATCH_POINTS;

    int start_i = max_i - 30;
    if (start_i < 0) start_i = 0;

    for (int i = start_i; i < max_i; i++)
    {
        int lx = Side_L[i].x;
        int rx = Side_R[i].x;
        int ly = Side_L[i].y;
        int ry = Side_R[i].y;

        float dx = (float)(rx - lx);
        float dy = (float)(ry - ly);
        float dist = dx * dx + dy * dy;  /* 欧氏距离 */

        dist_buf[pair_count] = dist;
        sum_dist += dist;
        pair_count++;
    }

    if (pair_count < MIN_VALID_PAIRS) return -1;

    /* ---------- Step2: 最小二乘线性拟合 ---------- */
    /*
     * x = 点对索引 i（0, 1, 2, …, n-1）
     * y = 距离 dist
     * b = (n·Σ(xi·yi) - Σxi·Σyi) / (n·Σ(xi²) - (Σxi)²)
     */
    float n      = (float)pair_count;
    float sum_x  = 0.0f;
    float sum_y  = 0.0f;
    float sum_xx = 0.0f;
    float sum_xy = 0.0f;

    for (int i = 0; i < pair_count; i++)
    {
        float xi = (float)i;
        float yi = dist_buf[i];
        sum_x  += xi;
        sum_y  += yi;
        sum_xx += xi * xi;
        sum_xy += xi * yi;
    }

    float denom = n * sum_xx - sum_x * sum_x;
    float slope = 0.0f;
    if (denom > 1e-6f || denom < -1e-6f)
    {
        slope = (n * sum_xy - sum_x * sum_y) / denom;
    }

    /* ---------- Step3: 填充结果 ---------- */
    result->slope       = slope;
    result->avg_dist    = sum_dist / n;
    result->first_dist  = dist_buf[0];
    result->last_dist   = dist_buf[pair_count - 1];
    result->valid_pairs = pair_count;

    return 0;
}

void outside_protect(void){
    if(L_point<20&&R_point<20){
        if(count_outside<50){
            car_stop();
            negative_pressure_stop();
        }else{
            count_outside++;
        }
    }
}
/*************************
 *

******************************/
void search_line_main(void) {
    uint8_t y = MT9V03X_1_H - 1;  // 初始扫描行号，默认为图像最底部
    int MAX_step = 200;        // 最大搜索步数

    int Lnum = 0, Rnum = 0;

    uint8_t start_found = First_Line_M(0, y);
    if (!start_found) {
        for(int i=MT9V03X_1_H-2;i>MT9V03X_1_H-22;i=i-2){
            if(start_found==0){
                start_found = First_Line_M(0,i);
            }else{
                break;
            }
        }
    }


    // 2. 从起始点开始，一次性搜索完整边界
    touch = 0;  // 重置标志
    findline_twoside(Side_L[0], Side_R[0], &Lnum, &Rnum, MAX_step);

    int search_attempts = 0;  // 防止无限循环
    const int MAX_ATTEMPTS = 5;  // 最大尝试次数

    while (touch == 1 && search_attempts < MAX_ATTEMPTS) {
            // 选择更靠上的点（y值更小）
            Point start_point = Max_Ly.y < Max_Ry.y ? Max_Ly : Max_Ry;

            // 向上3行寻找新起点
            int new_row = start_point.y - 3;
            if (new_row < SAFE_MARGIN_Y) {
                break;  // 防止越界
            }

            Point new_L, new_R;
            touch = 0;  // 重置touch标志

            if (find_line_edges_at_row(new_row, &new_L, &new_R) == 0) {
                // 找到新边界点，继续搜索
                touch = 0;
                findline_twoside(new_L, new_R, &Lnum, &Rnum, MAX_step - Lnum);
            } else {
                // 未找到新边界点，结束搜索
                break;
            }
        }
    L_point = Lnum;
    R_point = Rnum;

    calc_side_distance_trend(L_point, R_point, &trend);
    outside_protect();
    show_line();
}


