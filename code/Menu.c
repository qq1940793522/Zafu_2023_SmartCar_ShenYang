/*
 * Menu.c
 *
 *  Created on: 2023��5��5��
 *      Author: CJR
 */

#include "zf_common_headfile.h"
#include "Menu.h"



struct MenuItem
{
    short MenuCount;//��ǰ�˵���Ŀ����
    char *Display_menu; //��ǰ�˵�����
    char *DisplayString;//��ǰ��ĿҪ��ʾ���ַ�
    void(*Subs)();//ѡ��ĳһ�˵���ִ�еĹ��ܺ���
    struct MenuItem *ChildrenMenus;//��ǰ��Ŀ���Ӳ˵�
    struct MenuItem *ParentMenus;//��ǰ��Ŀ�ĸ��˵�
};

struct MenuItem MainMenu[3] = {
{ 3,(char *)"MainMenu",(char *)"car_speed",NULL,NULL,NULL },
{ 3,(char *)"MainMenu",(char*)"balance_pid",NULL,NULL,NULL },
{ 3,(char *)"MainMenu",(char*)"others",NULL,NULL,NULL },
};

struct MenuItem Secondmenu1[2]={
{ 2,(char *)"car_speed",(char *)"set_run",NULL,NULL,MainMenu },
{ 2,(char *)"car_speed",(char *)"set_speed",NULL,NULL,MainMenu }
};

struct MenuItem Secondmenu2[2] = {
{ 2,(char *)"balance_pid",(char *)"set_p",NULL,NULL,MainMenu },
{ 2,(char *)"balance_pid",(char *)"set_d",NULL,NULL,MainMenu }
};

struct MenuItem* MenuPoint = MainMenu;//��ǰ�˵��ĵ�ַ
short selectItem;                       //��ǰ�ڵڼ���


void display_arrow_mark(uint16 y,const uint16 color)
{
    uint16 i,j;
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


void display(struct MenuItem * MenuPoint,short selectItem)
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
    encoder_quad_init(MENU_ENCODER_TIM, MENU_ENCODER_A_PIN, MENU_ENCODER_B_PIN);
    gpio_init(MENU_KEY_PIN, GPI, 1, GPI_PULL_UP);                                       //��������ť��ʼ��

    ips200_init(IPS200_TYPE_PARALLEL8);
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);                                       //IPS200��ʼ��     �ڵװ���

    MainMenu[0].ChildrenMenus = Secondmenu1;
    MainMenu[1].ChildrenMenus = Secondmenu2;

    Secondmenu1[0].Subs = set_run;
    Secondmenu1[1].Subs = set_speed;

    Secondmenu2[0].Subs = set_p;
    Secondmenu2[1].Subs = set_d;

    selectItem =1;
    display(MenuPoint, selectItem);

}

void menu_run(void)
{
    if(encoder_get_count(MENU_ENCODER_TIM)>0)
    {
        //�����ǰ������Ϊ1����ǰ����Ϊ��ǰ�˵���������
       if (selectItem == 1) selectItem = MenuPoint->MenuCount;
        //����������1�������ƶ�һ��
       else selectItem--;
    }

    if(encoder_get_count(MENU_ENCODER_TIM)<0)
    {
        //�����ǰ������Ϊ��ǰ�˵�������������ǰ����Ϊ1
        if (selectItem == MenuPoint->MenuCount) selectItem = 1;
        //����������1�������ƶ�һ��
        else selectItem++;
    }


    display(MenuPoint, selectItem);

}


void set_run(void)
{}

void set_speed(void)
{}

void set_p(void)
{}

void set_d(void)
{}
