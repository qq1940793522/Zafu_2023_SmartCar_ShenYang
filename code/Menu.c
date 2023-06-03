/*
 * Menu.c
 *
 *  Created on: 2023��5��5��
 *      Author: CJR
 */

#include "zf_common_headfile.h"
#include "Menu.h"

static unsigned int K5_State = 0;//��������ť����״̬����  ����500Ϊ���� ������500Ϊ�̰�


//��������ť ��ת:����ѡ�� �̰�:ȷ�� ����:������һ��    ���±�������ť��ͨ�����ڲ鿴
//����˵�������Ӳ˵����߹��ܺ�����ע�� menu_init�ͽṹ��menuitem��Ҫ�޸�
//ע��!!!!!!!  �˵���zf_device_ips200.cò��������ͷ��������г�ͻ

struct MenuItem
{
    short MenuCount;//��ǰ�˵���Ŀ����
    char *Display_menu; //��ǰ�˵�����
    char *DisplayString;//��ǰ��ĿҪ��ʾ���ַ�
    void(*Subs)();//ѡ��ĳһ�˵���ִ�еĹ��ܺ���
    struct MenuItem *ChildrenMenus;//��ǰ��Ŀ���Ӳ˵�
    struct MenuItem *ParentMenus;//��ǰ��Ŀ�ĸ��˵�
};

//�޸Ľṹ��ʱע����ֵ����
struct MenuItem MainMenu[3] = {                                             //���˵���ʾ����   �ڴ˿�����Ӳ˵�
{ 3,(char *)"MainMenu",(char *)"car_speed",NULL,NULL,NULL },
{ 3,(char *)"MainMenu",(char*)"balance_pid",NULL,NULL,NULL },
{ 3,(char *)"MainMenu",(char*)"others",NULL,NULL,NULL },
};

struct MenuItem Secondmenu1[2]={                                            //�Ӳ˵�1��ʾ����  �ڴ˿���ӹ��ܺ��� ���ʱ��ע����������Լ�����Ĳ˵���ʼ������
{ 2,(char *)"car_speed",(char *)"set_run",NULL,NULL,MainMenu },
{ 2,(char *)"car_speed",(char *)"set_speed",NULL,NULL,MainMenu }
};

struct MenuItem Secondmenu2[2] = {                                          //�Ӳ˵�2��ʾ����  �ڴ˿���ӹ��ܺ��� ���ʱ��ע����������Լ�����Ĳ˵���ʼ������
{ 2,(char *)"balance_pid",(char *)"set_p",NULL,NULL,MainMenu },
{ 2,(char *)"balance_pid",(char *)"set_d",NULL,NULL,MainMenu }
};

struct MenuItem* MenuPoint = MainMenu;//��ǰ�˵��ĵ�ַ
short selectItem;                       //��ǰ�ڵڼ���


void display_arrow_mark(uint16 y,const uint16 color)//���ƹ��
{
    int32 i,j;
    y = y - 10;
    for(j = 0 ;j > -5 ; j--)
    {
        for(i = 10;i < 15 + j;i++)
        {
            ips200_draw_point(i,y+j,color);
        }
    }
    for(j = 0 ;j < 5 ; j++)
    {
        for(i = 10;i < 15 - j;i++)
        {
            ips200_draw_point(i,y+j,color);
        }
    }

}

void clear_arrow_mark(void)//������
{
    uint16 i,j;
    for(i=10;i<20;i++)
    {
        for(j=20;j<160;j++)
        {
            ips200_draw_point(i,j,RGB565_BLACK);
        }
    }


}

void display(struct MenuItem * MenuPoint,short selectItem)      //��Ļ��ʾ
{
    uint16 i;
    uint16 x = 20;
    uint16 y = 20;

    ips200_show_string(0,0,MenuPoint[0].Display_menu);
    display_arrow_mark(selectItem*20+20,RGB565_WHITE);

    for(i = 0;i < MenuPoint->MenuCount;i++)
        {
            ips200_show_string(x,y,MenuPoint[i].DisplayString);
            y += 20;
        }
}



