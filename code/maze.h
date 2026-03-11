#ifndef CODE_MAZE_H_
#define CODE_MAZE_H_

#define max(a,b) a>b?a:b
#define min(a,b) a>b?b:a
#define SAFE_MARGIN_Y 20

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    float   slope;          /* 线性拟合斜率（像素/步）  */
    float   avg_dist;       /* 平均距离（像素）          */
    float   first_dist;     /* 起点距离（像素）          */
    float   last_dist;      /* 终点距离（像素）          */
    int     valid_pairs;    /* 有效配对点数量            */
} DistTrend;

typedef struct {
    int fx;
    int fy;
    int fsx;
    int fsy;
    int step;
    int dir;
    int turn;
}maze;

#define MAX_MATCH_POINTS  200    /* 与 MAX_step 保持一致     */
#define MIN_VALID_PAIRS     3    /* 有效配对点最少数量        */


extern Point Side_L[200],Side_R[200],StarM;
extern int L_point,R_point;
extern Point Max_Lx,Max_Ly,Max_Rx,Max_Ry;
extern DistTrend trend;

void search_line_main(void);

#endif /* CODE_MAZE_H_ */
