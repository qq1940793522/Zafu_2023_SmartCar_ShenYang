#include "zf_common_headfile.h"
#include "image.h"
#include "Car_Control.h"


//���ߵ�
uint8 Loss_L_Point=0,Loss_R_Point=0;
//���ߺ��һ��������
uint8 Have_L_Point=0,Have_R_Point=0;

//������ʱ����
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
/* ��̬���� Ԫ�ز��߲��� */
uint8 Add_Carport=50;
uint8 Add_Junc_L =50, Add_Junc_R=50;
uint8 Add_Back_L =70, Add_Back_R=70;
uint8 Add_Circ_In=60, Add_Junc_L0=55,Add_Junc_R0=55;

/*ͼ����Ϣ����*/
uint8 image_new_gray_bin[ROW_H][COL_W];     //188*120��ԭʼͼ���ȡ��188*60��ͼ��
uint8 image_new_bin[ROW_H][COL_W];          //��ֵ��ͼ������

//��̬��ֵ�޷�
#define  yuzhi_max 250
#define  yuzhi_min 5
uint8 THRESHOLD=0;
uint8 Old_THRESHOLD=0;

/* image_classify �Ĳ���
 * */
//  ��ɫ��ͨ�ļ�����
#define Block_Value 800
//  ��ɫ��ֹ��
#define BEnd_M      94      //  COL_W/2
#define BEnd_L      30
#define BEnd_R      157     //  COL_W-1-BEnd_L
#define BEnd_ML     60      //  COL_W/2 - (COL_W/2 - BEnd_L)/2
#define BEnd_MR     127     //  COL_W/2 + (COL_W/2 - BEnd_L)/2
//  Ԫ���ж������
#define Out_Length  2      //  �ж�ͼ���������ڵ���ֵ
#define Empty_Len   15      //  ����Ԫ���У��Կհ������жϵ���ֵ
#define YYY_Len     15      //  �����жϵ����߽�ֹ�г���
#define Road_Len    20      //  ֱ���жϵ����߽�ֹ����
#define Curve_Len   15      //  ����жϵĺ��߽�ֹ����

/***************************************************************************************************************************
* Function Val:                                 ����ִ�лҶ�ͼ���ֵ������ĺ���
****************************************************************************************************************************/

