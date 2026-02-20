#ifndef CODE_MAZE_H_
#define CODE_MAZE_H_

#define max(a,b) a>b?a:b
#define min(a,b) a>b?b:a
#define SAFE_MARGIN_Y 20

typedef struct {
    int x;
    int y;
} Point;

extern Point Side_L[200],Side_R[200],StarM;
extern int L_point,R_point;
extern Point Max_Lx,Max_Ly,Max_Rx,Max_Ry;


#endif /* CODE_MAZE_H_ */
