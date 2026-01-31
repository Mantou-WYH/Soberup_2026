#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_

uint8_t safe_access_binimg(int x, int y);
uint8_t safe_access_img(int x, int y) ;
void binarization(int thres);
uint8_t otsuThreshold_optimized(uint8_t step);

#endif /* CODE_IMAGE_H_ */
