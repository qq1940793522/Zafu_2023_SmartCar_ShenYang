#include "zf_common_headfile.h"
#include "image.h"
#include "Car_Control.h"


//丢线点
uint8 Loss_L_Point=0,Loss_R_Point=0;
//丢线后第一个不丢点
uint8 Have_L_Point=0,Have_R_Point=0;

//后期临时补充
uint8 Y_point=0;
uint8 X_point=0;
uint8 X_point1=0;
int Width_Sum=0;
short DeltaL_Max=0,DeltaR_Min=0;

uint8 Delta_range=20;
uint32 Width_range = 4000;
uint8 End_Row = 30;
uint8 O_in = 50;
uint8 O_out = 50;
uint8 O_er = 50;
uint8 O_delat = 3;
uint8 KU_lastline =35;
uint8 YZ_delta=15;
/* 动态调整 元素补线参数 */
uint8 Add_Carport=50;
uint8 Add_Junc_L =50, Add_Junc_R=50;
uint8 Add_Back_L =70, Add_Back_R=70;
uint8 Add_Circ_In=60, Add_Junc_L0=55,Add_Junc_R0=55;

/*图像信息数组*/
uint8 image_new_gray_bin[ROW_H][COL_W];     //188*120的原始图像抽取成188*60的图像
uint8 image_new_bin[ROW_H][COL_W];          //二值化图像数组

//动态阈值限幅
#define  yuzhi_max 250
#define  yuzhi_min 5
uint8 THRESHOLD=0;
uint8 Old_THRESHOLD=0;

/* image_classify 的参数
 * */
//  白色连通的及格线
#define Block_Value 800
//  黑色截止行
#define BEnd_M      94      //  COL_W/2
#define BEnd_L      30
#define BEnd_R      157     //  COL_W-1-BEnd_L
#define BEnd_ML     60      //  COL_W/2 - (COL_W/2 - BEnd_L)/2
#define BEnd_MR     127     //  COL_W/2 + (COL_W/2 - BEnd_L)/2
//  元素判断类参数
#define Out_Length  2      //  判断图像还在赛道内的阈值
#define Empty_Len   15      //  三叉元素中，对空白区域判断的阈值
#define YYY_Len     15      //  三叉判断的中线截止行长度
#define Road_Len    20      //  直道判断的中线截止长度
#define Curve_Len   15      //  弯道判断的黑线截止长度

/***************************************************************************************************************************
* Function Val:                                 负责执行灰度图像二值化处理的函数
****************************************************************************************************************************/

/*******************************************************************************
* Function Name: Image_Get(void);
* Description  : 原始图像抽取函数
*******************************************************************************/
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

/***************************************************************************************************************************
* Description:                                          以下内容常变
* Function Val:                                 负责执行赛道信息，数组类特征提取的函数
****************************************************************************************************************************/

/*******************************************************************************
* Function Name:    Repair_Line(char begin, char last, float delta, char Dir, short first)
* Description  :    从上行往下行补线
*******************************************************************************/
void Repair_Line(char begin, char last, float delta, char Dir, short first){ //0是左 1是右边
    int i;
    Image_Shot.lastline = begin;
    if(Dir == 0){
        Image_Shot.left[begin] = first;
        for(i=begin+1;i<=last;i++){
            Image_Shot.left[i]= Image_Shot.left[i-1] +delta;
            Image_Shot.mid[i]= Image_Shot.left[i] + (int)Half_Road_Width57 - (57-i) ;
            Image_Shot.right[i]= Image_Shot.left[i] + 2* ((int)Half_Road_Width57 - (57-i) );

            // 边界限幅
            if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
            else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;
            if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
            else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
            // 中线 限幅
            if(Image_Shot.mid[57]<=1 ) Image_Shot.mid[57]=2;
            else if (Image_Shot.mid[57]>=187) Image_Shot.mid[57]=186;
        }
    }
    else{
        Image_Shot.right[begin] = first;
        for(i=begin+1;i<=last;i++){
            Image_Shot.right[i]= Image_Shot.right[i-1] +delta;
            Image_Shot.mid[i]= Image_Shot.right[i] - ((int)Half_Road_Width57 - (57-i) ) ;
            Image_Shot.left[i]= Image_Shot.right[i] - 2* ((int)Half_Road_Width57 - (57-i) ) ;

            // 边界限幅
            if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
            else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;
            if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
            else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
            // 中线 限幅
            if(Image_Shot.mid[57]<=1 ) Image_Shot.mid[57]=2;
            else if (Image_Shot.mid[57]>=187) Image_Shot.mid[57]=186;
        }
    }
}

/*******************************************************************************
* Function Name:    Centerline_kalman(uint8 data_now, uint8 data_last)
* Description  :    //中线卡尔曼滤波
*******************************************************************************/

float Centerline_kalman(float data_now, float data_last)//中线卡尔曼滤波
{
    //卡尔曼滤波参数
    static double center_RealData_P = 0;
    static double center_RealData = 94;
    static double center_NowData_P = 1;
    static double center_Kg;
    uint8 center_Q = 2;
    uint8 center_R = 5;

    center_NowData_P = center_RealData_P + center_Q;
    center_Kg = center_NowData_P * center_NowData_P / (center_NowData_P * center_NowData_P + center_R * center_R);
    center_RealData = ((1 - center_Kg) * data_now + center_Kg * data_last);
    center_RealData_P = (1 - center_Kg) * center_NowData_P;
    return (uint8)center_RealData;
}

