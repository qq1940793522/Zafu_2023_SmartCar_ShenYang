/*
 * SystemInit.c
 *
 *  Created on: 2023��5��23��
 *      Author: 29079
 */

#include "SystemInit.h"

uint8 Car_Go_Tag=1;

Car_State C_Car;    //��ģ״̬���� ���壬c��ģ
//void Car_State_Init(void){
//    C_Car.Yaw=0; C_Car.Roll=0; C_Car.Pitch=0;
//
//    C_Car.state=Carport;
//    C_Car.Old_state = Carport;
//    C_Car.V.Set=50;       //2000
//
//    C_Car.V.Max=65; C_Car.V.Min=35;
//    C_Car.V.goal[0]=C_Car.V.goal[1]=50;
//    C_Car.V.real[0]=C_Car.V.real[1]=0;
//    //0ȡΪ���� 1ȡΪ����
//}



PID_Base Steer_Con;    //������Ʊ��� ����
PID_Base Left_M_Con; PID_Base Righ_M_Con;   //������Ʊ��� ����
//void PID_Struct_Init(void){
//    //PID ������ʼ��
//    Steer_Con.P=0;
//    Steer_Con.D=5;
//    Steer_Con.I=0;
//    Steer_Con.Er_D=0;
//    Steer_Con.Er_I=0;
//    Steer_Con.Er_former=0;
//    Steer_Con.Error=0;
//    Steer_Con.Out=0;
//    Steer_Con.goal=93;
//    Steer_Con.real=93;
//
//    Left_M_Con.P=18;
//    Left_M_Con.D=460;
//    Left_M_Con.I=0;
//    Left_M_Con.Er_D=0;
//    Left_M_Con.Er_I=0;
//    Left_M_Con.Er_former=0;
//    Left_M_Con.Error=0;
//    Left_M_Con.Out=0;
//    Left_M_Con.goal= C_Car.V.goal[0];
//    Left_M_Con.real=0;
//
//    Righ_M_Con.P=18;
//    Righ_M_Con.D=460;
//    Righ_M_Con.I=0;
//    Righ_M_Con.Er_D=0;
//    Righ_M_Con.Er_I=0;
//    Righ_M_Con.Er_former=0;
//    Righ_M_Con.Error=0;
//    Righ_M_Con.Out=0;
//    Righ_M_Con.goal= C_Car.V.goal[1];
//    Righ_M_Con.real=0;
//}



Image_Info Image_Shot;  //һ֡ͼ����Ϣ ����
void Image_Info_Init(void){
    Image_Shot.class=Carport;
    Image_Shot.Mid_class=Road;
    Image_Shot.begin=94;
    Image_Shot.lastline=0;
    Image_Shot.Mid_Out=94;
    Image_Shot.ToBe=0;

    Image_Shot.deltaL = 1;
    Image_Shot.deltaR =-1;
    Image_Shot.MissL_Tag=0;
    Image_Shot.MissR_Tag=0;

    Image_Shot.Left_Max=0;
    Image_Shot.Right_Min=187;  //��¼���ұ߽����ֵ

    Image_Shot.Junction_Tag=1;
    Image_Shot.Carport_Num=0;
    Image_Shot.Carport_Direction=0;

    Image_Shot.CircleL_Enable =1;
    Image_Shot.CircleR_Enable =1;
    Image_Shot.Carport_Enable =1;
    Image_Shot.Circle_In_Enable=0;
    Image_Shot.Circle_Mid_Enable=0;
    Image_Shot.Circle_Out_Enable =0;
    Image_Shot.Back_Out_Enable =0;
    Image_Shot.Back_In_Enable =0;
    Image_Shot.Ramp_Out_Enable=0;

    //ʶ��ʹ��
    Image_Shot.Junction_Enable=0;
    Image_Shot.Circle_Find_Enable=0;
    Image_Shot.Back_Find_Enable=0;

    C_Car.state=Carport;

    for(char i=0;i<ROW_H;i++){
        Image_Shot.left[i]=0;
        Image_Shot.right[i]=187;
        Image_Shot.mid[i]=94;
        Image_Shot.no_left[i]=1;
        Image_Shot.no_right[i]=1;
        Image_Shot.width[i]=i;
    }
}