void menu_init(void)
{
    encoder_quad_init(MENU_ENCODER_TIM, MENU_ENCODER_A_PIN, MENU_ENCODER_B_PIN);        //��������ť��ʼ��

    ips200_init(IPS200_TYPE_PARALLEL8);
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);                                       //IPS200��ʼ��     �ڵװ���

    MainMenu[0].ChildrenMenus = Secondmenu1;                                            //������˵�������Ӳ˵�,�˴��ǵ����
    MainMenu[1].ChildrenMenus = Secondmenu2;

    Secondmenu1[0].Subs = set_run;                                                      //����Ӳ˵�1����˹��ܺ���,�˴��ǵ����
    Secondmenu1[1].Subs = set_speed;

    Secondmenu2[0].Subs = set_p;                                                        //����Ӳ˵�2����˹��ܺ���,�˴��ǵ����
    Secondmenu2[1].Subs = set_d;



    selectItem =1;
    display(MenuPoint, selectItem);

}

void menu_run(void)//�˵�ִ�к���
{
    if(encoder_get_count(MENU_ENCODER_TIM) != 0 | key_get_state(KEY_5) != KEY_RELEASE)
    {

        if(encoder_get_count(MENU_ENCODER_TIM)>0)
        {
            //�����ǰ������Ϊ1����ǰ����Ϊ��ǰ�˵���������
           if (selectItem == 1) selectItem = MenuPoint->MenuCount;
            //����������1�������ƶ�һ��
           else selectItem--;
           clear_arrow_mark();//�����ƶ���ֻ������
           display(MenuPoint, selectItem);
           encoder_clear_count(MENU_ENCODER_TIM);//�������������

        }

        if(encoder_get_count(MENU_ENCODER_TIM)<0)
        {
            //�����ǰ������Ϊ��ǰ�˵�������������ǰ����Ϊ1
            if (selectItem == MenuPoint->MenuCount) selectItem = 1;
            //����������1�������ƶ�һ��
            else selectItem++;
            clear_arrow_mark();//�����ƶ���ֻ������
            display(MenuPoint, selectItem);
            encoder_clear_count(MENU_ENCODER_TIM);//�������������
        }

        while(key_get_state(KEY_5) == KEY_SHORT_PRESS)
        {
            while(key_get_state(KEY_5) != KEY_RELEASE)
            {
                system_delay_ms(1);
                K5_State++;
                key_scanner();
            }

            if(K5_State < 500)//�̰�:ȷ��
            {
                printf("KEY5_SHORT_PRESS\n");
                if(MenuPoint[selectItem - 1].Subs != NULL)
                    {
                        MenuPoint[selectItem - 1].Subs();
                        ips200_clear();
                        display(MenuPoint, selectItem);
                    }

                //����Ӳ˵���Ϊ��
                if (MenuPoint[selectItem - 1].ChildrenMenus != NULL)
                    {
                        MenuPoint = MenuPoint[selectItem - 1].ChildrenMenus;
                        selectItem = 1;

                        ips200_clear();
                        display(MenuPoint, selectItem);
                    }

                K5_State = 0;
                break;
            }

            else if(K5_State >= 500)//����:����
            {
                printf("KEY5_LONG_PRESS\n");
                if (MenuPoint[selectItem - 1].ParentMenus != NULL)
                {
                    MenuPoint = MenuPoint[selectItem - 1].ParentMenus;
                    selectItem = 1;

                    ips200_clear();
                    display(MenuPoint, selectItem);
                }

                K5_State = 0;
                break;
            }


        }



    }



}




//���ܺ���(���԰� ���ȡ�ĺ�����,���޸ĺ�,��������������һһ��Ӧ)
void set_run(void)
{
    printf("set_run success!\n");
}

void set_speed(void)
{
    printf("set_speed success!\n");
}

void set_p(void)
{

}

void set_d(void)
{}
