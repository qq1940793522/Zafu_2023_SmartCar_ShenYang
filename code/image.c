#include "zf_common_headfile.h"
#include "image.h"

uint8 image_new_gray_bin[ROW_H][COL_W];  //188*120抽取成94*60
uint8 image_new_bin[ROW_H][COL_W];   //二值化图像数组
//int otsu (unsigned char *image,  int rows, int cols);
unsigned char Half_Road_Width57,Old_HRWid;
//动态阈值限幅
#define  yuzhi_max 250
#define  yuzhi_min 5
uint8 THRESHOLD=0;
uint8 Old_THRESHOLD=0;

uint8 YZ_delta=15;

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


/*******************************************************************************
* Function Name: yuzhiget(void);
* Description  : 最佳迭代法获取二值化阈值
*******************************************************************************/
void yuzhiget(void)
{
    static char ERROR = 2;       //合适阈值应该有的差值
    uint16 min=256,max=0, i = 0,j = 0,N0 = 0,N1 = 0,flag = 0;
    float T0,T1,T2,T_center;
    uint32 S0 = 0,S1 = 0;

    Old_THRESHOLD = THRESHOLD;

    for(i=20;i<50;i++)
    {
        for(j=12;j<COL_W-12;j++)
        {
                if(image_new_gray_bin[i][j] < min)
                {
                        min= image_new_gray_bin[i][j];
                }
                else if(image_new_gray_bin[i][j] > max)               //获得max，min值
                {
                        max= image_new_gray_bin[i][j];
                }
        }
    }
    T2 = (max+min)/2;
    do{
        for(i=20;i<50;i++)
        {
            for(j=12;j<COL_W-12;j++)
            {
                if( image_new_gray_bin[i][j] < T2)
                {
                    S0 += image_new_gray_bin[i][j];
                    N0++;
                }
                else
                {
                    S1 += image_new_gray_bin[i][j];
                    N1++;
                }
            }
        }
        T0 = ( S0)/N0;
        T1 = ( S1)/N1;
        T_center = (T0+T1)/2;
        if(T2 < T_center)
        {
            if((T_center - T2)> ERROR)
            {
                flag = 1;
            }
            else
            {
                flag = 0;
            }
        }
        else
        {
            if((T2 - T_center) > ERROR)
            {
                flag = 1;
            }
            else
            {
                flag = 0;
            }
        }
        T2 =T_center;
        THRESHOLD =(int) T2;
        if(THRESHOLD>yuzhi_max || THRESHOLD<yuzhi_min)
       {
           flag=0;
       }
    }
    while(flag);

}



/*******************************************************************************
* Function Name: Image_binaryzation(void);
* Description  : 二值化执行函数
*******************************************************************************/
void Image_binaryzation(void)                       //待优化阈值限幅6.12
{
  int i,j;

    if(THRESHOLD > YZ_delta + Old_THRESHOLD){
        THRESHOLD = Old_THRESHOLD+ YZ_delta;
    }
    else if(THRESHOLD  < Old_THRESHOLD -YZ_delta){
        THRESHOLD = Old_THRESHOLD- YZ_delta;
    }

  if(THRESHOLD>yuzhi_max)
      THRESHOLD= yuzhi_max;
  else if(THRESHOLD<yuzhi_min)
      THRESHOLD= yuzhi_min;



// 不去噪声 直接处理
  for( i=0;i<ROW_H;i++)
  {
    for( j=0;j<COL_W;j++)
    {
      if(image_new_gray_bin[i][j]<THRESHOLD )
        image_new_bin[i][j]= Black_Point;
      else
        image_new_bin[i][j]= White_Point;
    }
  }

}