void Camera_Graph_Show( uint8 i)
{
    ips200_draw_point(Image_Shot.left[i]+1,i*2,RGB565_BLUE);
    ips200_draw_point(Image_Shot.left[i]-1,i*2,RGB565_BLUE);
    ips200_draw_point(Image_Shot.left[i],i*2,RGB565_BLUE);

    ips200_draw_point(Image_Shot.right[i]+1,i*2,RGB565_GREEN);
    ips200_draw_point(Image_Shot.right[i]-1,i*2,RGB565_GREEN);
    ips200_draw_point(Image_Shot.right[i],i*2,RGB565_GREEN);

    ips200_draw_point(Image_Shot.mid[i]+1,i*2,RGB565_RED);
    ips200_draw_point(Image_Shot.mid[i]-1,i*2,RGB565_RED);
    ips200_draw_point(Image_Shot.mid[i],i*2,RGB565_RED);

    ips200_draw_point(95, i, RGB565_PURPLE);
    ips200_draw_point(94, i, RGB565_PURPLE);
    ips200_draw_point(93, i, RGB565_PURPLE);



    ips200_draw_point(29,i,RGB565_YELLOW);
    ips200_draw_point(31,i,RGB565_YELLOW);
    ips200_draw_point(30,i,RGB565_YELLOW);

    ips200_draw_point(157,i,RGB565_BROWN);
    ips200_draw_point(156,i,RGB565_BROWN);
    ips200_draw_point(158,i,RGB565_BROWN);
}

/*******************************************************************************
* Function Name: system_init();
* Description  : ϵͳ��ʼ��
*******************************************************************************/
//void system_Cpu0_init(void)
//{
//    disableInterrupts();
//    get_clk();//��ȡʱ��Ƶ��  ��ر���
//    //�û��ڴ˴����ø��ֳ�ʼ��������
//
//    gpio_init(P20_8, GPO, 0, PUSHPULL);     //����P20_8Ϊ��� Ĭ������͵�ƽ  PUSHPULL���������
//
//    dsw_init(DSW_MAX);  //����
//    key_event_init();   //����
//    BuzzerInit();       //������
//    color_init();       //UI��
//    SteerInit();        //���
//    MotorInit();        //���
//    ips114_init();      //��ʼ��IPS��Ļ
//    EncoderInit();      //������
//    seekfree_wireless_init();   //���ߴ���
//
//    /*����Ԫ�ص�Flash�б�Flash*/
//    /*������ַ+��������*/
//    /*˳������*/
//    OSG_AddFlashDataToList((void*)&ChaSu_coef,                      E_float32);     //0.144     0.144
//    OSG_AddFlashDataToList((void*)&Image_Shot.ToBe,                 E_uint8);       //0         0
//    OSG_AddFlashDataToList((void*)&C_Car.V.Max,                     E_int32);       //65        70
//    OSG_AddFlashDataToList((void*)&KU_lastline,                      E_uint8);       //1         1
//    OSG_AddFlashDataToList((void*)&MT9V03X_CFG[1][1],               E_int16);       //120       50
//    OSG_AddFlashDataToList((void*)&Image_Shot.Carport_Direction,    E_uint8);       //0         0
//    OSG_AddFlashDataToList((void*)&C_Car.V.Min,                     E_int32);       //60        56
//    OSG_AddFlashDataToList((void*)&Image_Shot.Junction_Enable,      E_uint8);       //1         1
//    OSG_AddFlashDataToList((void*)&Image_Shot.Circle_Find_Enable,   E_uint8);       //1         1
//    OSG_AddFlashDataToList((void*)&Image_Shot.Back_Find_Enable,     E_uint8);       //1         1
//    OSG_AddFlashDataToList((void*)&Half_Road_Width57,               E_uint8);       //66        52
//    OSG_AddFlashDataToList((void*)&Image_Shot.Junction_Tag,         E_uint8);       //1         1
//    OSG_AddFlashDataToList((void*)& Add_Junc_L,                     E_uint8);       //1         30
//    OSG_AddFlashDataToList((void*)& Add_Junc_R,                     E_uint8);       //1         30
//    OSG_AddFlashDataToList((void*)& Add_Back_L,                     E_uint8);       //1         90
//    OSG_AddFlashDataToList((void*)& Add_Back_R,                     E_uint8);       //1         90
//    OSG_AddFlashDataToList((void*)& Add_Circ_In,                    E_uint8);       //1         60
//    OSG_AddFlashDataToList((void*)& Add_Junc_L0,                   E_uint8);       //1         45
//    OSG_AddFlashDataToList((void*)& Add_Carport,                   E_uint8);       //1          30
//    OSG_AddFlashDataToList((void*)& Delta_range,                   E_uint8);       //           20
//    OSG_AddFlashDataToList((void*)& Width_range,                   E_uint32);       //           4000
//    OSG_AddFlashDataToList((void*)& End_Row,                       E_uint8);       //           30
//    OSG_AddFlashDataToList((void*)& O_in,                         E_uint8);       //           30
//    OSG_AddFlashDataToList((void*)& O_er,                         E_uint8);       //           30
//    OSG_AddFlashDataToList((void*)& O_delat,                         E_uint8);       //           30
//    OSG_AddFlashDataToList((void*)& O_out,                         E_uint8);       //           30
//    OSG_AddFlashDataToList((void*)& Flash_Wait_Num,                         E_uint8);       //           30
//    OSG_AddFlashDataToList((void*)& Add_Junc_R0,                         E_uint8);       //           30
//    OSG_AddFlashDataToList((void*)& C_Car.V.element,                         E_int32);       //           30
//    OSG_AddFlashDataToList((void*)& YZ_delta,                         E_int32);       //           30
////    /*������Ҫ�����Ĳ���(�Ķ�˳��\���\ɾ�����������������OSG_WriteFlash���µ�˳��д��Flash��,�����ȡ��Խ�����ִ���)*/
//
//    read_flash();
//
//    mt9v03x_init();     //��ʼ������ͷ
//    //�洢 ���
//    Old_HRWid = Half_Road_Width57;
//
//    //�ȴ����к��ĳ�ʼ�����
//    IfxCpu_emitEvent(&g_cpuSyncEvent);
//    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
//    enableInterrupts();
//
//    BuzzerRing;
//    systick_delay_ms(STM0, 500);   //��ʱ2s
//    BuzzerQuiet;
//
//}
//void system_Cpu1_init(void)
//{
//    disableInterrupts();
//    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
//    //�û��ڴ˴����ø��ֳ�ʼ��������
//
//    //��ʼ����ʱ��
//    pit_interrupt_ms(CCU6_1, PIT_CH1, 20);  //���ڰ����Ķ˿ڲɼ�
//    pit_interrupt_ms(CCU6_1, PIT_CH0, 2);   //���ڱ������Ķ˿ڲɼ�
//    pit_interrupt_ms(CCU6_0, PIT_CH1, 5);   //���� ת����ٶȵĿ���
//
//    //�ṹ������ʼ��
//    Car_State_Init();
//    PID_Struct_Init();
//    Image_Info_Init();
//
//    //�ȴ����к��ĳ�ʼ�����
//    IfxCpu_emitEvent(&g_cpuSyncEvent);
//    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
//    enableInterrupts();
//
//}


