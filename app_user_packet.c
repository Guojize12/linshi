#include "app_config.h"
#include "app_user_packet.h"
#include "app_user_resend.h"

// ����
void APP_USER_Send(uint8_t *buf, uint16_t buf_len)
{
    if(g_app_cfg.appComAllow == 0)
    {
        return;
    }
    BSP_UART_Transmit(APP_USER_UART,buf,buf_len);
}

// ����
void APP_WIRELESS_Send(uint8_t *buf, uint16_t buf_len)
{
    if(g_app_cfg.appComAllow == 0)
    {
        return;
    }
    BSP_UART_Transmit(APP_WIRELESS_UART,buf,buf_len);
}
void APP_USER_packet(uint16_t cmd,st_elevator_info *elev_info,uint8_t *buf,uint8_t len,uint8_t type)//type 0 Ӧ  1  Ӧ
{
    uint8_t sendbuf[200]= {0};
    int numb =0;
    sendbuf[numb++]='#';
    sendbuf[numb++]='W';
    sendbuf[numb++]=g_app_cfg.dtp;   //
    numb++;

    sendbuf[numb++]=elev_info->did.id_buf[2];  //ID
    sendbuf[numb++]=elev_info->did.id_buf[1];  //ID
    sendbuf[numb++]=elev_info->did.id_buf[0];  //ID
    sendbuf[numb++]=0;  //ID

    sendbuf[numb++]=cmd>>8;//
    sendbuf[numb++]=cmd&0xff;//
    memcpy(sendbuf+numb,buf,len);// 
    numb+=len;
    if(type==1)
    {
        sendbuf[numb++]=user_send.g_send_index;//
    }
    sendbuf[3]=numb-4;//
    uint8_t bcc=BSP_Bcc(sendbuf,numb );
    sendbuf[numb++]=bcc;



    //LOG_HEX(sendbuf, numb);
    APP_USER_Send(sendbuf, numb);

    if(type==1)
    {
//		user_send.g_send_index_save[ user_send.g_send_index]= user_send.g_send_index;
        user_send.g_send_buf_len[user_send.g_send_index] = numb;//ͳ
        memcpy(user_send.g_send_buf[user_send.g_send_index],sendbuf,user_send.g_send_buf_len[user_send.g_send_index]);//淢
        BSP_TIMER_Start(&user_send.g_timer_cb[user_send.g_send_index]);//ʱ
        user_send.g_send_max[user_send.g_send_index]=0;//ʱ0
        if(++user_send.g_send_index>=5)//жϻλ,ѭʹ
        {
            user_send.g_send_index = 0;
        }
    }
    //
}

void APP_USER_packet_ack(user_com_def *comm,uint8_t *buf,uint8_t len)
{
    uint8_t sendbuf[200]= {0};
    int numb =0;
    sendbuf[numb++]='#';
    sendbuf[numb++]='R';
    sendbuf[numb++]=g_app_cfg.dtp;   //
    sendbuf[numb++]= 0;
    sendbuf[numb++]=comm->did.id_buf[2];  //ID
    sendbuf[numb++]=comm->did.id_buf[1];  //ID
    sendbuf[numb++]=comm->did.id_buf[0];  //ID
    sendbuf[numb++]=0;  //ID

    sendbuf[numb++]=comm->cmd>>8;//
    sendbuf[numb++]=comm->cmd&0xff;//
    memcpy(sendbuf+numb,buf,len);// 
    numb+=len;
    sendbuf[numb++] = comm->pack_num;
    sendbuf[3]=numb-4;//
    
    uint8_t bcc=BSP_Bcc(sendbuf,numb );
    sendbuf[numb++]=bcc;
    //LOG_HEX(sendbuf, numb);
    APP_USER_Send(sendbuf, numb);
}

void APP_USER_Reply_4000(user_com_def *comm,uint8_t pack_num,uint8_t floor)
{
    uint8_t buf_send[32]= {0};
    int numb =0;
    buf_send[numb++]='#';
    buf_send[numb++]='R';
    buf_send[numb++]=g_app_cfg.dtp;  //
    numb++;
    buf_send[numb++]=comm->did.id_buf[2];  //ID
    buf_send[numb++]=comm->did.id_buf[1];  //ID
    buf_send[numb++]=comm->did.id_buf[0];  //ID
    buf_send[numb++]=0;  //ID
    buf_send[numb++]=comm->cmd>>8;//
    buf_send[numb++]=comm->cmd&0xff;//
    if(floor!=0)
    {
        buf_send[numb++]=floor;   //
    }

    buf_send[numb++]=pack_num;
    buf_send[3]=numb-4;
    uint8_t bcc=BSP_Bcc(buf_send,numb );
    buf_send[numb++]=bcc;
    //LOG_HEX(buf_send, numb);
    APP_USER_Send(buf_send, numb);

}

void APP_USER_Reply(user_com_def *comm,uint8_t pack_num)
{
    uint8_t buf_send[32]= {0};
    int numb =0;
    buf_send[numb++]='#';
    buf_send[numb++]='R';
    buf_send[numb++]=g_app_cfg.dtp;  //
    numb++;
    buf_send[numb++]=comm->did.id_buf[2];  //ID
    buf_send[numb++]=comm->did.id_buf[1];  //ID
    buf_send[numb++]=comm->did.id_buf[0];  //ID
    buf_send[numb++]=0;  //ID
    buf_send[numb++]=comm->cmd>>8;//
    buf_send[numb++]=comm->cmd&0xff;//

    buf_send[numb++]=pack_num;
    buf_send[3]=numb-4;
    uint8_t bcc=BSP_Bcc(buf_send,numb );
    buf_send[numb++]=bcc;
    //LOG_HEX(buf_send, numb);
    APP_USER_Send(buf_send, numb);

}
