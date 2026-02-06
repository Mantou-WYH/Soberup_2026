/**************************************************************
 * 文件名:  image.c
 * 创建时间: 2026年1月30日 下午8:26:19
 * 描述: 初步图像处理
 **************************************************************/
#include "zf_device_mt9v03x.h"
#include "stdio.h"
#include "stdint.h"

IFX_ALIGN(4) uint8      mt9v03x_image_bin[MT9V03X_H][MT9V03X_W];

uint8_t safe_access_binimg(int x, int y) {
    if (x >= 0 && x < MT9V03X_W && y >= 0 && y < MT9V03X_H) {
        return mt9v03x_image_bin[y][x];
    } else {
        return 0;
    }
}

uint8 safe_access_img(int x, int y) {
    if (x >= 0 && x < MT9V03X_W && y >= 0 && y < MT9V03X_H) {
        return mt9v03x_image[y][x];
    } else {
        return 0;
    }
}

void binarization(int thres){
  for(int y=0; y<MT9V03X_H; y++){
    for(int x=0; x<MT9V03X_W; x++){
        mt9v03x_image_bin[y][x] = safe_access_binimg(x,y)>thres ? 255 : 0;
    }
  }
}

/**
 * @brief 使用安全访问函数且优化的Otsu阈值计算
 * @param step 采样步长，默认2（隔点采样）。增大可提升速度，但会降低精度
 * @return uint8_t 计算得到的最佳阈值
 */
uint8 otsuThreshold_fast()   // 注意计算阈值的一定要是原图像
{
    #define GrayScale 256

    int Pixel_Max = 0;
    int Pixel_Min = 255;
    uint16 width = MT9V03X_W;
    uint16 height = MT9V03X_H;
    int pixelCount[GrayScale];  // 各像素GrayScale的个数pixelCount 一维数组
    float pixelPro[GrayScale];  // 各像素GrayScale所占百分比pixelPro 一维数组
    int i, j, pixelSum = width * height / 4;  // pixelSum是获取总的图像像素个数的1/4，相应下面轮询时高和宽都是以2为单位自增
    uint8 threshold = 0;

    // 清零
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum = 0;  // 每次执行到这会将gray_sum清零
    // 统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i += 2)   // 高
    {
        for (j = 0; j < width; j += 2)    // 宽
        {
            // 使用 safe_access_img 函数来安全访问图像数据
            uint8 pixel_value = safe_access_binimg(j, i);
            pixelCount[pixel_value]++;
            gray_sum += pixel_value;
            if (pixel_value > Pixel_Max)
                Pixel_Max = pixel_value;
            if (pixel_value < Pixel_Min)
                Pixel_Min = pixel_value;
        }
    }

    // 计算每个像素值的点在整幅图像中的比例
    for (i = Pixel_Min; i < Pixel_Max; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }

    // 遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;

    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {

        w0 += pixelPro[j];  // 背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  // 背景部分 每个灰度值的点的比例 *灰度值

        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;

        u0 = u0tmp / w0;              // 背景平均灰度
        u1 = u1tmp / w1;              // 前景平均灰度
        u = u0tmp + u1tmp;            // 全局平均灰度
        deltaTmp = (float)(w0 * w1 * (u0 - u1) * (u0 - u1));
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = (uint8)j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
    }

    if (threshold < 90)
    {
        threshold = 90;
    }
    if (threshold > 130)
    {
        threshold = 130;
    }

    return threshold;
}

