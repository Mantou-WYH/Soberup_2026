#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_

#include "zf_device_mt9v03x.h"

extern uint8 mt9v03x_image_bin[MT9V03X_H][MT9V03X_W];

uint8_t safe_access_binimg(int x, int y);
uint8 safe_access_img(int x, int y) ;
void binarization(int thres);
uint8 otsuThreshold_optimized(uint8 step);


#endif
