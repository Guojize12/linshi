#include "app_config.h"
#include "app_user.h"
#include "app_user_rx.h"
#include "app_user_globals.h"
#include "app_user_proto.h"

void APP_COMM_Rx_Analysis(uint8_t *buff,uint16_t size)
{
    int temp_len=0;
    int temp_size = size;
    uint8_t bcc,len;
    user_com_def comm;
    while(temp_size>11)
    {
        if(buff[temp_len+0]=='#' &&
                (buff[temp_len+1]=='R' || buff[temp_len+1]=='W'))
        {
            len=buff[temp_len+3]+4;
            bcc = BSP_Bcc(buff+temp_len,len);
            //LOG(" rev Bcc:%X %X\n",bcc, buff[temp_len+len]);
            if(bcc == buff[temp_len+len] )//bcc校验
            {
                //g_comm.did=(buff[temp_len+4]<<16)+(buff[temp_len+5]<<8)+buff[temp_len+6]; //接收ID
               //if(g_comm.did==g_app_config.M)
                {
                    //g_comm.dnum = buff[temp_len+7];//从机号
                    comm.type = buff[temp_len+2]; //从机类型
                    comm.cmd=((uint16_t )buff[temp_len+8]<<8)+buff[temp_len+9];//指令
                    comm.pid = buff[temp_len+len-1];
                    comm.pid_len=1;
                    comm.did.id_buf[3] = 0; //主机ID
                    comm.did.id_buf[2] = buff[temp_len+4];
                    comm.did.id_buf[1] = buff[temp_len+5];
                    comm.did.id_buf[0] = buff[temp_len+6];


                    //LOG(" rev did:%d\n",comm.did.deviceid);
                    uint8_t index;
                    index = APP_USER_did2Index(comm.did.deviceid);
                    if (index<MASTER_MAX)
                    {
                        if(buff[temp_len+1]=='R' )//应答
                        {
                            APP_COMM_Handle_R(&comm,buff+temp_len+10,len-11);
                        }
                        else if(buff[temp_len+1]=='W') //接受
                        {
                            comm.head = 'R';
                            APP_COMM_Handle_W(&comm,buff+temp_len+10,len-11);
                        }

                        //LOG(" rev index:%d %d\n",index,elevator_info[index].did.deviceid);
                        APP_USER_UpdateComm_sta(index,STA_COMM_OK);
                        BSP_TIMER_Modify_Start(&elevator_info[index].timer_MStatus, 3000);
                    }

                }

                len++;
                temp_len+= len;
                temp_size -= len;
            }
            else
            {
                temp_size = 0;
            }
        }
        else
        {
            temp_size--;
            temp_len++;
        }
    }
}


void APP_WIRELESS_Rx_Analysis(uint8_t *buff,uint16_t size)
{
     int temp_len=0;
    int temp_size = size;
    uint8_t bcc,len;
    user_com_def comm;
    uint8_t floor;
    uint8_t direction;
    while(temp_size>=5)
    {
        if(buff[temp_len+0]==0xAA &&
            buff[temp_len+1]==0xBB && 
            buff[temp_len+3]==0xCC && 
            buff[temp_len+4]==0xDD)
        {
            len = 5;
            
            //楼层 1 2 一层； 1=上 2=下
            floor = (buff[temp_len+2]+1)/2;
            direction = buff[temp_len+2]%2?M_KEY_UPPER:M_KEY_LOWER; 
            if(floor > 0 && floor < 42)
            {
                APP_USER_SendKeyEvnet(floor,direction);
                LOG("floor:%d direction:%d\n",floor,direction); 
            }
            
            temp_len+= len;
            temp_size -= len;
        }
        else
        {
            temp_size--;
            temp_len++;
        }
    }
}   


void APP_USER_Rx_Handle(void)
{
    if(g_app_cfg.appComAllow == 0)
    {
        return;
    }
    if(BSP_UART_Rec_Read(APP_USER_UART)==USR_EOK)
    {

        //LOG_HEX(APP_USER_UART_BUF.rxBuf,APP_USER_UART_BUF.rxLen);
        APP_COMM_Rx_Analysis(APP_USER_UART_BUF.rxBuf,APP_USER_UART_BUF.rxLen);

    }
}


void APP_WIRELESS_Rx_Handle(void)
{

    if(g_app_cfg.appComAllow == 0 || object_info.floorXEnable == 0)
    {
        return;
    }

    if(g_app_cfg.floorXType != FLOOR_X_TYPE_WIRELESS)
    {
        return;
    }

    if(BSP_UART_Rec_Read(APP_WIRELESS_UART)==USR_EOK)
    {

        LOG_HEX(APP_WIRELESS_UART_BUF.rxBuf,APP_WIRELESS_UART_BUF.rxLen);
        APP_WIRELESS_Rx_Analysis(APP_WIRELESS_UART_BUF.rxBuf,APP_WIRELESS_UART_BUF.rxLen);

    }
}

