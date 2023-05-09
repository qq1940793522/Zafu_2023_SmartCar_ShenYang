#include "zf_common_headfile.h"
#include "image.h"

uint8 THRESHOLD;
uint8 image_new_gray_bin[ROW_H][COL_W];  //188*120抽取成94*60
uint8 image_new_bin[ROW_H][COL_W];   //二值化图像数组
int otsu (unsigned char *image,  int rows, int cols);
unsigned char Half_Road_Width57,Old_HRWid;

void Image_Get(void)
{
  for(int i = 0; i < ROW_H; i++)
  {
    for(int j = 0; j < COL_W-0; j++)
    {
        image_new_gray_bin[i][j] = mt9v03x_image[i][j-0];
    }
    for(int j = 0; j < 0; j++)
    {
        image_new_gray_bin[i][j] = 0;
    }
    for(int j = COL_W-0; j < COL_W; j++)
    {
        image_new_gray_bin[i][j] = 0;
    }
  }
}