/*******************************************************************************
* Function Name: Image_Get(void);
* Description  : ԭʼͼ���ȡ����
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
* Description  : ��ѵ�������ȡ��ֵ����ֵ
*******************************************************************************/
void yuzhiget(void)
{
    static char ERROR = 2;       //������ֵӦ���еĲ�ֵ
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
                else if(image_new_gray_bin[i][j] > max)               //���max��minֵ
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
* Description  : ��ֵ��ִ�к���
*******************************************************************************/
void Image_binaryzation(void)                       //���Ż���ֵ�޷�6.12
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



// ��ȥ���� ֱ�Ӵ���
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
* Description:                                          �������ݳ���
* Function Val:                                 ����ִ��������Ϣ��������������ȡ�ĺ���
****************************************************************************************************************************/

/*******************************************************************************
* Function Name:    Repair_Line(char begin, char last, float delta, char Dir, short first)
* Description  :    �����������в���
*******************************************************************************/
void Repair_Line(char begin, char last, float delta, char Dir, short first){ //0���� 1���ұ�
    int i;
    Image_Shot.lastline = begin;
    if(Dir == 0){
        Image_Shot.left[begin] = first;
        for(i=begin+1;i<=last;i++){
            Image_Shot.left[i]= Image_Shot.left[i-1] +delta;
            Image_Shot.mid[i]= Image_Shot.left[i] + (int)Half_Road_Width57 - (57-i) ;
            Image_Shot.right[i]= Image_Shot.left[i] + 2* ((int)Half_Road_Width57 - (57-i) );

            // �߽��޷�
            if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
            else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;
            if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
            else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
            // ���� �޷�
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

            // �߽��޷�
            if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
            else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;
            if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
            else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
            // ���� �޷�
            if(Image_Shot.mid[57]<=1 ) Image_Shot.mid[57]=2;
            else if (Image_Shot.mid[57]>=187) Image_Shot.mid[57]=186;
        }
    }
}

/*******************************************************************************
* Function Name:    Centerline_kalman(uint8 data_now, uint8 data_last)
* Description  :    //���߿������˲�
*******************************************************************************/

float Centerline_kalman(float data_now, float data_last)//���߿������˲�
{
    //�������˲�����
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
* Description  :    �ұ߽�ɨ��
*******************************************************************************/
void HuntLine_Right(uint8 i)
{
    short j=0;
    //�ұ��б߽�֮ǰ ���Ϊ����
    Image_Shot.no_right[i]=1;
    Rno_Limit_Tag=0;

    if( Image_Shot.no_right[i+1]==1)    //��һ�ж����� ���е㿪ʼѲ��
        j=Image_Shot.begin;
    else    //û���� �ӱ߽翪ʼѲ��
        j=Image_Shot.right[i+1] -13;


    //���е㿪ʼ������
    for( ; j<=COL_W-14 && j>=1 ; j++)
    {
       if(  image_new_bin[i][j-1]==White_Point && image_new_bin[i][j]==White_Point &&
            image_new_bin[i][j+1]==Black_Point && image_new_bin[i][j+2]==Black_Point )
       {
           if( j <= Image_Shot.Right_Min){  //��������Χ�ڲ����ҵ�
               Image_Shot.right[i] = j;     //��¼�ұ߽�����
               Image_Shot.no_right[i]=0;    //�ҵ��� ���Ϊ��
           }
           else
               Rno_Limit_Tag = 1;
           break;   //�ҵ��ͽ���Ѳ��
       }
    }

    //������� ������һ֡

}
/*******************************************************************************
* Function Name:    HuntLine_Left(uint8 i)
* Description  :    ��߽�ɨ��
*******************************************************************************/
void HuntLine_Left(uint8 i)
{
    short j=0;
    //�ұ��б߽�֮ǰ ���Ϊ����
    Image_Shot.no_left[i]=1;
    Lno_Limit_Tag=0;

    if( Image_Shot.no_left[i+1]==1) //��һ�ж����� ���е㿪ʼѲ��
        j=Image_Shot.begin;
    else    //û���� �ӱ߽翪ʼѲ��
        j=Image_Shot.left[i+1] +13;

    //��ʼ������
    for( ; j>=13 && j<=COL_W-2 ; j--)
    {
       if(  image_new_bin[i][j-2]==Black_Point && image_new_bin[i][j-1]==Black_Point &&
            image_new_bin[i][j+1]==White_Point && image_new_bin[i][j]==White_Point )
       {
           if( j >= Image_Shot.Left_Max){   //��������Χ�ڲ����ҵ�
               Image_Shot.left[i] = j;
               Image_Shot.no_left[i]=0;    //�ҵ��� ��� û�ж���
           }
           else
               Lno_Limit_Tag=1;
           break;
       }
    }

    //������� ������һ֡����
}
/*******************************************************************************
* Function Name:    Search_Mid(void)
* Description  :    ��Ѱ����
*******************************************************************************/
// ����
#define Miss_Judge_Num      4   //�б��ߵ�������ֵ
#define Miss_Judge_Line     40  //�б��ߵ����� ����������������������
#define Miss_Judge_Line1    5  //�б��ߵ����� ����������������������
unsigned char Half_Road_Width57=66; //57����������
unsigned char Old_HRWid=66; //57����������
uint8 Class_NoLine_L =0, Class_NoLine_R =0; //0���Ҳ������� 1�ǲ���������
void Search_Mid(void){
    uint8 i=0;
    int Tag_deltaL=0,Tag_deltaR=0;  //����Ƿ��ҵ��仯��
    int temp_deltaL=0,temp_deltaR=0;  //����Ƿ��ҵ��仯��
    float deltaL_Sum=0,deltaR_Sum=0;    //��¼�仯���ۺ�
    char deltaL_Num=0,deltaR_Num=0;     //��¼�仯����Ŀ
    char MissNum_L=0,MissNum_R=0;   //�����б������
    char MissNum_L1=0,MissNum_R1=0;   //�����б������
    char HaveNum_L=0,HaveNum_R=0;   //�����б������
    char HaveDelta_L=0,HaveDelta_R=0;   //�����б������

    static char MissL_Tag_Y=0,MissR_Tag_Y=0;            //�����б����� ��������ı�־
    if(Image_Shot.MissL_Tag == 3) MissL_Tag_Y=1;        //����ǽ��м䶪�ߵ�״̬ �ͱ�־1 ��ֹ֮�� ���г�����
    else if(Image_Shot.MissL_Tag == 0) MissL_Tag_Y=0;   //��Ϊ���м䶪�ߵĻ� ֮��϶��� û�ж��� ���Եȴ�û�ж��� �ͻָ�
    if(Image_Shot.MissR_Tag == 3) MissR_Tag_Y=1;
    else if(Image_Shot.MissR_Tag == 0) MissR_Tag_Y=0;

    //ȡ��һ֡������ ��Ϊ���ɨ������
    Image_Shot.begin = Image_Shot.mid[57];
    //���²�ɨ������У�����һ֡�ĵײ�һ�¡�
    Image_Shot.mid[58]= Image_Shot.mid[57];
    Image_Shot.mid[59]= Image_Shot.mid[57];
    Image_Shot.left[58]= Image_Shot.left[57];
    Image_Shot.left[59]= Image_Shot.left[57];
    Image_Shot.right[58]= Image_Shot.right[57];
    Image_Shot.right[59]= Image_Shot.right[57];
    //״̬���
    Image_Shot.MissL_Tag = 0;
    Image_Shot.MissR_Tag = 0;
    Image_Shot.deltaL = 1;
    Image_Shot.deltaR =-1;
    Image_Shot.Left_Max=0;
    Image_Shot.Right_Min=187;  //��¼���ұ߽����ֵ
    Class_NoLine_R=0;
    Class_NoLine_L=0;
    Width_Sum = 0;
    DeltaL_Max=0,DeltaR_Min=0;
    //���ߵ�
    Loss_L_Point=0,Loss_R_Point=0;
    //���ߺ��һ��������
    Have_L_Point=0,Have_R_Point=0;


    //��57��ʼѲ��
    for( i=57; i>=2; i--){

        HuntLine_Left(i);   //������� ��ֵΪ0
        HuntLine_Right(i);  //������� ��ֵΪ187

        //�߽� �仯�ʺ����� �б�
        do{
            //�ж� Ѱ�ҵ������ұ߽� �Ƿ���������ֵ ����¼���ұ߽����ֵ   ǰ����û�ж��� ���ߵײ㶪����
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

            //���ж�û���ߣ����� ��û���ҵ��仯��
            if( Image_Shot.no_right[i+1]!=1 && Image_Shot.no_right[i]!=1 ){

                if(Loss_R_Point != 0 && Have_R_Point==0) Have_R_Point = i; //��һ�ζ��ߺ� ��¼��һ�β����߿�ʼ��λ��

                HaveNum_R++;
                //��¼�߽�ı仯�����
                temp_deltaR = Image_Shot.right[i] - Image_Shot.right[i+1];

                //�仯������ �͸���
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

                if(Loss_L_Point != 0 && Have_L_Point==0) Have_L_Point = i; //��һ�ζ��ߺ� ��¼��һ�β����߿�ʼ��λ��

                HaveNum_L++;

                //��¼�߽�ı仯�����
                temp_deltaL = Image_Shot.left[i] - Image_Shot.left[i+1];

                //�仯������ �͸���
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

        //�߽� ��������б�
        do{
            //��¼�������ߵ���Ŀ �����Ѿ��б������ �Ͳ���¼��
            if(Image_Shot.no_left[i]==1 && Image_Shot.MissL_Tag==0){
                if(Loss_L_Point == 0) Loss_L_Point = i; //��һ�ζ��� ��¼���߿�ʼ��λ��

                if(Lno_Limit_Tag == 0)
                    MissNum_L++;
                else
                    MissNum_L1++;

                if( MissNum_L >Miss_Judge_Num){
                    if(i>Miss_Judge_Line)    //ǰ������������ ���ǵײ㶪��
                        Image_Shot.MissL_Tag = 1;
                    else if(i>=Miss_Judge_Line1)   //��Ȼ�����в㶪��
                        Image_Shot.MissL_Tag = 2;
                }
                else if( MissNum_L1 >Miss_Judge_Num){
                    if(i>Miss_Judge_Line)    //ǰ������������ ���ǵײ㶪��
                        Image_Shot.MissL_Tag = 1;
                    else if(i>=Miss_Judge_Line1)   //��Ȼ�����в㶪��
                        Image_Shot.MissL_Tag = 2;

                        Class_NoLine_L = 1;
                }
            }
            //������������
            else{
                MissNum_L=0;
                MissNum_L1=0;
            }

            if(Image_Shot.no_right[i]==1 && Image_Shot.MissR_Tag==0){
                if(Loss_R_Point == 0) Loss_R_Point = i; //��һ�ζ��� ��¼���߿�ʼ��λ��
                if(Rno_Limit_Tag == 0)
                    MissNum_R++;
                else
                    MissNum_R1++;

                if( MissNum_R >Miss_Judge_Num){
                    if(i>Miss_Judge_Line)    //ǰ������������ ���ǵײ㶪��
                        Image_Shot.MissR_Tag = 1;
                    else if(i>=Miss_Judge_Line1)    //��Ȼ�����в㶪��
                        Image_Shot.MissR_Tag = 2;
                }
                else if( MissNum_R1 >Miss_Judge_Num){
                    if(i>Miss_Judge_Line)    //ǰ������������ ���ǵײ㶪��
                        Image_Shot.MissR_Tag = 1;
                    else if(i>=Miss_Judge_Line1)    //��Ȼ�����в㶪��
                        Image_Shot.MissR_Tag = 2;

                        Class_NoLine_R = 1;
                }
            }
            else{
                MissNum_R=0;
                MissNum_R1=0;
            }

            // ���û���� ����״̬�� �м䶪�� ��ζ�ź����������� ���Ϊ״̬3 ���� ��״̬4
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

        //���ݶ������ ��ȡ���� �����ұ߽�
        do{
            //���߽���
            if( Image_Shot.no_left[i]==1 && Image_Shot.no_right[i]==1 ){    //ȫ��
                if( Tag_deltaL ==1 ){   //�б仯�� ���ձ仯�ʲ�
                    Image_Shot.left[i] = Image_Shot.left[i+1] +Image_Shot.deltaL;
                    if(Image_Shot.left[i] >=187) Image_Shot.left[i]=187;
                }
                if( Tag_deltaR ==1 ){
                    Image_Shot.right[i] = Image_Shot.right[i+1] +Image_Shot.deltaR;
                    if(Image_Shot.right[i] <=0) Image_Shot.right[i]=1;
                }
            }
            else if( Image_Shot.no_left[i]==1 && Image_Shot.no_right[i]==0 ){  //ֻ�����
                //ֻ���� ͨ���ұ߲�
                Image_Shot.left[i]= Image_Shot.right[i] - 2* ((int)Half_Road_Width57 - (57-i) ) ;
            }
            else if( Image_Shot.no_right[i]==1 && Image_Shot.no_left[i]==0){ //ֻ���ұ�
                //ֻ���� ͨ����߲�
                Image_Shot.right[i]= Image_Shot.left[i] + 2* ((int)Half_Road_Width57 - (57-i) );
            }

            // �߽��޷�
            if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
            else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
            if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
            else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;

            // ������
            if( Image_Shot.no_right[i]==1 && Image_Shot.no_left[i]==1 ) //ȫ��
            {
                Image_Shot.mid[i] = ( Image_Shot.left[i] + Image_Shot.right[i]) /2;
                //��Ȼ���߶����� ���ǽ�һ����֮ǰ�ı仯��
                if( Tag_deltaL == 1 && Tag_deltaR == 1 ) {}
                else if( Tag_deltaL == 1 )
                    Image_Shot.mid[i]= Image_Shot.left[i] + (int)Half_Road_Width57 - (57-i) ;
                else if( Tag_deltaR == 1 )
                    Image_Shot.mid[i]= Image_Shot.right[i] - ((int)Half_Road_Width57 - (57-i) ) ;
            }
            else if( Image_Shot.no_right[i]==1 )  //ֻ���� ͨ����߲�
                Image_Shot.mid[i]= Image_Shot.left[i] + (int)Half_Road_Width57 - (57-i) ;
            else if( Image_Shot.no_left[i]==1 )   //ֻ���� ͨ���ұ߲�
                Image_Shot.mid[i]= Image_Shot.right[i] - ((int)Half_Road_Width57 - (57-i) ) ;
            else
                Image_Shot.mid[i] = ( Image_Shot.left[i] + Image_Shot.right[i]) /2;

            //���ߵ��˲� �ӵײ�����
            Image_Shot.mid[i]=Centerline_kalman(Image_Shot.mid[i],Image_Shot.mid[i+1]);

            // ���� �޷�
            if(Image_Shot.mid[i]<=1 ) Image_Shot.mid[i]=2;
            else if (Image_Shot.mid[i]>=187) Image_Shot.mid[i]=186;

            //ȫ���߽�ĵڶ��β��� ��������
            if( Image_Shot.no_left[i]==1 && Image_Shot.no_right[i]==1 ){    //ȫ��
                if( Tag_deltaL ==0 ){   //�ޱ仯�� �������߲���
                    Image_Shot.left[i] = Image_Shot.mid[i] - ((int)Half_Road_Width57 - (57-i) );
                }
                if( Tag_deltaR ==0 ){
                    Image_Shot.right[i] = Image_Shot.mid[i] + ((int)Half_Road_Width57 - (57-i) );
                }

                // �߽��޷�
                if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
                else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
                if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
                else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;
            }

        }while(0);

        //�����б�,���б����ǵ�
         if( Image_Shot.Junction_Enable==1 &&( i <=40 && i>=15 ) && Image_Shot.class == Keep ){     //ǰ��û��ʶ������
             int j=0;
             char Tag_W_Jump=0,Tag_B_Jump=0;
             Y_point=0;
             X_point=0;
             X_point1=0;
             //����߽� ���ұ���
             for( j=Image_Shot.left[i]-10; (j<=Image_Shot.right[i]+10 && j<=BEnd_R) ; j++)    //���ܳ��� �������ȥ��
             {
                 if(j<=BEnd_L) j=BEnd_L;
                 // ��û�ҵ�ʱ�����ҵ� �׿� ����ȥ�Һڿ�
                 //  ǰ����
                 if( image_new_bin[i-2][j-5]==White_Point && image_new_bin[i-2][j-3]==White_Point &&
                     image_new_bin[i-1][j-5]==White_Point && image_new_bin[i-1][j-3]==White_Point &&
                     image_new_bin[i-2][j-1]==White_Point && image_new_bin[i-2][j]==White_Point &&
                     image_new_bin[i-2][j+1]==Black_Point && image_new_bin[i-2][j+2]==Black_Point && //����Һ�
                     Tag_W_Jump == 0 && Tag_B_Jump == 0 )
                 {
                     Y_point = i-2;
                     X_point = j;
                     Tag_W_Jump = 1;
                 }
                 if(  //image_new_bin[i-2][j-1]==Black_Point && image_new_bin[i-2][j]==Black_Point &&
                      image_new_bin[i-1][j+1]==White_Point && image_new_bin[i-1][j+2]==White_Point && //����Ұ�
                      image_new_bin[i-2][j+4]==White_Point && image_new_bin[i-2][j+6]==White_Point && //����Ұ�
                      image_new_bin[i-2][j+1]==White_Point && image_new_bin[i-2][j+2]==White_Point && //����Ұ�
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

         // ��¼������仯
         temp_deltaR = Image_Shot.right[i] - Image_Shot.right[i+1];
         temp_deltaL = Image_Shot.left[i]  - Image_Shot.left[i+1] ;
         if(temp_deltaL>DeltaL_Max) DeltaL_Max= temp_deltaL;
         if(temp_deltaR<DeltaR_Min) DeltaR_Min= temp_deltaR;

        //��ȡbegin width
        Image_Shot.width[i] = ( Image_Shot.right[i] - Image_Shot.left[i] );
        Width_Sum +=Image_Shot.width[i];
        Image_Shot.begin = Image_Shot.mid[i];

        //�鿴 �Ƿ�����Ѳ������
        if( image_new_bin[i-1][ Image_Shot.mid[i] ] ==Black_Point && image_new_bin[i-2][ Image_Shot.mid[i] ] ==Black_Point ) break; //�е�ǰ�����Ǻڵ�
        if( image_new_bin[i][ Image_Shot.mid[i] ] ==Black_Point ) break; //�е����Ǻڵ�
        if( Image_Shot.mid[i]<=Image_Shot.left[i+1] || Image_Shot.mid[i]>=Image_Shot.right[i+1]) break;
        if( Image_Shot.right[i] <= Image_Shot.left[i] )  break;     //�ұ�С�����
        if( Image_Shot.right[i] <= Image_Shot.Left_Max )  break;    //�ұ�С���������
        if( Image_Shot.Right_Min <= Image_Shot.left[i] )  break;    //��ߴ����ұ�����
        if( Image_Shot.mid[i] <= Image_Shot.Left_Max )  break;      //�м�С���������
        if( Image_Shot.Right_Min <= Image_Shot.mid[i] )  break;     //�м�����ұ�����
        if( Image_Shot.mid[i] <=1 || Image_Shot.mid[i] >=186 ) break;  //���߳�����
    }

    Image_Shot.lastline=i;

    //�������о���
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
            ){ //���Ҷ�û�� ���м䶪�ߵı�־

            if( ( Image_Shot.MissL_Tag == 1 && Image_Shot.MissR_Tag == 1)
              ){    //һ��ȫ�� һ���м� //б�ʲ��� �������м� ��������

                    Image_Shot.class=Junction;
            }
            else if( ( Image_Shot.MissL_Tag == 1 && Image_Shot.MissR_Tag == 4)
                   ){   //ȫ���� �����ұ����ҵ��� //�������м� ��������
                Image_Shot.class=Junction;

            }
            else if( ( Image_Shot.MissL_Tag == 4 && Image_Shot.MissR_Tag == 1)
                   ){   //ȫ���� ����������ҵ��� //�������м� ��������
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


    //����������� �͸��ݺ��漸��û���ߵ� ��ǰ���еĶ���
    if(Image_Shot.MissL_Tag == 4 && Image_Shot.MissR_Tag == 4){ //���߶������ҵ� ��Ҫ������
        for( i = Image_Shot.lastline+1; (Image_Shot.no_left[i]==0||Image_Shot.no_right[i]==0) ;i++);   //���ҵ�ȫ���ߵ���

        for( ; i<=57; i++){ //�Ӷ��ߵ��� ��ʼ��57��
            if( Image_Shot.no_left[i]==1 && Image_Shot.no_right[i]==1 ){ //������ ������һ�����߲�
                Image_Shot.mid[i]=Image_Shot.mid[i-1];
            }
        }
    }
    if( Image_Shot.MissL_Tag == 4){ //�����״̬4 ˵���ǵײ㶪�� ���Ǻ�������
        for( i = Image_Shot.lastline+1; Image_Shot.no_left[i]==0 ;i++);   //���ҵ����ߵ���

        for( ; i<=57; i++){ //�Ӷ��ߵ��� ��ʼ��57��
            if( Image_Shot.no_left[i]==1 ){ //������ �������߲�
                Image_Shot.left[i] = Image_Shot.mid[i] - ((int)Half_Road_Width57 - (57-i) );
                // �߽��޷�
                if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
                else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
            }
        }
    }
    if( Image_Shot.MissR_Tag == 4){ //�����״̬4 ˵���ǵײ㶪�� ���Ǻ�������
        for( i = Image_Shot.lastline; Image_Shot.no_right[i]==0 ;i++);

        for( ; i<=57; i++){ //�Ӷ��ߵ��� ��ʼ��57��
            if( Image_Shot.no_right[i]==1 ){ //������ �������߲�
                Image_Shot.right[i] = Image_Shot.mid[i] + ((int)Half_Road_Width57 - (57-i) );
                // �߽��޷�
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
* Description  :    ֱ���Ұ����ߣ��ҵ��˾ͱ��ͼƬ��ϢΪ����
*******************************************************************************/
void Find_Carport(void)
{
    uint8 i=0,JumpCount=0,LastCol=0;
    uint8 Ending_Row=0;

    //�ҵ������߿�ʼ������
    //������ʩ ��ֹ�Ҳ���
    if( !(image_new_bin[57][75] & image_new_bin[57][90] & image_new_bin[57][105]) ) //����Ҫ����ڰ�������ж�
        Ending_Row=57;

    for(i=57; i>10; i--) //�����������Χȥ�Ұ�����,���ҵ��ڿ�
    {
        if( !image_new_bin[i][90] & !image_new_bin[i+1][90] )//40�У������м�  �жϱ�׼�� �ڰ�����
        {
            Ending_Row=i;
            break;
        }
        if( !image_new_bin[i][100] & !image_new_bin[i+1][100] )//�ұ�һ��
        {
            Ending_Row=i;
            break;
        }
        if( !image_new_bin[i][80] & !image_new_bin[i+1][80] )//���һ��
        {
            Ending_Row=i;
            break;
        }
        if( !image_new_bin[i][70] & !image_new_bin[i+1][70] )//�����һ��
        {
            Ending_Row=i;
            break;
        }
        if( !image_new_bin[i][110] & !image_new_bin[i+1][110] )//���ұ�һ��
        {
            Ending_Row=i;
            break;
        }
    }


    //��ʼ���� ���ҵ��İ����ߵ� ������ͨ������ ��Ч������
    for(i=30; i<150; i++)
    {
      if(image_new_bin[Ending_Row-2][i]!=image_new_bin[Ending_Row-2][i-1])
      {
        if( (int)(i - LastCol)  > 2 )   //��Ч���䣺����������С��ĳ��ֵ�������������������м�������̫Զ�԰ɣ�/*ʶ�����ֵ*/
            JumpCount++;                //���Ϊ��7�����ҵ��а�����

        LastCol=i;
      }

      if(JumpCount>8 && Ending_Row>=End_Row )//��Ч������,���ڷ�Χ��
      {
        Image_Shot.class=Carport;    //�ҵ�������
        return;
      }
      else if(JumpCount>8){
        Image_Shot.class=Road;    //�ҵ������ˣ����ǻ�������Ч�ж���Χ����Ϊֱ��
        return;
      }
    }

}

/***************************************************************************************************************************
* Function Val:                                 ����ִ��ͼƬ�����������������Ĵ���
****************************************************************************************************************************/

/*******************************************************************************
* Function Name: Image_Classify(void);
* Description  : ͼƬ����
* �߼���
*   1.��������ֵ���׿�ͺ��߽�ֹ�У������߽߱���
*   2.��������ֵ�ж��������
* ��Ҫ�������ο����º궨��
*******************************************************************************/
void Image_Classify(void){

    //���������ֵ
    for(char i=0;i<6;i++)//����ֵ�������
        Image_Shot.Block_White[i]=0;
    for(char i=0;i<5;i++)//����ֵ�������
        Image_Shot.Black_End[i]=0;

    /* ��������������ȡ*/
    //���߽�ֹ�м�⣺|0 1 2 3 4|
    //ȡ
    for (char i = ROW_H-1; i >= 3 ; i--)//��[2]���߽�����
    {
        Image_Shot.Black_End[2]++;
        if( image_new_bin[i-1][BEnd_M] == Black_Point &&
            image_new_bin[i-2][BEnd_M] == Black_Point )
            break;//���������Ǻ�ɫ
    }
    for (char i = ROW_H-1; i >= 3 ; i--)//��[0]���߽�����
    {
        Image_Shot.Black_End[0]++;
        if( image_new_bin[i-1][BEnd_L] == Black_Point &&
            image_new_bin[i-2][BEnd_L] == Black_Point )
            break;//���������Ǻ�ɫ
    }
    for (char i = ROW_H-1; i >= 3 ; i--)//��[4]���߽�����
    {
        Image_Shot.Black_End[4]++;
        if( image_new_bin[i-1][BEnd_R] == Black_Point &&
            image_new_bin[i-2][BEnd_R] == Black_Point )
            break;//���������Ǻ�ɫ
    }
    for (char i = ROW_H-1; i >= 3 ; i--)//����[1]
    {
        Image_Shot.Black_End[1]++;
        if( image_new_bin[i-1][BEnd_ML] == Black_Point &&
            image_new_bin[i-2][BEnd_ML] == Black_Point )
            break;//���������Ǻ�ɫ
    }
    for (char i = ROW_H-1; i >= 3 ; i--)//����[3]
    {
        Image_Shot.Black_End[3]++;
        if( image_new_bin[i-1][BEnd_MR] == Black_Point &&
            image_new_bin[i-2][BEnd_MR] == Black_Point )
            break;//���������Ǻ�ɫ
    }

    //�׿���
    for(int i=1;i<30;i++)//��30��
    {
      for(int j=62;j<124;j++)//�м�62��
        if(image_new_bin[i][j] == White_Point)
            Image_Shot.Block_White[1]++;//�׿�Ũ�ȼ�1
    }
    for(int i=30;i<59;i++)//��30
    {
      for(int j=1;j<62;j++)//��30
        if(image_new_bin[i][j] == White_Point)
            Image_Shot.Block_White[3]++;

      for(int j=62;j<124;j++)//��30
        if(image_new_bin[i][j] == White_Point)
            Image_Shot.Block_White[4]++;

      for(int j=124;j<186;j++)//��30
        if(image_new_bin[i][j] == White_Point)
            Image_Shot.Block_White[5]++;
    }

    /* ��������Ԫ���ж�
     * ������ж��Ƕ����ģ���Ϊ�����ǳ����ء�
     * Բ�����ж�ֻ�ǳ����ģ���Ѱ�����л��һ���жϡ�
     */
    //�ȳ�ʼ��״̬
    Image_Shot.class = Keep;

    //���ҳ���
    if( Car_Go_Tag ==0 ){   //��û�����ͱ��ֳ���
        Image_Shot.class=Carport;
        return;
    }
    static uint8 First_Frame=1;
    if( First_Frame==1 ) {  //����ǵ�һ֡����Ȼ�ǳ���ֱ�ӷ���
        Image_Shot.class=Carport;
        First_Frame =0;
        return;
    }
    if( C_Car.state != Carport && Image_Shot.Carport_Enable ==1 ){
        Find_Carport();
        if( Image_Shot.class == Carport ) return;    //�ҵ������˾�ֱ�ӷ��أ�������ԥ
        if( Image_Shot.class == Road ) return;    //�ҵ�ֱ���˾�ֱ�ӷ��أ�������ԥ
    }


    //ʶ��ʹ���ж�
    if( Image_Shot.Carport_Enable == 0){ //�������Ѱ�Ҳ�ʹ��
        static uint8 Search_Count1=0;
        Search_Count1++;     //����
        if(Search_Count1>50){//����50֡����ʹ�ܣ���������
            Search_Count1=0;
            Image_Shot.Carport_Enable = 1;
            Image_Shot.Carport_Num++;     //�������+1
        }
    }
    if( Image_Shot.CircleL_Enable == 0){ //�����Բ��Ѱ�Ҳ�ʹ��
        static uint8 Search_Count2=0;
        Search_Count2++;     //����
        if(Search_Count2> 200){//����30֡����ʹ�ܣ���������
            Search_Count2=0;
            Image_Shot.CircleL_Enable = 1;
        }
    }
    if( Image_Shot.CircleR_Enable == 0){ //�����Բ��Ѱ�Ҳ�ʹ��
        static uint8 Search_Count3=0;
        Search_Count3++;     //����
        if(Search_Count3> 200){//����30֡����ʹ�ܣ���������
            Search_Count3=0;
            Image_Shot.CircleR_Enable = 1;
        }
    }

    //�����б����� ��������ı�־
    static char MissL_Tag_P=0,MissR_Tag_P=0;
    if( Image_Shot.MissL_Tag == 2 &&    //������� �ұ߶��߻��߶����ҵ���б�� ��������������
        ( (Image_Shot.MissR_Tag == 1) || ( Image_Shot.MissR_Tag == 4 ) ) )
        MissR_Tag_P=1;
    else if( (Image_Shot.MissR_Tag == 0 || Image_Shot.MissL_Tag == 0) && Image_Shot.lastline<=50)  //һ�߲����� �ͽ����ж�������
        MissR_Tag_P=0;

    if( Image_Shot.MissR_Tag == 2 &&    //������� ��߶��߻��߶����ҵ���б�� ��������������
        ( (Image_Shot.MissL_Tag == 1) || ( Image_Shot.MissL_Tag == 4 ) ) )
        MissL_Tag_P=1;
    else if( (Image_Shot.MissL_Tag == 0 || Image_Shot.MissR_Tag == 0) && Image_Shot.lastline<=50)  //һ�߲����� �ͽ����ж�������
        MissL_Tag_P=0;


    //�����ұ߽��ߺ����ߣ���Ѳ��
    Search_Mid();

    /*ÿ������ ���1880
     *      |0 1 2|
     *      |3 4 5|
     * ���������Ǻ��ĵ���������չ���жϡ�
     * �ж���ͨ�����ð׵����������жϣ��Ƿ��п�����������
     * Ȼ���� ��ɫ��ֹ�и����жϣ���ɫ���������������������ö���������ж��Ƿ��ǳ����ġ�*/
    if(  (Image_Shot.Black_End[2] > Out_Length) ||  (Image_Shot.Black_End[0] > Out_Length) ||  (Image_Shot.Black_End[4] > Out_Length) ){ //�жϺ�����ͨ����δƫ��������

        char Left_Block=0,Right_Block=0,Forward_Block=0;    //��־λ

        //ֻ��������� �����жϵļ�ֵ
        if(Image_Shot.Block_White[3]>Block_Value)  Left_Block = 1;  //���ͱ�־1
        if(Image_Shot.Block_White[5]>Block_Value)  Right_Block = 1;
        if(Image_Shot.Block_White[1]>Block_Value)  Forward_Block = 1;

        //��û���� ����ֱ��
        if(Image_Shot.class == Ramp){
            //�µ�ʶ�� ��Ѳ�ߵĿ�ȼ������
        }
        else if( (Image_Shot.MissL_Tag ==0 || Image_Shot.MissL_Tag ==2 || Image_Shot.MissL_Tag ==3) &&
                 (Image_Shot.MissR_Tag ==0 || Image_Shot.MissR_Tag ==2 || Image_Shot.MissR_Tag ==3) ){
            Image_Shot.class=Road;
            if( Image_Shot.Circle_In_Enable == 1 ){
                Image_Shot.Circle_Mid_Enable=1;
                Half_Road_Width57 = Add_Circ_In;
            }
        }
        //ֱ������Բ��
        else if( Image_Shot.MissL_Tag ==0    &&  //���߹��� ���û����
                 Image_Shot.deltaL <= 2 &&
                 Image_Shot.Circle_Find_Enable==1 ){               //�м䳩ͨ���裬���� ֱ��

            Image_Shot.class=Road;

            if( Image_Shot.CircleR_Enable == 1 && C_Car.state !=CircleR ){
                //���ж�Բ��
                if(  (Image_Shot.MissR_Tag ==4 || Image_Shot.MissR_Tag ==1 )        //һ��û�� һ�ߵ��¶�
                     ){

                    //���... ��Բ��ʹ��
                    if(Class_NoLine_R == 0 && Image_Shot.lastline <=  Road_Len   && Right_Block == 1 &&
                       Image_Shot.deltaR <=-O_delat && Image_Shot.Circle_In_Enable ==0){
                        Image_Shot.Circle_In_Enable =1;
                    }
                    //���... ����ʽ����Բ��
                    else if( Image_Shot.Circle_Mid_Enable ==1   //
                             ){
                        Image_Shot.Circle_In_Enable =0;
                        Image_Shot.Circle_Mid_Enable =0;
                        Image_Shot.class= CircleR;                //  �һ�
                        Half_Road_Width57 = Old_HRWid;
                    }

                }
            }

        }
        //ֱ������Բ��
        else if( Image_Shot.MissR_Tag ==0    &&  //���߹��� �ұ�û����
                 Image_Shot.deltaR >=-2 &&
                 Image_Shot.Circle_Find_Enable==1 ){               //�м䳩ͨ���裬���� ֱ��

            Image_Shot.class=Road;

            if( Image_Shot.CircleL_Enable == 1  && C_Car.state !=CircleL ){
                //���ж�Բ��
                if(  (Image_Shot.MissL_Tag ==4 || Image_Shot.MissL_Tag ==1 )          //һ��û�� һ�ߵ��¶�
                     ){

                    //���... ��Բ��ʹ��
                    if( Class_NoLine_L == 0 && Left_Block == 1 && Image_Shot.lastline <=  Road_Len   &&
                        Image_Shot.deltaL >= O_delat && Image_Shot.Circle_In_Enable ==0){
                        Image_Shot.Circle_In_Enable =1;
                    }
                    //���... ����ʽ����Բ��
                    else if( Image_Shot.Circle_Mid_Enable ==1   //
                             ){
                        Image_Shot.class=CircleL;                //�������ͨ������  ��
                        Image_Shot.Circle_In_Enable =0;
                        Image_Shot.Circle_Mid_Enable =0;
                        Half_Road_Width57 = Old_HRWid;
                    }

                }
            }

        }
        //�б�������
        else if( Image_Shot.MissL_Tag ==1 &&    //�� �Ҷ����ҵ���б��
                ( (Image_Shot.MissR_Tag == 4 ) )  &&    //(Image_Shot.MissR_Tag == 1) ||
                 C_Car.state == CurveR && MissR_Tag_P==1   &&            //����״̬ �������������ж�
                 //(Image_Shot.Black_End[1]>= Image_Shot.Black_End[2] )  && //�м�>3  3>4
                 (Image_Shot.Black_End[2]>= Image_Shot.Black_End[3] )  && //�м�>3  3>4
                 (Image_Shot.Black_End[3]>= Image_Shot.Black_End[4] )  && Image_Shot.Back_Find_Enable==1  ){

                Image_Shot.class=BackR;    //���� ������
        }
        //�б�������
        else if( Image_Shot.MissR_Tag ==1 &&   //�Ҷ� �󶪺��ҵ���б��
                 ( (Image_Shot.MissL_Tag ==4 ) )  &&    // (Image_Shot.MissL_Tag == 1) ||
                 C_Car.state == CurveL && MissL_Tag_P==1   &&            //����״̬ �������������ж�
                 //(Image_Shot.Black_End[3]>= Image_Shot.Black_End[2] )  && //�м�>3  3>4
                 (Image_Shot.Black_End[2]>= Image_Shot.Black_End[1] )  && //�м�>1  1>0
                 (Image_Shot.Black_End[1]>= Image_Shot.Black_End[0] )  && Image_Shot.Back_Find_Enable==1  ){

                Image_Shot.class=BackL;    //���� ������
        }
        else if( Image_Shot.class==Junction ){
            //���� ʶ�� ��Ѳ������
        }
        //����
        else if( Image_Shot.MissL_Tag ==1 && Image_Shot.MissR_Tag ==0 &&
                (Image_Shot.Black_End[2]<= Image_Shot.Black_End[1] )   &&
                (Image_Shot.Black_End[3]<= Image_Shot.Black_End[2] )
                )   {   // ����
            Image_Shot.class=CurveL;
        }
        //����
        else if( Image_Shot.MissR_Tag ==1 && Image_Shot.MissL_Tag ==0 &&
                (Image_Shot.Black_End[2]<= Image_Shot.Black_End[3] )   &&
                (Image_Shot.Black_End[1]<= Image_Shot.Black_End[2] )
                )  {   //  ����
            Image_Shot.class=CurveR;
        }
    }
    else{//�жϺ���δ��ͨ��ƫ��������
        Image_Shot.class =Outroad; //���ʾ��������
    }

}



/*******************************************************************************
* Function Name: Car_Mealy(void);
* Description  : ����״̬������
* �߼����ο�״̬ת��ͼ
* ��Ҫ������״̬ת��ͼ
* �ر�ע�⣬keep��outroad��ʹ��
*******************************************************************************/

static char CircleL_O_Tag=0,CircleR_O_Tag=0;
uint8 Flash_Wait_Num = 5;
void Car_Mealy(road_state Next_State){
    //һ������� Next_State���ǵ�ǰͼƬ״̬�Ľ��
    //��¼��ǰ��ȥ��֡��
    static char Flash_Num=0;
    Flash_Num++;
    C_Car.Old_state = C_Car.state;

    //�����ǰ״̬�����´�״̬һ�£���ֱ�ӽ���
    if(Next_State == C_Car.state){
        return;
    }
    else if( Next_State == Outroad ){ // Next_State == Keep ||
        return;
    }
    //״̬����ȣ��Ҳ���keep��outroad״̬����֤����Ҫ�б��Ƿ�ı䵱ǰ״̬
    else{
        //��ǰ״̬�ǳ���
        if( C_Car.state == Carport){
            //���ж��ǵڼ��γ���
            if(Image_Shot.Carport_Num == 0){    //����
                //����һ��״̬������ֱ��Ҳ���������
                if( ( Next_State == Road || Next_State == CurveR || Next_State == CurveL ) && (Image_Shot.lastline <=KU_lastline)  ) //
                {
                    //��ǰ״̬���ɵ���һ״̬
                    C_Car.state = Next_State;
                    Flash_Num=0;
                    //���û�п�����������ı�����״̬
                    if( Car_Go_Tag ==1 ) {
                        Image_Shot.Carport_Enable =0; // �ر�ʹ�ܣ���ʱ���ڲ��ڲ��ҳ���
                        Image_Shot.Junction_Tag= !Image_Shot.Junction_Tag;  //�ı����淽��
                    }
                }

            }
            else if(Image_Shot.Carport_Num == 1){   //�ڶ���
                //����һ��״ֻ̬������ֱ��
                if( Next_State == Road )
                {
                    C_Car.state = Next_State;
                    Flash_Num=0;
                    //���û�п�����������ı�����״̬
                    if( Car_Go_Tag ==1 ) {
                        Image_Shot.Carport_Enable =0; // �ر�ʹ�ܣ���ʱ���ڲ��ڲ��ҳ���
                        Image_Shot.Junction_Tag= !Image_Shot.Junction_Tag;  //�ı����淽��
                    }
                }
            }
            else if(Image_Shot.Carport_Num == 2){   //���
                //����һ��״ֻ̬���������
//                Stop_Num=1;
            }
            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;
        }
        //��ǰ״̬������
        else if( C_Car.state == Junction ){
            if( Flash_Num>Flash_Wait_Num ){
                //��ǰ״̬���ɵ���һ״̬
                C_Car.state = Road;
                Flash_Num=0;
//                BuzzerQuiet;
            }

            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;
        }
        //��ǰ״̬����Բ��
        else if( C_Car.state == CircleL){
            //����һ��״ֻ̬������ֱ��
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
                //��ǰ״̬���ɵ���һ״̬
                C_Car.state = Next_State;
                Flash_Num=0;
            }
            else if( Next_State == CurveL && CircleL_O_Tag==0 && Flash_Num> Flash_Wait_Num && Image_Shot.Circle_Out_Enable !=1 ){
                //��Ϊ�뻷�� ��˳�����ʹ��
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
        //��ǰ״̬����Բ��
        else if( C_Car.state == CircleR){
            //����һ��״ֻ̬������ֱ��
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
                //��ǰ״̬���ɵ���һ״̬
                C_Car.state = Next_State;
                Flash_Num=0;
            }
            else if( Next_State == CurveR && CircleR_O_Tag==0 && Flash_Num> Flash_Wait_Num && Image_Shot.Circle_Out_Enable !=1 ){
                //��Ϊ�뻷�� ��˳�����ʹ��
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
        //��ǰ״̬��������
        else if( C_Car.state == BackL ){
            if( Flash_Num>Flash_Wait_Num && Image_Shot.Back_Out_Enable == 1 ){
                //��ǰ״̬���ɵ���һ״̬
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
                Image_Shot.Back_Out_Enable=1;   //�ж��ɹ���ʱ����ǳ�������ʱ��
            }
            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;
        }
        //��ǰ״̬��������
        else if( C_Car.state == BackR ){
            if( Flash_Num>Flash_Wait_Num && Image_Shot.Back_Out_Enable == 1){
                //��ǰ״̬���ɵ���һ״̬
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
                Image_Shot.Back_Out_Enable=1;   //�ж��ɹ���ʱ����ǳ�������ʱ��
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
                //��ǰ״̬���ɵ���һ״̬
                C_Car.state = Road;
                Flash_Num=0;
//                BuzzerQuiet;
            }
            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;
        }
        else if (Next_State == Keep) return;
        //��ǰ״̬�����
        else if( C_Car.state == CurveL || C_Car.state == CurveR ){

            //�����һ��״̬��Բ������˵����ֱ��
            if( Next_State == CircleL || Next_State == CircleR ) {
                C_Car.state = Road;
            }
            //����ֱ�ӽ�����һ�ε�״̬
            else
                C_Car.state = Next_State;

            //�������ʱ�� ��Բ��״̬
            Image_Shot.Circle_Out_Enable =0;
            Image_Shot.Circle_In_Enable =0;
            Image_Shot.Circle_Mid_Enable =0;

            Flash_Num=0;
        }
        //����״̬ ֱ��
        else {
            //��ֱ�ӽ�����һ�ε�״̬
            C_Car.state = Next_State;
            Flash_Num=0;
        }
    }
}



/*******************************************************************************
* Function Name: Mid_Mealy(road_state Next_State);
* Description  : ����״̬������
* �߼����ο�״̬ת��ͼ
* ��Ҫ������״̬ת��ͼ
*******************************************************************************/
void Mid_Mealy(road_state Next_State){
    // ����Ѳ�ߺ�ͼƬ���࣬������״̬����״̬������
    if( Next_State == CircleL ){
        //���״̬��Բ�� ��������
        Image_Shot.Mid_class=CurveL;
    }
    else if( Next_State == CircleR ){
        //���״̬��Բ�� ��������
        Image_Shot.Mid_class=CurveR;
    }
    else if( Next_State == Junction ){   //ֱ�Ӹ�������ı�־������ǰ��ͼƬ���͸�ֵ
        if(Image_Shot.Junction_Tag == 0){
            Image_Shot.Mid_class =CurveL;
        }
        else {
            Image_Shot.Junction_Tag = 1;
            Image_Shot.Mid_class =CurveR;
        }
    }
    else if( Next_State == BackL ){   //������ ����
        Image_Shot.Mid_class = CurveL;

        if( Image_Shot.Back_Out_Enable==1 ){//�ó�ȥ��
            Image_Shot.Mid_class=CurveR;    //����
        }

    }
    else if( Next_State == BackR ){   //������ ����
        Image_Shot.Mid_class = CurveR;

        if( Image_Shot.Back_Out_Enable==1 ){//�ó�ȥ��
            Image_Shot.Mid_class=CurveL;    //����
        }

    }
    else if( Next_State == Carport ){    //���⣬�ж��ǵڼ��γ���

        if(Image_Shot.Carport_Num == 0){
            //����
            if(Image_Shot.Carport_Direction ==0)
                Image_Shot.Mid_class = CurveR;
            else
                Image_Shot.Mid_class = CurveL;

        }
        else if(Image_Shot.Carport_Num == 1){
            //����ֱ��
            Image_Shot.Mid_class = Road;
        }
        else if(Image_Shot.Carport_Num >= 2){
            Image_Shot.Carport_Num = 2;
            //���
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
* Description  : ���ߴ�����
* �߼���
*   1.���ж�Ԫ�أ�������ض�Ԫ�ؾ������Ӧ��Image_Class �������keep��out״̬��ͬʱ��ȡmid����
*   2.����Image_Class�����ı�Car״̬�������keep��out���򲻸ı䡣
*   3.����Car״̬�����ı�Mid״̬�������midֻ��ֱ���������
*   4.����mid״̬����Image_Class�Ƿ�Ϊkeep��out�����������������
* ��Ҫ������ִ������״̬�����ʱ�����ߵĲ�����
*******************************************************************************/
//���߽�ֹ����ǰ�涨�����
//#define BEnd_M

float Weight_Auto[100]={   //�����õ�Ȩֵ
    // ԭ�ȵ�Ȩֵ
    2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,
    5,5,5,5,5,5,5,5,5,5,
    4,4,4,4,4,4,4,4,4,4,
    3,3,3,3,3,3,3,3,1,1,    //0~59

    // Ϊ����Ӧ��̬ǰհ������Ȩֵ
    0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,
    0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,
    0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,
    1,1,1,1,1,1,1,1,1,1     //60~99
};

#define Delta_Range 5       //�仯���޷�
//#define Curve_coco  3       //��� �������� �ں����ڲ������� ���� ����Ԫ���޸Ĳ���ϵ��
void Mid_Proc(void){
    //��� ��������
    float Curve_coco =1;
    //Ȩֵ����
    float   CenterSum=0,WeightSum=0.1;
    //���߱仯�ʣ��ݽ��棬�������εĲ�ֵ
    float temp_delta[60]={0},Curve_Delta=0;
    int temp_Mid = 0;

    Image_Classify();               //�ı� Image_Shot.class �� mid
    Car_Mealy(Image_Shot.class);    //�ı� C_Car.state
    Mid_Mealy(C_Car.state);         //�ı� Image_Shot.Mid_class

    //�����out ��ֱ�ӽ��� ���ı�Mid_Out ����Ҫ���� Ѳ�߽��
    if( Image_Shot.class == Outroad ){
        //����һ֡�ĵײ�һ�¡�
        Image_Shot.mid[57] = Image_Shot.mid[58];
        Image_Shot.left[57]= Image_Shot.left[58];
        Image_Shot.right[57]= Image_Shot.right[58];
        return;
    }

    //���߲��� ����
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


    //����� Ԫ�ز��� �����ò��� ��������
    if(C_Car.state != Image_Shot.Mid_class){

            //��������
            //�ײ㶪��  ������һ֡Ϊ���
            if( Image_Shot.MissL_Tag ==1 && Image_Shot.MissR_Tag ==1 )
                temp_Mid = Image_Shot.mid[58];
            else
                //�Ա�֡ͼ�������Ϊ���
                temp_Mid = Image_Shot.mid[57];

            //����� Ԫ�ز��� �����ò���
            Curve_coco =2;
            if( C_Car.state == Ramp) {  //�µ���road
//                BuzzerRing;
            }
            else if( C_Car.state == Junction) {  //���油����
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
            else if( C_Car.state == CircleL || C_Car.state == CircleR) { //Բ��������
//                BuzzerRing;
                if(Image_Shot.Circle_Out_Enable ==1 ){  //��Բ��
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
                else if( CircleL_O_Tag ==1 || CircleR_O_Tag ==1) { //�Ѿ�����Բ��
                    goto Final_Out;
                }
                else if( CircleR_O_Tag ==0 && CircleL_O_Tag==0 ){   //���ڽ�Բ��
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
            else if( C_Car.state == Carport &&  Image_Shot.Carport_Num == 2) {  //����һ��
                Curve_coco = 8;
            }
            else if( C_Car.state == BackL ||  C_Car.state == BackR ) {  //����������
//                BuzzerRing;
                Curve_coco = 2;
            }

            //����������� б�ʺ�����
            if( Image_Shot.Mid_class == CurveL){
                //��ĸȡС��
                float high_t=Image_Shot.Black_End[1];
                if(Image_Shot.Black_End[0] < high_t)
                    high_t = Image_Shot.Black_End[0];
                if(high_t<=10) high_t = 10;

                //�������ߵ����䣬���䲻ͬ�����߲ο���ͬ
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
                //��ĸȡС��
                float high_t=Image_Shot.Black_End[3];
                if(Image_Shot.Black_End[4] < high_t)
                    high_t = Image_Shot.Black_End[4];
                if(high_t<=10) high_t = 10;

                //�������ߵ����䣬���䲻ͬ�����߲ο���ͬ
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


            // �ٸ�������״̬�����ж����������У��������Ĳ��ֽ��в���
            for(uint8 i=56; i>Image_Shot.lastline; i--)
            {
                //����仯��Ϊdelta
                float delta = Image_Shot.mid[i]- Image_Shot.mid[i+1];
                //�仯���޷�
                if( delta>Delta_Range ) delta=Delta_Range;
                else if( delta<-Delta_Range ) delta=-Delta_Range;

                //���ۼƱ仯��
                temp_delta[i] = temp_delta[i+1];

                //����������������� �Ա仯�ʽ�������
                if(Image_Shot.Mid_class == Road){
                    temp_delta[i] += delta;
                }
                else if(Image_Shot.Mid_class == CurveL){   //���� ����������ı仯��
                    if( (delta >=0 )  )
                    {
                        temp_delta[i] += Curve_Delta;
                    }
                    else if( delta <0)  //�������� �ͱ仯��
                        temp_delta[i] +=delta;
                }
                else if(Image_Shot.Mid_class == CurveR){   //����
                    if( (delta <=0 ) )
                    {
                        temp_delta[i] += Curve_Delta;
                    }
                    else if( delta >0)
                        temp_delta[i] +=delta;
                }
            }

            // lastline�޷�
            if(Image_Shot.lastline >=58)
                Image_Shot.lastline=57;
            else if(Image_Shot.lastline <1)
                Image_Shot.lastline=1;

            // ���ݱ仯�� �ı��������
            Image_Shot.mid[57] = temp_Mid;
            for(uint8 i=56; i>Image_Shot.lastline; i--){
                int temp= temp_Mid + (int)temp_delta[i] ;

                //���ߵ��˲� �ӵײ�����
                Image_Shot.mid[i]=Centerline_kalman((float)temp,Image_Shot.mid[i+1]);

                //�����޷�
                if(temp >=186) temp=186;
                else if(temp<=0) temp =1;
                Image_Shot.mid[i] = temp;

            }

            // ���ݲ������� �߽���
            for(uint8 i=57; i>Image_Shot.lastline; i--){
                    if(Image_Shot.no_left[i]==1){
                        Image_Shot.left[i] = Image_Shot.mid[i] - ((int)Half_Road_Width57 - (57-i) );
                        // �߽��޷�
                        if(Image_Shot.left[i]<=0 ) Image_Shot.left[i]=1;
                        else if (Image_Shot.left[i]>=188) Image_Shot.left[i]=187;
                    }

                    if(Image_Shot.no_right[i]==1){
                        Image_Shot.right[i] = Image_Shot.mid[i] + ((int)Half_Road_Width57 - (57-i) );
                        // �߽��޷�
                        if(Image_Shot.right[i]<=0 ) Image_Shot.right[i]=1;
                        else if (Image_Shot.right[i]>=188) Image_Shot.right[i]=187;
                    }
            }

    }
Final_Out:
//ʹ�� ��̬ǰհ
    if(Image_Shot.ToBe >100 ) Image_Shot.ToBe=0;
    else if (Image_Shot.ToBe> 40) Image_Shot.ToBe=40;
    for(int i=ROW_H-3; i>= Image_Shot.lastline ;i--)//LastLine
    {
      CenterSum+=Image_Shot.mid[i]*Weight_Auto[i +Image_Shot.ToBe];
      WeightSum+=Weight_Auto[i +Image_Shot.ToBe];
    }

    Image_Shot.Mid_Out=(CenterSum/WeightSum);


    // �Բ��ߵĴ��� ����һ��������
    if( C_Car.state == Ramp) {  //�µ�������
        if(Image_Shot.Mid_Out<=74)
            Image_Shot.Mid_Out=74;
        else if(Image_Shot.Mid_Out>=114)
            Image_Shot.Mid_Out=114;
    }
    else if( C_Car.state == Junction) {  //���油����
        if(Image_Shot.Mid_class == CurveR){
            if(Image_Shot.Mid_Out<=Steer_Con.goal + Add_Junc_R)
                Image_Shot.Mid_Out=Steer_Con.goal + Add_Junc_R;
        }
        else if(Image_Shot.Mid_class == CurveL){
            if(Image_Shot.Mid_Out>= Steer_Con.goal - Add_Junc_L)
                Image_Shot.Mid_Out=Steer_Con.goal - Add_Junc_L;
        }
    }
    else if( C_Car.state == BackL ) { //������
        if(Image_Shot.Mid_Out<= Steer_Con.goal + Add_Back_L)
            Image_Shot.Mid_Out= Steer_Con.goal + Add_Back_L;
    }
    else if( C_Car.state == BackR ) { //������
        if(Image_Shot.Mid_Out>= Steer_Con.goal - Add_Back_R)
            Image_Shot.Mid_Out= Steer_Con.goal - Add_Back_R;
    }
    else if( C_Car.state == Carport) {  //����ⲹ����
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

    //Բ�� ������
    if(Image_Shot.Mid_Out<(Steer_Con.goal - O_er) && C_Car.state!=CircleL && C_Car.state!=CircleR ){
        //�������ʱ�� ��Բ��״̬
        Image_Shot.Circle_Out_Enable =0;
        Image_Shot.Circle_In_Enable =0;
        Image_Shot.Circle_Mid_Enable =0;

    }
    else if(Image_Shot.Mid_Out>(Steer_Con.goal + O_er) && C_Car.state!=CircleL && C_Car.state!=CircleR ){
        //�������ʱ�� ��Բ��״̬
        Image_Shot.Circle_Out_Enable =0;
        Image_Shot.Circle_In_Enable =0;
        Image_Shot.Circle_Mid_Enable =0;
    }

    /*�����ټ� ·���Ż�
     *1: ǰհΪ0��ʱ�� ������39�е�29��
     *2: �õ���Ȩֵ���� ��ȥ�����ĵ�·�� �����������ұ߽���
     *3: ��������� ��Ҫ�ʵ��������� �����������ұ߽���
     * */

    car_control( 250, Image_Shot.Mid_Out );
}





