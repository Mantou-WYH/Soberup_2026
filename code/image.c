/**************************************************************
 * 文件名:  image.c
 * 创建时间: 2026年1月30日 下午8:26:19
 * 描述: 初步图像处理
 **************************************************************/
#include "zf_common_headfile.h"

IFX_ALIGN(4) uint8      mt9v03x_image_bin[MT9V03X_H][MT9V03X_W];

// 安全访问图像像素，越界时返回0
inline uint8_t safe_access_binimg(int x, int y) {
    if (x >= 0 && x < MT9V03X_W && y >= 0 && y < MT9V03X_H) {
        return mt9v03x_image_bin[y][x];
    } else {
        return 0;
    }
}

inline uint8_t safe_access_img(int x, int y) {
    if (x >= 0 && x < MT9V03X_W && y >= 0 && y < MT9V03X_H) {
        return mt9v03x_image[y][x];
    } else {
        return 0;
    }
}

void binarization(int thres){
  for(int y=0; y<MT9V03X_H; y++){
    for(int x=0; x<MT9V03X_W; x++){
        mt9v03x_image_bin[y][x] = safe_access_img(x,y)>thres ? 255 : 0;
    }
  }
}

/**
 * @brief 使用安全访问函数且优化的Otsu阈值计算
 * @param step 采样步长，默认2（隔点采样）。增大可提升速度，但会降低精度
 * @return uint8_t 计算得到的最佳阈值
 */
uint8_t otsuThreshold_optimized(uint8_t step = 2)
{
    const int GrayScale = 256;
    uint32_t pixelCount[GrayScale] = {0}; // 灰度直方图
    uint32_t gray_sum = 0;
    uint8_t min_gray = 255;
    uint8_t max_gray = 0;

    // 1. 统计灰度直方图（使用安全访问并下采样）
    for (int y = 0; y < MT9V03X_H; y += step) {
        for (int x = 0; x < MT9V03X_W; x += step) {
            uint8_t pixel_value = safe_access_img(x, y);
            pixelCount[pixel_value]++;
            gray_sum += pixel_value;
            if (pixel_value < min_gray) min_gray = pixel_value;
            if (pixel_value > max_gray) max_gray = pixel_value;
        }
    }

    uint32_t total_pixels = (MT9V03X_H * MT9V03X_W) / (step * step);
    uint32_t total_sum = gray_sum;

    uint64_t best_sigma = 0; // 类间方差
    uint8_t best_thresh = (min_gray + max_gray) / 2; // 初始阈值取中值

    uint32_t w0 = 0;   // 背景像素数累积
    uint32_t sum0 = 0; // 背景灰度值累积

    // 2. 遍历实际灰度范围，寻找最佳阈值
    for (uint16_t t = min_gray; t <= max_gray; t++) {
        w0 += pixelCount[t];
        if (w0 == 0) continue; // 避免除零

        sum0 += (uint32_t)t * pixelCount[t];

        uint32_t w1 = total_pixels - w0; // 前景像素数
        if (w1 == 0) break; // 所有像素已归为背景，终止循环

        uint32_t sum1 = total_sum - sum0; // 前景灰度值总和

        // 3. 整数计算类间方差（放大10000倍保持精度）
        uint32_t u0 = (sum0 * 10000) / w0; // 背景平均灰度（放大后）
        uint32_t u1 = (sum1 * 10000) / w1; // 前景平均灰度（放大后）

        int32_t diff = (int32_t)u0 - (int32_t)u1;
        uint64_t sigma = (uint64_t)w0 * w1 * diff * diff;

        // 4. 更新最佳阈值
        if (sigma > best_sigma) {
            best_sigma = sigma;
            best_thresh = t;
        }
    }

    return best_thresh;
}
