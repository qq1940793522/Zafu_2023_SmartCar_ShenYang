/*
 * image.h
 *
 *  Created on: 2021��11��30��
 *      Author: 14033
 */

#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_

/*** �궨�� ****/
#if !defined(MAX)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define Black_Point 0
#define White_Point 255

#define ROW_H        60
#define COL_W        188

//  ��ɫ��ֹ��
#define BEnd_M      94      //  COL_W/2
#define BEnd_L      30
#define BEnd_R      157     //  COL_W-1-BEnd_L
#define BEnd_ML     60      //  COL_W/2 - (COL_W/2 - BEnd_L)/2
#define BEnd_MR     127     //  COL_W/2 + (COL_W/2 - BEnd_L)/2


/*** �����ⲿ���� ***/
extern uint8 THRESHOLD;
extern uint8 image_new_gray_bin[ROW_H][COL_W];  //188*120��ȡ��94*60
extern uint8 image_new_bin[ROW_H][COL_W];   //��ֵ��ͼ������
extern int otsu (unsigned char *image,  int rows, int cols);
extern unsigned char Half_Road_Width57,Old_HRWid;

/* ��̬���� Ԫ�ز��߲��� */
extern uint8 Add_Carport;
extern uint8 Add_Junc_L;
extern uint8 Add_Junc_R;
extern uint8 Add_Back_L;
extern uint8 Add_Back_R;
extern uint8 Add_Circ_In;
extern uint8 Add_Junc_L0;
extern uint8 Add_Junc_R0;

extern uint8 Y_point;
extern uint8 X_point;
extern uint8 X_point1;
extern int Width_Sum;
extern short DeltaL_Max ,DeltaR_Min ;

//���ߵ�
extern uint8 Loss_L_Point,Loss_R_Point;
//���ߺ��һ��������
extern uint8 Have_L_Point,Have_R_Point;
extern uint8 Delta_range;
extern uint32 Width_range;
extern uint8 End_Row;
extern uint8 O_in;
extern uint8 O_out;
extern uint8 O_er;
extern uint8 O_delat;
extern uint8 Flash_Wait_Num;
extern uint8 KU_lastline;
extern uint8 YZ_delta;
/*** �����ⲿ���� ***/
extern void Image_Get(void);            //��ԭʼͼ���ȡͼ��
extern void yuzhiget(void);             //��ѵ���������ȡ��̬��ֵ
extern void Image_binaryzation(void);   //��ֵ������

extern void Mid_Proc(void);

#endif /* CODE_IMAGE_H_ */
