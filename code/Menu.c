/*
 * Menu.c
 *
 *  Created on: 2023年5月5日
 *      Author: CJR
 */

#include "zf_common_headfile.h"
#include "Menu.h"

static unsigned int K5_State = 0;//编码器旋钮按键状态次数  超过500为长按 不超过500为短按


//编码器旋钮 旋转:上下选择 短按:确定 长按:返回上一级    按下编码器旋钮可通过串口查看
//如果菜单添加了子菜单或者功能函数需注意 menu_init和结构体menuitem需要修改
//注意!!!!!!!  菜单的zf_device_ips200.c貌似与摄像头相关内容有冲突

struct MenuItem
{
    short MenuCount;//当前菜单项目总数
    char *Display_menu; //当前菜单名称
    char *DisplayString;//当前项目要显示的字符
    void(*Subs)();//选择某一菜单后执行的功能函数
    struct MenuItem *ChildrenMenus;//当前项目的子菜单
    struct MenuItem *ParentMenus;//当前项目的父菜单
};

//修改结构体时注意数值个数
struct MenuItem MainMenu[3] = {                                             //主菜单显示内容   在此可添加子菜单
{ 3,(char *)"MainMenu",(char *)"car_speed",NULL,NULL,NULL },
{ 3,(char *)"MainMenu",(char*)"balance_pid",NULL,NULL,NULL },
{ 3,(char *)"MainMenu",(char*)"others",NULL,NULL,NULL },
};

struct MenuItem Secondmenu1[2]={                                            //子菜单1显示内容  在此可添加功能函数 添加时需注意数组个数以及下面的菜单初始化函数
{ 2,(char *)"car_speed",(char *)"set_run",NULL,NULL,MainMenu },
{ 2,(char *)"car_speed",(char *)"set_speed",NULL,NULL,MainMenu }
};

struct MenuItem Secondmenu2[2] = {                                          //子菜单2显示函数  在此可添加功能函数 添加时需注意数组个数以及下面的菜单初始化函数
{ 2,(char *)"balance_pid",(char *)"set_p",NULL,NULL,MainMenu },
{ 2,(char *)"balance_pid",(char *)"set_d",NULL,NULL,MainMenu }
};

struct MenuItem* MenuPoint = MainMenu;//当前菜单的地址
short selectItem;                       //当前在第几行


void display_arrow_mark(uint16 y,const uint16 color)//绘制光标
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

void clear_arrow_mark(void)//清除光标
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

void display(struct MenuItem * MenuPoint,short selectItem)      //屏幕显示
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
    encoder_quad_init(MENU_ENCODER_TIM, MENU_ENCODER_A_PIN, MENU_ENCODER_B_PIN);        //编码器旋钮初始化

    ips200_init(IPS200_TYPE_PARALLEL8);
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);                                       //IPS200初始化     黑底白字

    MainMenu[0].ChildrenMenus = Secondmenu1;                                            //如果主菜单添加了子菜单,此处记得添加
    MainMenu[1].ChildrenMenus = Secondmenu2;

    Secondmenu1[0].Subs = set_run;                                                      //如果子菜单1添加了功能函数,此处记得添加
    Secondmenu1[1].Subs = set_speed;

    Secondmenu2[0].Subs = set_p;                                                        //如果子菜单2添加了功能函数,此处记得添加
    Secondmenu2[1].Subs = set_d;



    selectItem =1;
    display(MenuPoint, selectItem);

}

void menu_run(void)//菜单执行函数
{
    if(encoder_get_count(MENU_ENCODER_TIM) != 0 | key_get_state(KEY_5) != KEY_RELEASE)
    {

        if(encoder_get_count(MENU_ENCODER_TIM)>0)
        {
            //如果当前的行数为1，当前行数为当前菜单的总行数
           if (selectItem == 1) selectItem = MenuPoint->MenuCount;
            //否则行数减1，向上移动一行
           else selectItem--;
           clear_arrow_mark();//上下移动后只清除光标
           display(MenuPoint, selectItem);
           encoder_clear_count(MENU_ENCODER_TIM);//清除编码器读数

        }

        if(encoder_get_count(MENU_ENCODER_TIM)<0)
        {
            //如果当前的行数为当前菜单的总行数，当前行数为1
            if (selectItem == MenuPoint->MenuCount) selectItem = 1;
            //否则行数加1，向下移动一行
            else selectItem++;
            clear_arrow_mark();//上下移动后只清除光标
            display(MenuPoint, selectItem);
            encoder_clear_count(MENU_ENCODER_TIM);//清除编码器读数
        }

        while(key_get_state(KEY_5) == KEY_SHORT_PRESS)
        {
            while(key_get_state(KEY_5) != KEY_RELEASE)
            {
                system_delay_ms(1);
                K5_State++;
                key_scanner();
            }

            if(K5_State < 500)//短按:确定
            {
                printf("KEY5_SHORT_PRESS\n");
                if(MenuPoint[selectItem - 1].Subs != NULL)
                    {
                        MenuPoint[selectItem - 1].Subs();
                        ips200_clear();
                        display(MenuPoint, selectItem);
                    }

                //如果子菜单不为空
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

            else if(K5_State >= 500)//长按:返回
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




//功能函数(测试版 随便取的函数名,如修改后,函数名需与上面一一对应)
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