uint8 Lno_Limit_Tag=0,Rno_Limit_Tag=0;
/*******************************************************************************
* Function Name:    HuntLine_Right(uint8 line)
* Description  :    右边界扫描
*******************************************************************************/
void HuntLine_Right(uint8 i)
{
    short j=0;
    //找本行边界之前 标记为丢线
    Image_Shot.no_right[i]=1;
    Rno_Limit_Tag=0;

    if( Image_Shot.no_right[i+1]==1)    //上一行丢线了 从中点开始巡线
        j=Image_Shot.begin;
    else    //没丢线 从边界开始巡线
        j=Image_Shot.right[i+1] -13;


    //从中点开始往右找
    for( ; j<=COL_W-14 && j>=1 ; j++)
    {
       if(  image_new_bin[i][j-1]==White_Point && image_new_bin[i][j]==White_Point &&
            image_new_bin[i][j+1]==Black_Point && image_new_bin[i][j+2]==Black_Point )
       {
           if( j <= Image_Shot.Right_Min){  //在收敛范围内才算找到
               Image_Shot.right[i] = j;     //记录右边界坐标
               Image_Shot.no_right[i]=0;    //找到了 标记为无
           }
           else
               Rno_Limit_Tag = 1;
           break;   //找到就结束巡线
       }
    }

    //如果丢线 保持上一帧

}
/*******************************************************************************
* Function Name:    HuntLine_Left(uint8 i)
* Description  :    左边阶扫描
*******************************************************************************/
void HuntLine_Left(uint8 i)
{
    short j=0;
    //找本行边界之前 标记为丢线
    Image_Shot.no_left[i]=1;
    Lno_Limit_Tag=0;

    if( Image_Shot.no_left[i+1]==1) //上一行丢线了 从中点开始巡线
        j=Image_Shot.begin;
    else    //没丢线 从边界开始巡线
        j=Image_Shot.left[i+1] +13;

    //开始往左找
    for( ; j>=13 && j<=COL_W-2 ; j--)
    {
       if(  image_new_bin[i][j-2]==Black_Point && image_new_bin[i][j-1]==Black_Point &&
            image_new_bin[i][j+1]==White_Point && image_new_bin[i][j]==White_Point )
       {
           if( j >= Image_Shot.Left_Max){   //在收敛范围内才算找到
               Image_Shot.left[i] = j;
               Image_Shot.no_left[i]=0;    //找到就 标记 没有丢线
           }
           else
               Lno_Limit_Tag=1;
           break;
       }
    }

    //如果丢线 保持上一帧即可
}
/*******************************************************************************
* Function Name:    Search_Mid(void)
* Description  :    搜寻中线
*******************************************************************************/
// 参数
#define Miss_Judge_Num      4   //判别丢线的数量阈值
#define Miss_Judge_Line     40  //判别丢线的行数 以这个行数划分是那种情况
#define Miss_Judge_Line1    5  //判别丢线的行数 以这个行数划分是那种情况
unsigned char Half_Road_Width57=66; //57层的赛道半宽
unsigned char Old_HRWid=66; //57层的赛道半宽
uint8 Class_NoLine_L =0, Class_NoLine_R =0; //0是找不到丢线 1是不收敛丢线
void Search_Mid(void){
    uint8 i=0;
    int Tag_deltaL=0,Tag_deltaR=0;  //标记是否找到变化率
    int temp_deltaL=0,temp_deltaR=0;  //标记是否找到变化率
    float deltaL_Sum=0,deltaR_Sum=0;    //记录变化率综合
    char deltaL_Num=0,deltaR_Num=0;     //记录变化率数目
    char MissNum_L=0,MissNum_R=0;   //辅助判别丢线情况
    char MissNum_L1=0,MissNum_R1=0;   //辅助判别丢线情况
    char HaveNum_L=0,HaveNum_R=0;   //辅助判别丢线情况
    char HaveDelta_L=0,HaveDelta_R=0;   //辅助判别丢线情况

    static char MissL_Tag_Y=0,MissR_Tag_Y=0;            //辅助判别三叉 丢线情况的标志
    if(Image_Shot.MissL_Tag == 3) MissL_Tag_Y=1;        //如果是仅中间丢线的状态 就标志1 防止之后 误判成三叉
    else if(Image_Shot.MissL_Tag == 0) MissL_Tag_Y=0;   //因为仅中间丢线的话 之后肯定会 没有丢线 所以等待没有丢线 就恢复
    if(Image_Shot.MissR_Tag == 3) MissR_Tag_Y=1;
    else if(Image_Shot.MissR_Tag == 0) MissR_Tag_Y=0;

    //取上一帧的中线 作为这次扫描的起点
    Image_Shot.begin = Image_Shot.mid[57];
    //更新不扫描的两行，与上一帧的底层一致。
    Image_Shot.mid[58]= Image_Shot.mid[57];
    Image_Shot.mid[59]= Image_Shot.mid[57];
    Image_Shot.left[58]= Image_Shot.left[57];
    Image_Shot.left[59]= Image_Shot.left[57];
    Image_Shot.right[58]= Image_Shot.right[57];
    Image_Shot.right[59]= Image_Shot.right[57];
    //状态清空
    Image_Shot.MissL_Tag = 0;
    Image_Shot.MissR_Tag = 0;
    Image_Shot.deltaL = 1;
    Image_Shot.deltaR =-1;
    Image_Shot.Left_Max=0;
    Image_Shot.Right_Min=187;  //记录左右边界的最值
    Class_NoLine_R=0;
    Class_NoLine_L=0;
    Width_Sum = 0;
    DeltaL_Max=0,DeltaR_Min=0;
    //丢线点
    Loss_L_Point=0,Loss_R_Point=0;
    //丢线后第一个不丢点
    Have_L_Point=0,Have_R_Point=0;


    //从57开始巡线
    for( i=57; i>=2; i--){

        HuntLine_Left(i);   //如果丢线 赋值为0
        HuntLine_Right(i);  //如果丢线 赋值为187

        //边界 变化率和收敛 判别
        do{
            //判断 寻找到的左右边界 是否收敛于最值 并记录左右边界的最值   前提是没有丢线 或者底层丢线了
            if( Image_Shot.left[i] >= Image_Shot.Left_Max && (Image_Shot.no_left[i]==0 ) )    // && (Image_Shot.no_left[i]==0 )
                Image_Shot.Left_Max=Image_Shot.left[i];
            else{
                Image_Shot.no_left[i]=1;
            }
            if( Image_Shot.right[i] <= Image_Shot.Right_Min && (Image_Shot.no_right[i]==0 ) )   // && (Image_Shot.no_right[i]==0 )
                Image_Shot.Right_Min=Image_Shot.right[i];
            else{
                Image_Shot.no_right[i]=1;
            }

            //两行都没丢线，而且 还没有找到变化率
            if( Image_Shot.no_right[i+1]!=1 && Image_Shot.no_right[i]!=1 ){

                if(Loss_R_Point != 0 && Have_R_Point==0) Have_R_Point = i; //第一次丢线后 记录第一次不丢线开始的位置

                HaveNum_R++;
                //记录边界的变化率情况
                temp_deltaR = Image_Shot.right[i] - Image_Shot.right[i+1];

                //变化率正常 就更新
                if( temp_deltaR <=Image_Shot.deltaR && temp_deltaR >=-4 && HaveDelta_R!=2){
                    HaveDelta_R = 1;
                    Tag_deltaR = 1;
                    deltaR_Sum+=temp_deltaR;
                    deltaR_Num++;
                    Image_Shot.deltaR = deltaR_Sum/deltaR_Num;
                }
            }
            else{
                HaveNum_R=0;
                if(HaveDelta_R == 1) HaveDelta_R=2;
            }

            if( Image_Shot.no_left[i+1]!=1 && Image_Shot.no_left[i]!=1 ){

                if(Loss_L_Point != 0 && Have_L_Point==0) Have_L_Point = i; //第一次丢线后 记录第一次不丢线开始的位置

                HaveNum_L++;

                //记录边界的变化率情况
                temp_deltaL = Image_Shot.left[i] - Image_Shot.left[i+1];

                //变化率正常 就更新
                if( temp_deltaL >=Image_Shot.deltaL && temp_deltaL <=4 && HaveDelta_L!=2){
                    HaveDelta_L = 1;
                    Tag_deltaL = 1;
                    deltaL_Sum+=temp_deltaL;
                    deltaL_Num++;
                    Image_Shot.deltaL = deltaL_Sum/deltaL_Num;
                }
            }
            else{
                HaveNum_L=0;
                if(HaveDelta_L == 1) HaveDelta_L=2;
            }



        }while(0);

        //边界 丢线情况判别
        do{
            //记录连续丢线的数目 若是已经判别丢线情况 就不记录了
            if(Image_Shot.no_left[i]==1 && Image_Shot.MissL_Tag==0){
                if(Loss_L_Point == 0) Loss_L_Point = i; //第一次丢线 记录丢线开始的位置

                if(Lno_Limit_Tag == 0)
                    MissNum_L++;
                else
                    MissNum_L1++;

                if( MissNum_L >Miss_Judge_Num){
                    if(i>Miss_Judge_Line)    //前几行连续丢线 则是底层丢线
                        Image_Shot.MissL_Tag = 1;
                    else if(i>=Miss_Judge_Line1)   //不然就是中层丢线
                        Image_Shot.MissL_Tag = 2;
                }
                else if( MissNum_L1 >Miss_Judge_Num){
                    if(i>Miss_Judge_Line)    //前几行连续丢线 则是底层丢线
                        Image_Shot.MissL_Tag = 1;
                    else if(i>=Miss_Judge_Line1)   //不然就是中层丢线
                        Image_Shot.MissL_Tag = 2;

                        Class_NoLine_L = 1;
                }
            }
            //不连续就清零
            else{
                MissNum_L=0;
                MissNum_L1=0;
            }

            if(Image_Shot.no_right[i]==1 && Image_Shot.MissR_Tag==0){
                if(Loss_R_Point == 0) Loss_R_Point = i; //第一次丢线 记录丢线开始的位置
                if(Rno_Limit_Tag == 0)
                    MissNum_R++;
                else
                    MissNum_R1++;

                if( MissNum_R >Miss_Judge_Num){
                    if(i>Miss_Judge_Line)    //前几行连续丢线 则是底层丢线
                        Image_Shot.MissR_Tag = 1;
                    else if(i>=Miss_Judge_Line1)    //不然就是中层丢线
                        Image_Shot.MissR_Tag = 2;
                }
                else if( MissNum_R1 >Miss_Judge_Num){
                    if(i>Miss_Judge_Line)    //前几行连续丢线 则是底层丢线
                        Image_Shot.MissR_Tag = 1;
                    else if(i>=Miss_Judge_Line1)    //不然就是中层丢线
                        Image_Shot.MissR_Tag = 2;

                        Class_NoLine_R = 1;
                }
            }
            else{
                MissNum_R=0;
                MissNum_R1=0;
            }

            // 如果没丢线 但是状态是 中间丢线 意味着后面又有线了 标记为状态3 或者 是状态4
            if( Image_Shot.no_right[i]==0 && Image_Shot.MissR_Tag ==2 ){
                if(HaveNum_R>Miss_Judge_Num/2)
                    Image_Shot.MissR_Tag = 3;
            }
            else if( Image_Shot.no_right[i]==0 && Image_Shot.MissR_Tag ==1 && i>=Miss_Judge_Line1){
                if(HaveNum_R>Miss_Judge_Num/2)
                    Image_Shot.MissR_Tag = 4;
            }

            if( Image_Shot.no_left[i]==0 && Image_Shot.MissL_Tag ==2 ){
                if(HaveNum_L>Miss_Judge_Num/2)
                    Image_Shot.MissL_Tag = 3;
            }
            else if( Image_Shot.no_left[i]==0 && Image_Shot.MissL_Tag ==1 && i>=Miss_Judge_Line1 ){
                if(HaveNum_L>Miss_Judge_Num/2)
                    Image_Shot.MissL_Tag = 4;
            }

        }while(0);

        //根据丢线情况 获取中线 补左右边界
        do{
            //补边界线
            if( Image_Shot.no_left[i]==1 && Image_Shot.no_right[i]==1 ){    //全丢
                if( Tag_deltaL ==1 ){   //有变化率 按照变化率补
                    Image_Shot.left[i] = Image_Shot.left[i+1] +Image_Shot.deltaL;
                    if(Image_Shot.left[i] >=187) Image_Shot.left[i]=187;
                }
                if( Tag_deltaR ==1 ){
                    Image_Shot.right[i] = Image_Shot.right[i+1] +Image_Shot.deltaR;
                    if(Image_Shot.right[i] <=0) Image_Shot.right[i]=1;
                }
            }
            else if( Image_Shot.no_left[i]==1 && Image_Shot.no_right[i]==0 ){  //只丢左边
                //只丢左 通过右边补
                Image_Shot.left[i]= Image_Shot.right[i] - 2* ((int)Half_Road_Width57 - (57-i) ) ;
            }
            else if( Image_Shot.no_right[i]==1 && Image_Shot.no_left[i]==0){ //只丢右边
                //只丢右 通过左边补
                Image_Shot.right[i]= Image_Shot.left[i] + 2* ((int)Half_Road_Width57 - (57-i) );
            }

            // 边界限幅
            if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
            else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
            if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
            else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;

            // 补中线
            if( Image_Shot.no_right[i]==1 && Image_Shot.no_left[i]==1 ) //全丢
            {
                Image_Shot.mid[i] = ( Image_Shot.left[i] + Image_Shot.right[i]) /2;
                //虽然两边都丢线 但是仅一边有之前的变化率
                if( Tag_deltaL == 1 && Tag_deltaR == 1 ) {}
                else if( Tag_deltaL == 1 )
                    Image_Shot.mid[i]= Image_Shot.left[i] + (int)Half_Road_Width57 - (57-i) ;
                else if( Tag_deltaR == 1 )
                    Image_Shot.mid[i]= Image_Shot.right[i] - ((int)Half_Road_Width57 - (57-i) ) ;
            }
            else if( Image_Shot.no_right[i]==1 )  //只丢右 通过左边补
                Image_Shot.mid[i]= Image_Shot.left[i] + (int)Half_Road_Width57 - (57-i) ;
            else if( Image_Shot.no_left[i]==1 )   //只丢左 通过右边补
                Image_Shot.mid[i]= Image_Shot.right[i] - ((int)Half_Road_Width57 - (57-i) ) ;
            else
                Image_Shot.mid[i] = ( Image_Shot.left[i] + Image_Shot.right[i]) /2;

            //中线的滤波 从底层往上
            Image_Shot.mid[i]=Centerline_kalman(Image_Shot.mid[i],Image_Shot.mid[i+1]);

            // 中线 限幅
            if(Image_Shot.mid[i]<=1 ) Image_Shot.mid[i]=2;
            else if (Image_Shot.mid[i]>=187) Image_Shot.mid[i]=186;

            //全丢边界的第二次补线 根据中线
            if( Image_Shot.no_left[i]==1 && Image_Shot.no_right[i]==1 ){    //全丢
                if( Tag_deltaL ==0 ){   //无变化率 按照中线补线
                    Image_Shot.left[i] = Image_Shot.mid[i] - ((int)Half_Road_Width57 - (57-i) );
                }
                if( Tag_deltaR ==0 ){
                    Image_Shot.right[i] = Image_Shot.mid[i] + ((int)Half_Road_Width57 - (57-i) );
                }

                // 边界限幅
                if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
                else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
                if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
                else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;
            }

        }while(0);

        //三叉判别,仅判别三角点
         if( Image_Shot.Junction_Enable==1 &&( i <=40 && i>=15 ) && Image_Shot.class == Keep ){     //前面没有识别出入库
             int j=0;
             char Tag_W_Jump=0,Tag_B_Jump=0;
             Y_point=0;
             X_point=0;
             X_point1=0;
             //从左边界 往右边找
             for( j=Image_Shot.left[i]-10; (j<=Image_Shot.right[i]+10 && j<=BEnd_R) ; j++)    //不能超过 这个区域去找
             {
                 if(j<=BEnd_L) j=BEnd_L;
                 // 都没找到时，先找到 白块 ，再去找黑块
                 //  前两行
                 if( image_new_bin[i-2][j-5]==White_Point && image_new_bin[i-2][j-3]==White_Point &&
                     image_new_bin[i-1][j-5]==White_Point && image_new_bin[i-1][j-3]==White_Point &&
                     image_new_bin[i-2][j-1]==White_Point && image_new_bin[i-2][j]==White_Point &&
                     image_new_bin[i-2][j+1]==Black_Point && image_new_bin[i-2][j+2]==Black_Point && //左白右黑
                     Tag_W_Jump == 0 && Tag_B_Jump == 0 )
                 {
                     Y_point = i-2;
                     X_point = j;
                     Tag_W_Jump = 1;
                 }
                 if(  //image_new_bin[i-2][j-1]==Black_Point && image_new_bin[i-2][j]==Black_Point &&
                      image_new_bin[i-1][j+1]==White_Point && image_new_bin[i-1][j+2]==White_Point && //左黑右白
                      image_new_bin[i-2][j+4]==White_Point && image_new_bin[i-2][j+6]==White_Point && //左黑右白
                      image_new_bin[i-2][j+1]==White_Point && image_new_bin[i-2][j+2]==White_Point && //左黑右白
                      Tag_B_Jump == 0 && Tag_W_Jump == 1)
                 {
                     Tag_B_Jump = 1;
                     X_point1 = j;
                 }

             }

             if(    (Tag_B_Jump == 1 ) && Tag_W_Jump == 1
                     && X_point  <BEnd_MR
                     && X_point1 >BEnd_ML
                ){
                 Image_Shot.class=Junction;
             }
         }

         // 记录最大误差变化
         temp_deltaR = Image_Shot.right[i] - Image_Shot.right[i+1];
         temp_deltaL = Image_Shot.left[i]  - Image_Shot.left[i+1] ;
         if(temp_deltaL>DeltaL_Max) DeltaL_Max= temp_deltaL;
         if(temp_deltaR<DeltaR_Min) DeltaR_Min= temp_deltaR;

        //获取begin width
        Image_Shot.width[i] = ( Image_Shot.right[i] - Image_Shot.left[i] );
        Width_Sum +=Image_Shot.width[i];
        Image_Shot.begin = Image_Shot.mid[i];

        //查看 是否满足巡线条件
        if( image_new_bin[i-1][ Image_Shot.mid[i] ] ==Black_Point && image_new_bin[i-2][ Image_Shot.mid[i] ] ==Black_Point ) break; //中点前两行是黑的
        if( image_new_bin[i][ Image_Shot.mid[i] ] ==Black_Point ) break; //中点列是黑的
        if( Image_Shot.mid[i]<=Image_Shot.left[i+1] || Image_Shot.mid[i]>=Image_Shot.right[i+1]) break;
        if( Image_Shot.right[i] <= Image_Shot.left[i] )  break;     //右边小于左边
        if( Image_Shot.right[i] <= Image_Shot.Left_Max )  break;    //右边小于左边收敛
        if( Image_Shot.Right_Min <= Image_Shot.left[i] )  break;    //左边大于右边收敛
        if( Image_Shot.mid[i] <= Image_Shot.Left_Max )  break;      //中间小于左边收敛
        if( Image_Shot.Right_Min <= Image_Shot.mid[i] )  break;     //中间大于右边收敛
        if( Image_Shot.mid[i] <=1 || Image_Shot.mid[i] >=186 ) break;  //中线出界了
    }

    Image_Shot.lastline=i;

    //三叉误判纠正
    if( Image_Shot.class== Junction){
        Image_Shot.class = Keep;

        if(DeltaR_Min < -Delta_range){
            MissR_Tag_Y=0;
        }
        else if(DeltaL_Max > Delta_range){
            MissL_Tag_Y=0;
        }
        if( MissL_Tag_Y == 0 && MissR_Tag_Y==0
            && Image_Shot.lastline >10
            ){ //左右都没有 仅中间丢线的标志

            if( ( Image_Shot.MissL_Tag == 1 && Image_Shot.MissR_Tag == 1)
              ){    //一边全丢 一边中间 //斜率不大 收敛于中间 才是三叉

                    Image_Shot.class=Junction;
            }
            else if( ( Image_Shot.MissL_Tag == 1 && Image_Shot.MissR_Tag == 4)
                   ){   //全丢了 但是右边又找到了 //收敛于中间 才是三叉
                Image_Shot.class=Junction;

            }
            else if( ( Image_Shot.MissL_Tag == 4 && Image_Shot.MissR_Tag == 1)
                   ){   //全丢了 但是左边又找到了 //收敛于中间 才是三叉
                Image_Shot.class=Junction;

            }

        }
    }

    if( Width_Sum/(60 - Image_Shot.lastline) >=Width_range && Image_Shot.lastline>=End_Row && Image_Shot.lastline<=52 &&
        Image_Shot.deltaL==1 && Image_Shot.deltaR==-1 && C_Car.state != Ramp &&
        ( (Image_Shot.MissL_Tag == 0 && Image_Shot.MissR_Tag == 1)    ||
          (Image_Shot.MissL_Tag == 1 && Image_Shot.MissR_Tag == 0) )
        ){
        Image_Shot.class = Ramp;
    }


    //如果满足条件 就根据后面几行没丢线的 补前几行的丢线
    if(Image_Shot.MissL_Tag == 4 && Image_Shot.MissR_Tag == 4){ //两边都丢又找到 需要补中线
        for( i = Image_Shot.lastline+1; (Image_Shot.no_left[i]==0||Image_Shot.no_right[i]==0) ;i++);   //先找到全丢线的行

        for( ; i<=57; i++){ //从丢线的行 开始往57走
            if( Image_Shot.no_left[i]==1 && Image_Shot.no_right[i]==1 ){ //丢线了 就用上一行中线补
                Image_Shot.mid[i]=Image_Shot.mid[i-1];
            }
        }
    }
    if( Image_Shot.MissL_Tag == 4){ //如果是状态4 说明是底层丢线 但是后面又有
        for( i = Image_Shot.lastline+1; Image_Shot.no_left[i]==0 ;i++);   //先找到丢线的行

        for( ; i<=57; i++){ //从丢线的行 开始往57走
            if( Image_Shot.no_left[i]==1 ){ //丢线了 就用中线补
                Image_Shot.left[i] = Image_Shot.mid[i] - ((int)Half_Road_Width57 - (57-i) );
                // 边界限幅
                if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
                else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
            }
        }
    }
    if( Image_Shot.MissR_Tag == 4){ //如果是状态4 说明是底层丢线 但是后面又有
        for( i = Image_Shot.lastline; Image_Shot.no_right[i]==0 ;i++);

        for( ; i<=57; i++){ //从丢线的行 开始往57走
            if( Image_Shot.no_right[i]==1 ){ //丢线了 就用中线补
                Image_Shot.right[i] = Image_Shot.mid[i] + ((int)Half_Road_Width57 - (57-i) );
                // 边界限幅
                if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
                else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;
            }
        }
    }
  for(i = 120 - 3; i > 1; i--)
        {
            Camera_Graph_Show(i);
        }



}
/*******************************************************************************
* Function Name:    Find_Carport(void)
* Description  :    直接找斑马线，找到了就标记图片信息为车库
*******************************************************************************/
void Find_Carport(void)
{
    uint8 i=0,JumpCount=0,LastCol=0;
    uint8 Ending_Row=0;

    //找到斑马线开始的行数
    //保护措施 防止找不到
    if( !(image_new_bin[57][75] & image_new_bin[57][90] & image_new_bin[57][105]) ) //不需要满足黑白跳变的判断
        Ending_Row=57;

    for(i=57; i>10; i--) //限制在这个范围去找斑马线,先找到黑块
    {
        if( !image_new_bin[i][90] & !image_new_bin[i+1][90] )//40列，就是中间  判断标准是 黑白跳变
        {
            Ending_Row=i;
            break;
        }
        if( !image_new_bin[i][100] & !image_new_bin[i+1][100] )//右边一点
        {
            Ending_Row=i;
            break;
        }
        if( !image_new_bin[i][80] & !image_new_bin[i+1][80] )//左边一点
        {
            Ending_Row=i;
            break;
        }
        if( !image_new_bin[i][70] & !image_new_bin[i+1][70] )//左左边一点
        {
            Ending_Row=i;
            break;
        }
        if( !image_new_bin[i][110] & !image_new_bin[i+1][110] )//右右边一点
        {
            Ending_Row=i;
            break;
        }
    }


    //开始计算 查找到的斑马线的 数量；通过计算 有效跳变数
    for(i=30; i<150; i++)
    {
      if(image_new_bin[Ending_Row-2][i]!=image_new_bin[Ending_Row-2][i-1])
      {
        if( (int)(i - LastCol)  > 2 )   //有效跳变：两次跳变间隔小于某个值，斑马线特征，跳变列间隔不会隔太远对吧？/*识别的阈值*/
            JumpCount++;                //理解为有7个左右的列斑马线

        LastCol=i;
      }

      if(JumpCount>8 && Ending_Row>=End_Row )//有效跳变数,且在范围内
      {
        Image_Shot.class=Carport;    //找到车库了
        return;
      }
      else if(JumpCount>8){
        Image_Shot.class=Road;    //找到车库了，但是还不到有效判定范围，改为直道
        return;
      }
    }

}

