#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_

#include "zf_device_mt9v03x_double.h"

extern uint8 mt9v03x_image_bin[MT9V03X_1_H][MT9V03X_1_W];

uint8 safe_access_binimg(int x, int y);
uint8 safe_access_img(int x, int y) ;
void binarization(uint8 thres);
uint8 otsuThreshold_fast(void);


#endif
