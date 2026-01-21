#include "app_user_resend.h"
#include "app_user_packet.h"

bsp_send_def user_send;

void APP_USER_timeout_callback0(void)
{
    if(++user_send.g_send_max[0]<MAX_TIME) //���ʹ�����Ч
    {
        APP_USER_Send(user_send.g_send_buf[0],user_send.g_send_buf_len[0]);//���ͻ����
        BSP_TIMER_Start(&user_send.g_timer_cb[0]);//������ʱ
        LOG("XXXXX resend 0\n");
        LOG_HEX(user_send.g_send_buf[1],user_send.g_send_buf_len[1]);
    }
}

void APP_USER_timeout_callback1(void)
{
    if(++user_send.g_send_max[1]<MAX_TIME) //���ʹ�����Ч
    {
        APP_USER_Send(user_send.g_send_buf[1],user_send.g_send_buf_len[1]);//���ͻ����
        BSP_TIMER_Start(&user_send.g_timer_cb[1]);//������ʱ
        LOG("XXXXX resend 1\n");
        LOG_HEX(user_send.g_send_buf[1],user_send.g_send_buf_len[1]);
    }
}
void APP_USER_timeout_callback2(void)
{
    if(++user_send.g_send_max[2]<MAX_TIME) //���ʹ�����Ч
    {
        APP_USER_Send(user_send.g_send_buf[2],user_send.g_send_buf_len[2]);//���ͻ����
        BSP_TIMER_Start(&user_send.g_timer_cb[2]);//������ʱ
        LOG("XXXXX resend 2\n");
    }
}
void APP_USER_timeout_callback3(void)
{
    if(++user_send.g_send_max[3]<MAX_TIME) //���ʹ�����Ч
    {
        APP_USER_Send(user_send.g_send_buf[3],user_send.g_send_buf_len[3]);//���ͻ����
        BSP_TIMER_Start(&user_send.g_timer_cb[3]);//������ʱ
        LOG("XXXXX resend 3\n");
    }
}
void APP_USER_timeout_callback4(void)
{
    if(++user_send.g_send_max[4]<MAX_TIME) //���ʹ�����Ч
    {
        APP_USER_Send(user_send.g_send_buf[4],user_send.g_send_buf_len[4]);//���ͻ����
        BSP_TIMER_Start(&user_send.g_timer_cb[4]);//������ʱ
        LOG("XXXXX resend 4\n");
    }
}

void (*g_to_cb[5])(void) =
{
    APP_USER_timeout_callback0,
    APP_USER_timeout_callback1,
    APP_USER_timeout_callback2,
    APP_USER_timeout_callback3,
    APP_USER_timeout_callback4,
};

void to_cb_init(void)
{
    for(int i=0; i<5; i++)
    {
        BSP_TIMER_Init(&user_send.g_timer_cb[i],g_to_cb[i],TIMEOUT_150MS+210*i,0);
    }
}