/***************************************************************************************************************************
* Function Val:                                 负责执行图片分类和最终中线数组的处理
****************************************************************************************************************************/

/*******************************************************************************
* Function Name: Image_Classify(void);
* Description  : 图片分类
* 逻辑：
*   1.先找特征值，白块和黑线截止行，和中线边界线
*   2.根据特征值判断赛道类别
* 主要参数：参考以下宏定义
*******************************************************************************/
void Image_Classify(void){

    //先清空特征值
    for(char i=0;i<6;i++)//特征值，先清空
        Image_Shot.Block_White[i]=0;
    for(char i=0;i<5;i++)//特征值，先清空
        Image_Shot.Black_End[i]=0;

    /* 接下来是特征提取*/
    //黑线截止行检测：|0 1 2 3 4|
    //取
    for (char i = ROW_H-1; i >= 3 ; i--)//中[2]黑线截至行
    {
        Image_Shot.Black_End[2]++;
        if( image_new_bin[i-1][BEnd_M] == Black_Point &&
            image_new_bin[i-2][BEnd_M] == Black_Point )
            break;//连续两行是黑色
    }
    for (char i = ROW_H-1; i >= 3 ; i--)//左[0]黑线截至行
    {
        Image_Shot.Black_End[0]++;
        if( image_new_bin[i-1][BEnd_L] == Black_Point &&
            image_new_bin[i-2][BEnd_L] == Black_Point )
            break;//连续两行是黑色
    }
    for (char i = ROW_H-1; i >= 3 ; i--)//右[4]黑线截至行
    {
        Image_Shot.Black_End[4]++;
        if( image_new_bin[i-1][BEnd_R] == Black_Point &&
            image_new_bin[i-2][BEnd_R] == Black_Point )
            break;//连续两行是黑色
    }
    for (char i = ROW_H-1; i >= 3 ; i--)//中左[1]
    {
        Image_Shot.Black_End[1]++;
        if( image_new_bin[i-1][BEnd_ML] == Black_Point &&
            image_new_bin[i-2][BEnd_ML] == Black_Point )
            break;//连续两行是黑色
    }
    for (char i = ROW_H-1; i >= 3 ; i--)//中右[3]
    {
        Image_Shot.Black_End[3]++;
        if( image_new_bin[i-1][BEnd_MR] == Black_Point &&
            image_new_bin[i-2][BEnd_MR] == Black_Point )
            break;//连续两行是黑色
    }

    //白块检测
    for(int i=1;i<30;i++)//上30行
    {
      for(int j=62;j<124;j++)//中间62列
        if(image_new_bin[i][j] == White_Point)
            Image_Shot.Block_White[1]++;//白块浓度加1
    }
    for(int i=30;i<59;i++)//下30
    {
      for(int j=1;j<62;j++)//左30
        if(image_new_bin[i][j] == White_Point)
            Image_Shot.Block_White[3]++;

      for(int j=62;j<124;j++)//中30
        if(image_new_bin[i][j] == White_Point)
            Image_Shot.Block_White[4]++;

      for(int j=124;j<186;j++)//右30
        if(image_new_bin[i][j] == White_Point)
            Image_Shot.Block_White[5]++;
    }

    /* 接下来是元素判断
     * 车库的判断是独立的，因为特征非常独特。
     * 圆环的判断只是初步的，在寻中线中会进一步判断。
     */
    //先初始化状态
    Image_Shot.class = Keep;

    //先找车库
    if( Car_Go_Tag ==0 ){   //车没开，就保持车库
        Image_Shot.class=Carport;
        return;
    }
    static uint8 First_Frame=1;
    if( First_Frame==1 ) {  //如果是第一帧，必然是车库直接返回
        Image_Shot.class=Carport;
        First_Frame =0;
        return;
    }
    if( C_Car.state != Carport && Image_Shot.Carport_Enable ==1 ){
        Find_Carport();
        if( Image_Shot.class == Carport ) return;    //找到车库了就直接返回，不必犹豫
        if( Image_Shot.class == Road ) return;    //找到直道了就直接返回，不必犹豫
    }


    //识别使能判定
    if( Image_Shot.Carport_Enable == 0){ //如果车库寻找不使能
        static uint8 Search_Count1=0;
        Search_Count1++;     //计数
        if(Search_Count1>50){//超过50帧，则使能，计数清零
            Search_Count1=0;
            Image_Shot.Carport_Enable = 1;
            Image_Shot.Carport_Num++;     //车库次数+1
        }
    }
    if( Image_Shot.CircleL_Enable == 0){ //如果左圆环寻找不使能
        static uint8 Search_Count2=0;
        Search_Count2++;     //计数
        if(Search_Count2> 200){//超过30帧，则使能，计数清零
            Search_Count2=0;
            Image_Shot.CircleL_Enable = 1;
        }
    }
    if( Image_Shot.CircleR_Enable == 0){ //如果右圆环寻找不使能
        static uint8 Search_Count3=0;
        Search_Count3++;     //计数
        if(Search_Count3> 200){//超过30帧，则使能，计数清零
            Search_Count3=0;
            Image_Shot.CircleR_Enable = 1;
        }
    }

    //辅助判别往返 丢线情况的标志
    static char MissL_Tag_P=0,MissR_Tag_P=0;
    if( Image_Shot.MissL_Tag == 2 &&    //如果左中 右边丢线或者丢线找到大斜率 则允许判右往返
        ( (Image_Shot.MissR_Tag == 1) || ( Image_Shot.MissR_Tag == 4 ) ) )
        MissR_Tag_P=1;
    else if( (Image_Shot.MissR_Tag == 0 || Image_Shot.MissL_Tag == 0) && Image_Shot.lastline<=50)  //一边不丢线 就结束判定右往返
        MissR_Tag_P=0;

    if( Image_Shot.MissR_Tag == 2 &&    //如果右中 左边丢线或者丢线找到大斜率 则允许判左往返
        ( (Image_Shot.MissL_Tag == 1) || ( Image_Shot.MissL_Tag == 4 ) ) )
        MissL_Tag_P=1;
    else if( (Image_Shot.MissL_Tag == 0 || Image_Shot.MissR_Tag == 0) && Image_Shot.lastline<=50)  //一边不丢线 就结束判定左往返
        MissL_Tag_P=0;


    //找左右边界线和中线，简单巡线
    Search_Mid();

    /*每块区域 最多1880
     *      |0 1 2|
     *      |3 4 5|
     * 中下区域是核心地区，以它展开判断。
     * 判断连通区域，用白点数量进行判断，是否有可能是赛道。
     * 然后用 黑色截止行辅助判断，白色区域是赛道还是噪声，用丢线情况来判断是否是敞开的。*/
    if(  (Image_Shot.Black_End[2] > Out_Length) ||  (Image_Shot.Black_End[0] > Out_Length) ||  (Image_Shot.Black_End[4] > Out_Length) ){ //判断核心连通，则还未偏离主赛道

        char Left_Block=0,Right_Block=0,Forward_Block=0;    //标志位

        //只有数量达标 才有判断的价值
        if(Image_Shot.Block_White[3]>Block_Value)  Left_Block = 1;  //达标就标志1
        if(Image_Shot.Block_White[5]>Block_Value)  Right_Block = 1;
        if(Image_Shot.Block_White[1]>Block_Value)  Forward_Block = 1;

        //都没丢线 就是直道
        if(Image_Shot.class == Ramp){
            //坡道识别 在巡线的宽度检测里面
        }
        else if( (Image_Shot.MissL_Tag ==0 || Image_Shot.MissL_Tag ==2 || Image_Shot.MissL_Tag ==3) &&
                 (Image_Shot.MissR_Tag ==0 || Image_Shot.MissR_Tag ==2 || Image_Shot.MissR_Tag ==3) ){
            Image_Shot.class=Road;
            if( Image_Shot.Circle_In_Enable == 1 ){
                Image_Shot.Circle_Mid_Enable=1;
                Half_Road_Width57 = Add_Circ_In;
            }
        }
        //直道、右圆环
        else if( Image_Shot.MissL_Tag ==0    &&  //中线够长 左边没丢线
                 Image_Shot.deltaL <= 2 &&
                 Image_Shot.Circle_Find_Enable==1 ){               //中间畅通无阻，则是 直道

            Image_Shot.class=Road;

            if( Image_Shot.CircleR_Enable == 1 && C_Car.state !=CircleR ){
                //再判断圆环
                if(  (Image_Shot.MissR_Tag ==4 || Image_Shot.MissR_Tag ==1 )        //一边没丢 一边底下丢
                     ){

                    //如果... 就圆环使能
                    if(Class_NoLine_R == 0 && Image_Shot.lastline <=  Road_Len   && Right_Block == 1 &&
                       Image_Shot.deltaR <=-O_delat && Image_Shot.Circle_In_Enable ==0){
                        Image_Shot.Circle_In_Enable =1;
                    }
                    //如果... 就正式进入圆环
                    else if( Image_Shot.Circle_Mid_Enable ==1   //
                             ){
                        Image_Shot.Circle_In_Enable =0;
                        Image_Shot.Circle_Mid_Enable =0;
                        Image_Shot.class= CircleR;                //  右环
                        Half_Road_Width57 = Old_HRWid;
                    }

                }
            }

        }
        //直道、左圆环
        else if( Image_Shot.MissR_Tag ==0    &&  //中线够长 右边没丢线
                 Image_Shot.deltaR >=-2 &&
                 Image_Shot.Circle_Find_Enable==1 ){               //中间畅通无阻，则是 直道

            Image_Shot.class=Road;

            if( Image_Shot.CircleL_Enable == 1  && C_Car.state !=CircleL ){
                //再判断圆环
                if(  (Image_Shot.MissL_Tag ==4 || Image_Shot.MissL_Tag ==1 )          //一边没丢 一边底下丢
                     ){

                    //如果... 就圆环使能
                    if( Class_NoLine_L == 0 && Left_Block == 1 && Image_Shot.lastline <=  Road_Len   &&
                        Image_Shot.deltaL >= O_delat && Image_Shot.Circle_In_Enable ==0){
                        Image_Shot.Circle_In_Enable =1;
                    }
                    //如果... 就正式进入圆环
                    else if( Image_Shot.Circle_Mid_Enable ==1   //
                             ){
                        Image_Shot.class=CircleL;                //且左边连通，则是  左环
                        Image_Shot.Circle_In_Enable =0;
                        Image_Shot.Circle_Mid_Enable =0;
                        Half_Road_Width57 = Old_HRWid;
                    }

                }
            }

        }
        //判别右往返
        else if( Image_Shot.MissL_Tag ==1 &&    //左丢 右丢后找到大斜率
                ( (Image_Shot.MissR_Tag == 4 ) )  &&    //(Image_Shot.MissR_Tag == 1) ||
                 C_Car.state == CurveR && MissR_Tag_P==1   &&            //右弯状态 且允许右往返判定
                 //(Image_Shot.Black_End[1]>= Image_Shot.Black_End[2] )  && //中间>3  3>4
                 (Image_Shot.Black_End[2]>= Image_Shot.Black_End[3] )  && //中间>3  3>4
                 (Image_Shot.Black_End[3]>= Image_Shot.Black_End[4] )  && Image_Shot.Back_Find_Enable==1  ){

                Image_Shot.class=BackR;    //则是 右往返
        }
        //判别左往返
        else if( Image_Shot.MissR_Tag ==1 &&   //右丢 左丢后找到大斜率
                 ( (Image_Shot.MissL_Tag ==4 ) )  &&    // (Image_Shot.MissL_Tag == 1) ||
                 C_Car.state == CurveL && MissL_Tag_P==1   &&            //左弯状态 且允许左往返判定
                 //(Image_Shot.Black_End[3]>= Image_Shot.Black_End[2] )  && //中间>3  3>4
                 (Image_Shot.Black_End[2]>= Image_Shot.Black_End[1] )  && //中间>1  1>0
                 (Image_Shot.Black_End[1]>= Image_Shot.Black_End[0] )  && Image_Shot.Back_Find_Enable==1  ){

                Image_Shot.class=BackL;    //则是 左往返
        }
        else if( Image_Shot.class==Junction ){
            //三叉 识别 在巡线里面
        }
        //左弯
        else if( Image_Shot.MissL_Tag ==1 && Image_Shot.MissR_Tag ==0 &&
                (Image_Shot.Black_End[2]<= Image_Shot.Black_End[1] )   &&
                (Image_Shot.Black_End[3]<= Image_Shot.Black_End[2] )
                )   {   // 左弯
            Image_Shot.class=CurveL;
        }
        //右弯
        else if( Image_Shot.MissR_Tag ==1 && Image_Shot.MissL_Tag ==0 &&
                (Image_Shot.Black_End[2]<= Image_Shot.Black_End[3] )   &&
                (Image_Shot.Black_End[1]<= Image_Shot.Black_End[2] )
                )  {   //  右弯
            Image_Shot.class=CurveR;
        }
    }
    else{//判断核心未连通，偏离主赛道
        Image_Shot.class =Outroad; //则表示出赛道了
    }

}



