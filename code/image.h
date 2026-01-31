#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_

uint8_t safe_access_binimg(int x, int y);
uint8 safe_access_img(int x, int y) ;
void binarization(int thres);
uint8 otsuThreshold_optimized(uint8 step);

#endif /* CODE_IMAGE_H_ */