/*******************************************************************************
* Function Name: Debug_Function
* Description  : ����ʹ�õĴ���
*******************************************************************************/

/**********���Ͳ�������**********/
// ��vofa��λ�� justfloat,һ�δ�6��
// �ȴ���Ԥ�� �۲��6������

//char Send_Wave_Enable=0;
//float Wave_list[6]={0};
//
//void Send_Wave(float val0,float val1,float val2,
//        float val3,float val4,float val5){
//
//    uint8 Send_Buff[28]={0,0,0,0,   0,0,0,0,    0,0,0,0,
//                         0,0,0,0,   0,0,0,0,    0,0,0,0,
//                         0,0,0x80,0x7f};//vofa��֡β0,0,0x80,0x7f
//    //�����۲�����ݴ��� list
//    Wave_list[0]=val0;
//    Wave_list[1]=val1;
//    Wave_list[2]=val2;
//    Wave_list[3]=val3;
//    Wave_list[4]=val4;
//    Wave_list[5]=val5;
//
//    memcpy(Send_Buff, (uint8 *)Wave_list ,sizeof(Wave_list));
//    seekfree_wireless_send_buff(Send_Buff,28);
//}
//
//
//
///**********����ͼ��**********/
//char Send_Image_Enable=0;
//
//void Send_Image(void){// ֱ�ӵ��� ���ɷ���ͼ����zafu��λ������
//    //����ͨ��֡ͷ
//    seekfree_wireless_send_buff("CSU",3);
//    //���ߴ���      ͼ����
//    seekfree_wireless_send_buff(image_new_bin[0],188*60);//����ͼ��
//    //����ͨ��֡β
//    seekfree_wireless_send_buff("USC",3);
//}