/*******************************************************************************
* Function Name: Car_Mealy(void);
* Description  : 车体状态机函数
* 逻辑：参考状态转移图
* 主要参数：状态转移图
* 特别注意，keep和outroad的使用
*******************************************************************************/

static char CircleL_O_Tag=0,CircleR_O_Tag=0;
uint8 Flash_Wait_Num = 5;
void Car_Mealy(road_state Next_State){
    //一般输入的 Next_State就是当前图片状态的结果
    //记录当前过去的帧数
    static char Flash_Num=0;
    Flash_Num++;
    C_Car.Old_state = C_Car.state;

    //如果当前状态，和下次状态一致，则直接结束
    if(Next_State == C_Car.state){
        return;
    }
    else if( Next_State == Outroad ){ // Next_State == Keep ||
        return;
    }
    //状态不相等，且不是keep和outroad状态，则证明需要判别是否改变当前状态
    else{
        //当前状态是车库
        if( C_Car.state == Carport){
            //先判断是第几次车库
            if(Image_Shot.Carport_Num == 0){    //出库
                //则下一次状态可能是直道也可能是弯道
                if( ( Next_State == Road || Next_State == CurveR || Next_State == CurveL ) && (Image_Shot.lastline <=KU_lastline)  ) //
                {
                    //当前状态过渡到下一状态
                    C_Car.state = Next_State;
                    Flash_Num=0;
                    //如果没有开启，则不允许改变出入库状态
                    if( Car_Go_Tag ==1 ) {
                        Image_Shot.Carport_Enable =0; // 关闭使能，短时间内不在查找车库
                        Image_Shot.Junction_Tag= !Image_Shot.Junction_Tag;  //改变三叉方向
                    }
                }

            }
            else if(Image_Shot.Carport_Num == 1){   //第二次
                //则下一次状态只可能是直道
                if( Next_State == Road )
                {
                    C_Car.state = Next_State;
                    Flash_Num=0;
                    //如果没有开启，则不允许改变出入库状态
                    if( Car_Go_Tag ==1 ) {
                        Image_Shot.Carport_Enable =0; // 关闭使能，短时间内不在查找车库
                        Image_Shot.Junction_Tag= !Image_Shot.Junction_Tag;  //改变三叉方向
                    }
                }
            }
            else if(Image_Shot.Carport_Num == 2){   //入库
                //则下一次状态只可能是弯道
//                Stop_Num=1;
            }
            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;
        }
        //当前状态是三叉
        else if( C_Car.state == Junction ){
            if( Flash_Num>Flash_Wait_Num ){
                //当前状态过渡到下一状态
                C_Car.state = Road;
                Flash_Num=0;
//                BuzzerQuiet;
            }

            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;
        }
        //当前状态是左圆环
        else if( C_Car.state == CircleL){
            //则下一次状态只可能是直道
            if( (Image_Shot.MissR_Tag==0 ) &&
                //Next_State == Road &&
                Image_Shot.Circle_Out_Enable ==1 && Flash_Num> Flash_Wait_Num
               ){
                Image_Shot.Circle_Out_Enable =0;
                Image_Shot.CircleL_Enable =0;
                Image_Shot.Circle_In_Enable =0;
                Image_Shot.Circle_Mid_Enable =0;
                CircleL_O_Tag=0;
//                BuzzerQuiet;
            Half_Road_Width57 = Old_HRWid;
                //当前状态过渡到下一状态
                C_Car.state = Next_State;
                Flash_Num=0;
            }
            else if( Next_State == CurveL && CircleL_O_Tag==0 && Flash_Num> Flash_Wait_Num && Image_Shot.Circle_Out_Enable !=1 ){
                //因为入环了 因此出环才使能
                CircleL_O_Tag=1;
            }

            if( CircleL_O_Tag == 1 &&
                (Image_Shot.MissL_Tag ==1 || Image_Shot.MissL_Tag ==4 ) &&
                (Image_Shot.MissR_Tag ==1 || Image_Shot.MissR_Tag ==4 ) //|| Image_Shot.MissR_Tag ==2
                //&& Flash_Num> 3*Flash_Wait_Num
               ){
                CircleL_O_Tag=0;
                Image_Shot.Circle_Out_Enable =1;
            }
        }
        //当前状态是右圆环
        else if( C_Car.state == CircleR){
            //则下一次状态只可能是直道
            if( (Image_Shot.MissL_Tag==0 ) &&
                //Next_State == Road &&
                Image_Shot.Circle_Out_Enable ==1 &&  Flash_Num> Flash_Wait_Num
               ){
                Image_Shot.Circle_Out_Enable =0;
                Image_Shot.Circle_Mid_Enable =0;
                Image_Shot.CircleR_Enable =0;
                Image_Shot.Circle_In_Enable =0;
                CircleR_O_Tag=0;
//                BuzzerQuiet;
            Half_Road_Width57 = Old_HRWid;
                //当前状态过渡到下一状态
                C_Car.state = Next_State;
                Flash_Num=0;
            }
            else if( Next_State == CurveR && CircleR_O_Tag==0 && Flash_Num> Flash_Wait_Num && Image_Shot.Circle_Out_Enable !=1 ){
                //因为入环了 因此出环才使能
                CircleR_O_Tag=1;
            }

            if( CircleR_O_Tag == 1 &&
                (Image_Shot.MissL_Tag ==1 || Image_Shot.MissL_Tag ==4) && // || Image_Shot.MissL_Tag ==2
                (Image_Shot.MissR_Tag ==1 || Image_Shot.MissR_Tag ==4 )
                //&& Flash_Num> 3*Flash_Wait_Num
               ){
                CircleR_O_Tag=0;
                Image_Shot.Circle_Out_Enable =1;
            }
        }
        //当前状态是左往返
        else if( C_Car.state == BackL ){
            if( Flash_Num>Flash_Wait_Num && Image_Shot.Back_Out_Enable == 1 ){
                //当前状态过渡到下一状态
                C_Car.state = Road;
                Image_Shot.Back_Out_Enable=0;
                Flash_Num=0;
//                BuzzerQuiet;
            }
            else if( (Image_Shot.Black_End[2] < 20) &&
                     (Image_Shot.Black_End[0] > Out_Length) &&
                     (Image_Shot.Black_End[4] > Out_Length)
                     ){
                Flash_Num=0;
                Image_Shot.Back_Out_Enable=1;   //判定成功的时候就是出往返的时候
            }
            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;
        }
        //当前状态是右往返
        else if( C_Car.state == BackR ){
            if( Flash_Num>Flash_Wait_Num && Image_Shot.Back_Out_Enable == 1){
                //当前状态过渡到下一状态
                C_Car.state = Road;
                Image_Shot.Back_Out_Enable=0;
                Flash_Num=0;
//                BuzzerQuiet;
            }
            else if( (Image_Shot.Black_End[2] < 20) &&
                     (Image_Shot.Black_End[0] > Out_Length) &&
                     (Image_Shot.Black_End[4] > Out_Length)
                     ){
                Flash_Num=0;
                Image_Shot.Back_Out_Enable=1;   //判定成功的时候就是出往返的时候
            }
            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;
        }
        else if( C_Car.state == Ramp ){
            if( Flash_Num>=Flash_Wait_Num && Width_Sum>0
                && Image_Shot.lastline <=End_Row
                && (Image_Shot.MissL_Tag == 0 || Image_Shot.MissR_Tag == 0)
                ){
                //当前状态过渡到下一状态
                C_Car.state = Road;
                Flash_Num=0;
//                BuzzerQuiet;
            }
            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;
        }
        else if (Next_State == Keep) return;
        //当前状态是弯道
        else if( C_Car.state == CurveL || C_Car.state == CurveR ){

            //如果下一个状态是圆环，则说明是直道
            if( Next_State == CircleL || Next_State == CircleR ) {
                C_Car.state = Road;
            }
            //否则直接接收下一次的状态
            else
                C_Car.state = Next_State;

            //在弯道的时候 清圆环状态
            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;

            Flash_Num=0;
        }
        //其他状态 直道
        else {
            //则直接接收下一次的状态
            C_Car.state = Next_State;
            Flash_Num=0;
        }
    }
}



/*******************************************************************************
* Function Name: Mid_Mealy(road_state Next_State);
* Description  : 中线状态机函数
* 逻辑：参考状态转移图
* 主要参数：状态转移图
*******************************************************************************/
void Mid_Mealy(road_state Next_State){
    // 根据巡线和图片种类，对中线状态进行状态机描述
    if( Next_State == CircleL ){
        //如果状态是圆环 则输出弯道
        Image_Shot.Mid_class=CurveL;
    }
    else if( Next_State == CircleR ){
        //如果状态是圆环 则输出弯道
        Image_Shot.Mid_class=CurveR;
    }
    else if( Next_State == Junction ){   //直接根据三叉的标志，给当前的图片类型赋值
        if(Image_Shot.Junction_Tag == 0){
            Image_Shot.Mid_class =CurveL;
        }
        else {
            Image_Shot.Junction_Tag = 1;
            Image_Shot.Mid_class =CurveR;
        }
    }
    else if( Next_State == BackL ){   //左往返 左弯
        Image_Shot.Mid_class = CurveL;

        if( Image_Shot.Back_Out_Enable==1 ){//该出去了
            Image_Shot.Mid_class=CurveR;    //右弯
        }

    }
    else if( Next_State == BackR ){   //右往返 右弯
        Image_Shot.Mid_class = CurveR;

        if( Image_Shot.Back_Out_Enable==1 ){//该出去了
            Image_Shot.Mid_class=CurveL;    //左弯
        }

    }
    else if( Next_State == Carport ){    //车库，判断是第几次车库

        if(Image_Shot.Carport_Num == 0){
            //出库
            if(Image_Shot.Carport_Direction ==0)
                Image_Shot.Mid_class = CurveR;
            else
                Image_Shot.Mid_class = CurveL;

        }
        else if(Image_Shot.Carport_Num == 1){
            //保持直线
            Image_Shot.Mid_class = Road;
        }
        else if(Image_Shot.Carport_Num >= 2){
            Image_Shot.Carport_Num = 2;
            //入库
            if(Image_Shot.Carport_Direction ==0)
                Image_Shot.Mid_class = CurveR;
            else
                Image_Shot.Mid_class = CurveL;
        }
    }
    else if(Next_State == Ramp){
        Image_Shot.Mid_class = Road;
    }
    else if(Next_State == CurveL){
        Image_Shot.Mid_class = CurveL;
    }
    else if(Next_State == CurveR){
        Image_Shot.Mid_class = CurveR;
    }
    else
        Image_Shot.Mid_class = Road;
}



/*******************************************************************************
* Function Name: Mid_Proc(void);
* Description  : 中线处理函数
* 逻辑：
*   1.先判断元素，如果是特定元素就输出对应的Image_Class 否则就是keep或out状态。同时获取mid数组
*   2.根据Image_Class，来改变Car状态。如果是keep和out，则不改变。
*   3.根据Car状态，来改变Mid状态。输出的mid只有直道和弯道。
*   4.根据mid状态，和Image_Class是否为keep和out，来决定输出的中线
* 主要参数：执行中线状态机输出时，补线的参数：
*******************************************************************************/
//黑线截止行在前面定义过了
//#define BEnd_M

float Weight_Auto[100]={   //计算用的权值
    // 原先的权值
    2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,
    5,5,5,5,5,5,5,5,5,5,
    4,4,4,4,4,4,4,4,4,4,
    3,3,3,3,3,3,3,3,1,1,    //0~59

    // 为了适应动态前瞻给出的权值
    0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,
    0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,
    0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,
    1,1,1,1,1,1,1,1,1,1     //60~99
};

#define Delta_Range 5       //变化率限幅
//#define Curve_coco  3       //弯道 补偿参数 在函数内部定义了 方便 根据元素修改补线系数
void Mid_Proc(void){
    //弯道 补偿参数
    float Curve_coco =1;
    //权值变量
    float   CenterSum=0,WeightSum=0.1;
    //中线变化率，递进版，不是两次的差值
    float temp_delta[60]={0},Curve_Delta=0;
    int temp_Mid = 0;

    Image_Classify();               //改变 Image_Shot.class 和 mid
    Car_Mealy(Image_Shot.class);    //改变 C_Car.state
    Mid_Mealy(C_Car.state);         //改变 Image_Shot.Mid_class

    //如果是out 就直接结束 不改变Mid_Out 而且要修正 巡线结果
    if( Image_Shot.class == Outroad ){
        //与上一帧的底层一致。
        Image_Shot.mid[57] = Image_Shot.mid[58];
        Image_Shot.left[57]= Image_Shot.left[58];
        Image_Shot.right[57]= Image_Shot.right[58];
        return;
    }

    //补线参数 保护
    if(Add_Carport < 10 )       Add_Carport=90;
    else if(Add_Carport > 90 )  Add_Carport=90;
    if(Add_Junc_L < 0 )        Add_Junc_L =90;
    else if(Add_Junc_L > 90 )   Add_Junc_L =90;
    if(Add_Junc_R < 0 )        Add_Junc_R =90;
    else if(Add_Junc_R > 90 )   Add_Junc_R =90;
    if(Add_Back_L < 0 )        Add_Back_L =90;
    else if(Add_Back_L > 90 )   Add_Back_L =90;
    if(Add_Back_R < 0 )        Add_Back_R =90;
    else if(Add_Back_R > 90 )   Add_Back_R =90;
    if(Add_Circ_In < 20 )       Add_Circ_In =90;
    else if(Add_Circ_In > 90 )  Add_Circ_In =90;


    //如果是 元素补线 则启用补线 否则不启用
    if(C_Car.state != Image_Shot.Mid_class){

            //中线修正
            //底层丢线  则以上一帧为起点
            if( Image_Shot.MissL_Tag ==1 && Image_Shot.MissR_Tag ==1 )
                temp_Mid = Image_Shot.mid[58];
            else
                //以本帧图像的中线为起点
                temp_Mid = Image_Shot.mid[57];

            //如果是 元素补线 则启用补线
            Curve_coco =2;
            if( C_Car.state == Ramp) {  //坡道走road
//                BuzzerRing;
            }
            else if( C_Car.state == Junction) {  //三叉补大线
                Curve_coco = 2;
//                BuzzerRing;

                if(Loss_L_Point != 0 && Have_L_Point != 0 ){
                    if( Image_Shot.class == CurveL ){
                        Repair_Line(Have_L_Point,Loss_L_Point,Add_Junc_L0,1,Image_Shot.left[Have_L_Point]);
                    }
                    else if( Image_Shot.class == CurveR ){
                        Repair_Line(Have_L_Point,Loss_L_Point,-Add_Junc_R0,0,Image_Shot.left[Have_L_Point]);
                    }
                    goto Final_Out;
                }
                else if(Loss_R_Point != 0 && Have_R_Point != 0 ){
                    if( Image_Shot.class == CurveL ){
                        Repair_Line(Have_R_Point,Loss_R_Point,Add_Junc_L0,1,Image_Shot.right[Have_R_Point]);
                    }
                    else if( Image_Shot.class == CurveR ){
                        Repair_Line(Have_R_Point,Loss_R_Point,-Add_Junc_R0,0,Image_Shot.right[Have_R_Point]);
                    }
                    goto Final_Out;
                }
            }
            else if( C_Car.state == CircleL || C_Car.state == CircleR) { //圆环正常点
//                BuzzerRing;
                if(Image_Shot.Circle_Out_Enable ==1 ){  //出圆环
                    //Half_Road_Width57 = Add_Circ_Out2;

                    if(Loss_L_Point != 0 && Have_L_Point != 0 && C_Car.state == CircleL && Image_Shot.MissR_Tag!=0 ){
                        Repair_Line(Have_L_Point,Loss_L_Point,O_out,1,Image_Shot.left[Have_L_Point]);
                        goto Final_Out;
                    }
                    else if(Loss_R_Point != 0 && Have_R_Point != 0 && C_Car.state == CircleR && Image_Shot.MissL_Tag!=0 ){
                        Repair_Line(Have_R_Point,Loss_R_Point,-O_out,0,Image_Shot.right[Have_R_Point]);
                        goto Final_Out;
                    }
                    else if(Image_Shot.MissL_Tag==0 || Image_Shot.MissR_Tag==0){
                        goto Final_Out;
                    }
                }
                else if( CircleL_O_Tag ==1 || CircleR_O_Tag ==1) { //已经进入圆环
                    goto Final_Out;
                }
                else if( CircleR_O_Tag ==0 && CircleL_O_Tag==0 ){   //还在进圆环
//                    Half_Road_Width57 = Add_Circ_Out;

                    if(Loss_L_Point != 0 && Have_L_Point != 0 && C_Car.state == CircleL){
                        Repair_Line(Have_L_Point,Loss_L_Point,O_in,1,Image_Shot.left[Have_L_Point]);
                        goto Final_Out;
                    }
                    else if(Loss_R_Point != 0 && Have_R_Point != 0 && C_Car.state == CircleR){
                        Repair_Line(Have_R_Point,Loss_R_Point,-O_in,0,Image_Shot.right[Have_R_Point]);
                        goto Final_Out;
                    }
                }
            }
            else if( C_Car.state == Carport &&  Image_Shot.Carport_Num == 2) {  //入库大一点
                Curve_coco = 8;
            }
            else if( C_Car.state == BackL ||  C_Car.state == BackR ) {  //往返正常点
//                BuzzerRing;
                Curve_coco = 2;
            }

            //先修正弯道的 斜率和中线
            if( Image_Shot.Mid_class == CurveL){
                //分母取小的
                float high_t=Image_Shot.Black_End[1];
                if(Image_Shot.Black_End[0] < high_t)
                    high_t = Image_Shot.Black_End[0];
                if(high_t<=10) high_t = 10;

                //划分中线的区间，区间不同，补线参考不同
                if( temp_Mid > COL_W/2){
                    Curve_Delta=-Curve_coco;
                    temp_Mid = COL_W/2+ Curve_Delta;
                }
                else if( temp_Mid > BEnd_ML)
                    Curve_Delta=-Curve_coco*(temp_Mid - BEnd_ML)/high_t;
                else if( temp_Mid > BEnd_L)
                    Curve_Delta=-Curve_coco*(temp_Mid - BEnd_L)/high_t;
                else
                    Curve_Delta=-Curve_coco;

                if( Curve_Delta<-Delta_Range ) Curve_Delta=-Delta_Range;
                else if( Curve_Delta>=-2 ) Curve_Delta=-2;

            }
            else if(Image_Shot.Mid_class == CurveR){
                //分母取小的
                float high_t=Image_Shot.Black_End[3];
                if(Image_Shot.Black_End[4] < high_t)
                    high_t = Image_Shot.Black_End[4];
                if(high_t<=10) high_t = 10;

                //划分中线的区间，区间不同，补线参考不同
                if( temp_Mid < COL_W/2){
                    Curve_Delta= Curve_coco;
                    temp_Mid = COL_W/2 + Curve_Delta;
                }
                else if( temp_Mid < BEnd_MR)
                    Curve_Delta= Curve_coco*(BEnd_MR - temp_Mid)/high_t;
                else if( temp_Mid < BEnd_R)
                    Curve_Delta= Curve_coco*(BEnd_R - temp_Mid)/high_t;
                else
                    Curve_Delta= Curve_coco;

                if( Curve_Delta>Delta_Range ) Curve_Delta=Delta_Range;
                else if( Curve_Delta<=2 ) Curve_Delta=2;

            }


            // 再根据中线状态，进行对中线数组中，不正常的部分进行补偿
            for(uint8 i=56; i>Image_Shot.lastline; i--)
            {
                //定义变化率为delta
                float delta = Image_Shot.mid[i]- Image_Shot.mid[i+1];
                //变化率限幅
                if( delta>Delta_Range ) delta=Delta_Range;
                else if( delta<-Delta_Range ) delta=-Delta_Range;

                //先累计变化率
                temp_delta[i] = temp_delta[i+1];

                //根据输出的中线类型 对变化率进行修正
                if(Image_Shot.Mid_class == Road){
                    temp_delta[i] += delta;
                }
                else if(Image_Shot.Mid_class == CurveL){   //左弯 修正不向左的变化率
                    if( (delta >=0 )  )
                    {
                        temp_delta[i] += Curve_Delta;
                    }
                    else if( delta <0)  //趋势正常 就变化率
                        temp_delta[i] +=delta;
                }
                else if(Image_Shot.Mid_class == CurveR){   //右弯
                    if( (delta <=0 ) )
                    {
                        temp_delta[i] += Curve_Delta;
                    }
                    else if( delta >0)
                        temp_delta[i] +=delta;
                }
            }

            // lastline限幅
            if(Image_Shot.lastline >=58)
                Image_Shot.lastline=57;
            else if(Image_Shot.lastline <1)
                Image_Shot.lastline=1;

            // 根据变化率 改变中线输出
            Image_Shot.mid[57] = temp_Mid;
            for(uint8 i=56; i>Image_Shot.lastline; i--){
                int temp= temp_Mid + (int)temp_delta[i] ;

                //中线的滤波 从底层往上
                Image_Shot.mid[i]=Centerline_kalman((float)temp,Image_Shot.mid[i+1]);

                //中线限幅
                if(temp >=186) temp=186;
                else if(temp<=0) temp =1;
                Image_Shot.mid[i] = temp;

            }

            // 根据补线修正 边界线
            for(uint8 i=57; i>Image_Shot.lastline; i--){
                    if(Image_Shot.no_left[i]==1){
                        Image_Shot.left[i] = Image_Shot.mid[i] - ((int)Half_Road_Width57 - (57-i) );
                        // 边界限幅
                        if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
                        else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
                    }

                    if(Image_Shot.no_right[i]==1){
                        Image_Shot.right[i] = Image_Shot.mid[i] + ((int)Half_Road_Width57 - (57-i) );
                        // 边界限幅
                        if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
                        else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;
                    }
            }

    }
Final_Out:
//使用 动态前瞻
    if(Image_Shot.ToBe >100 ) Image_Shot.ToBe=0;
    else if (Image_Shot.ToBe> 40) Image_Shot.ToBe=40;
    for(int i=ROW_H-3; i>= Image_Shot.lastline ;i--)//LastLine
    {
      CenterSum+=Image_Shot.mid[i]*Weight_Auto[i +Image_Shot.ToBe];
      WeightSum+=Weight_Auto[i +Image_Shot.ToBe];
    }

    Image_Shot.Mid_Out=(CenterSum/WeightSum);


    // 对补线的错误 进行一定的修正
    if( C_Car.state == Ramp) {  //坡道不拐弯
        if(Image_Shot.Mid_Out<=74)
            Image_Shot.Mid_Out=74;
        else if(Image_Shot.Mid_Out>=114)
            Image_Shot.Mid_Out=114;
    }
    else if( C_Car.state == Junction) {  //三叉补大线
        if(Image_Shot.Mid_class == CurveR){
            if(Image_Shot.Mid_Out<=Steer_Con.goal + Add_Junc_R)
                Image_Shot.Mid_Out=Steer_Con.goal + Add_Junc_R;
        }
        else if(Image_Shot.Mid_class == CurveL){
            if(Image_Shot.Mid_Out>= Steer_Con.goal - Add_Junc_L)
                Image_Shot.Mid_Out=Steer_Con.goal - Add_Junc_L;
        }
    }
    else if( C_Car.state == BackL ) { //出往返
        if(Image_Shot.Mid_Out<= Steer_Con.goal + Add_Back_L)
            Image_Shot.Mid_Out= Steer_Con.goal + Add_Back_L;
    }
    else if( C_Car.state == BackR ) { //出往返
        if(Image_Shot.Mid_Out>= Steer_Con.goal - Add_Back_R)
            Image_Shot.Mid_Out= Steer_Con.goal - Add_Back_R;
    }
    else if( C_Car.state == Carport) {  //出入库补大线
        if(Image_Shot.Mid_class == CurveR){
            if(Image_Shot.Mid_Out<=Steer_Con.goal +Add_Carport)
                Image_Shot.Mid_Out=Steer_Con.goal +Add_Carport;
        }
        else if(Image_Shot.Mid_class == CurveL){
            if(Image_Shot.Mid_Out>=Steer_Con.goal -Add_Carport)
                Image_Shot.Mid_Out=Steer_Con.goal -Add_Carport;
        }
    }

    if(Image_Shot.Mid_Out>=187) Image_Shot.Mid_Out=186;
    else if(Image_Shot.Mid_Out<=1) Image_Shot.Mid_Out=1;

    //圆环 防误判
    if(Image_Shot.Mid_Out<(Steer_Con.goal - O_er) && C_Car.state!=CircleL && C_Car.state!=CircleR ){
        //在弯道的时候 清圆环状态
        Image_Shot.Circle_Out_Enable =0;
        Image_Shot.Circle_In_Enable =0;
        Image_Shot.Circle_Mid_Enable =0;

    }
    else if(Image_Shot.Mid_Out>(Steer_Con.goal + O_er) && C_Car.state!=CircleL && C_Car.state!=CircleR ){
        //在弯道的时候 清圆环状态
        Image_Shot.Circle_Out_Enable =0;
        Image_Shot.Circle_In_Enable =0;
        Image_Shot.Circle_Mid_Enable =0;
    }

    /*这里再加 路径优化
     *1: 前瞻为0的时候 重心在39行到29行
     *2: 得到的权值中线 在去往重心的路上 不能遇到左右边界线
     *3: 如果遇到了 则要适当的往回走 避免碰到左右边界线
     * */

    car_control( 250, Image_Shot.Mid_Out );
}





