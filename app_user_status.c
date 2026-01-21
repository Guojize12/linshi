#include "app_config.h"
#include "app_user.h"
#include "app_user_status.h"
#include "app_user_globals.h"
#include "app_user_packet.h"

void APP_USER_SetCtlMode(uint8_t index,uint8_t ctlMode)
{
    uint8_t buff_len = 0;
    send_buff[buff_len++] = 1;
    send_buff[buff_len++] = ctlMode;

    if(elevator_info[index].MCommSta == STA_COMM_OK)
    {
        APP_USER_packet(5009,&elevator_info[index],send_buff,buff_len,1);//type 0 不等应答  1  等应答
        LOG("SetCtlMode ");
        LOG_HEX(send_buff,buff_len);
    }
        

    
}


void APP_USER_recoverErr(uint8_t index)
{
    uint8_t buff_len = 0;
    send_buff[buff_len++] = 2;
    send_buff[buff_len++] = 1;

    if(elevator_info[index].MCommSta == STA_COMM_OK)
        APP_USER_packet(5009,&elevator_info[index],send_buff,buff_len,1);//type 0 不等应答  1  等应答
    
    APP_COMM_clearFlooXList(index);
    LOG("RecoverErr ");
    LOG_HEX(send_buff,buff_len);
}


#define TEXT_ENERGY 0

void APP_USER_SendEnergy(void)
{
    uint8_t i;
    uint8_t cancel_Floor = 0;
    uint8_t buff_len = 0;
    IEEE754 tmp;

    for (i=0; i<3; i++)
    {
#if TEXT_ENERGY
        g_energy_data.POWER_CURRENT[i].flt =0.123+i;
#endif
        send_buff[buff_len++] = g_energy_data.POWER_CURRENT[i].u8_buf[3];
        send_buff[buff_len++] = g_energy_data.POWER_CURRENT[i].u8_buf[2];
        send_buff[buff_len++] = g_energy_data.POWER_CURRENT[i].u8_buf[1];
        send_buff[buff_len++] = g_energy_data.POWER_CURRENT[i].u8_buf[0];
    }

#if TEXT_ENERGY
    g_energy_data.POWER_YG_ALL.flt =4.567;
    g_energy_data.ENERGY_YG_ALL.flt =5.678;
    object_info.temperCable.flt =25.12;
    object_info.temperEnvir.flt =26.34;
    g_energy_data.LEAKAGE_CURRENT.flt =123.456;
#endif

    send_buff[buff_len++] = g_energy_data.POWER_YG_ALL.u8_buf[3];
    send_buff[buff_len++] = g_energy_data.POWER_YG_ALL.u8_buf[2];
    send_buff[buff_len++] = g_energy_data.POWER_YG_ALL.u8_buf[1];
    send_buff[buff_len++] = g_energy_data.POWER_YG_ALL.u8_buf[0];

    
    send_buff[buff_len++] = g_energy_data.ENERGY_YG_ALL.u8_buf[3];
    send_buff[buff_len++] = g_energy_data.ENERGY_YG_ALL.u8_buf[2];
    send_buff[buff_len++] = g_energy_data.ENERGY_YG_ALL.u8_buf[1];
    send_buff[buff_len++] = g_energy_data.ENERGY_YG_ALL.u8_buf[0];

    
    send_buff[buff_len++] = object_info.temperCable.u8_buf[3];
    send_buff[buff_len++] = object_info.temperCable.u8_buf[2];
    send_buff[buff_len++] = object_info.temperCable.u8_buf[1];
    send_buff[buff_len++] = object_info.temperCable.u8_buf[0];

    send_buff[buff_len++] = object_info.temperEnvir.u8_buf[3];
    send_buff[buff_len++] = object_info.temperEnvir.u8_buf[2];
    send_buff[buff_len++] = object_info.temperEnvir.u8_buf[1];
    send_buff[buff_len++] = object_info.temperEnvir.u8_buf[0];

    send_buff[buff_len++] = g_energy_data.LEAKAGE_CURRENT.u8_buf[3];
    send_buff[buff_len++] = g_energy_data.LEAKAGE_CURRENT.u8_buf[2];
    send_buff[buff_len++] = g_energy_data.LEAKAGE_CURRENT.u8_buf[1];
    send_buff[buff_len++] = g_energy_data.LEAKAGE_CURRENT.u8_buf[0];

    tmp.u32 = 0;

#if TEXT_ENERGY
    //漏电流
    tmp.bit32_2.bit4 = 1;
    //线缆温度
    tmp.bit32_2.bit2 =2; 
#else


    //漏电流
    tmp.bit32_2.bit4 =  g_energy_data.LEAKAGE_CURRENT_OVERLOAD_FLG? 2:0;
    //线缆温度
    tmp.bit32_2.bit2 =  g_energy_data.CABLE_TEMPER_BJ_FLG? 2:0;
#endif

    send_buff[buff_len++] = tmp.u8_buf[3];
    send_buff[buff_len++] = tmp.u8_buf[2];
    send_buff[buff_len++] = tmp.u8_buf[1];
    send_buff[buff_len++] = tmp.u8_buf[0];
     for(i=0;i<MASTER_MAX;i++)
    {
        //if (elevator_info[i].MCommSta==STA_COMM_OK)
        {
   
           APP_USER_packet(5004,&elevator_info[i],send_buff,buff_len,0);//type 0 不等应答  1  等应答
        }
    }
}

//发送状态

void APP_USER_SendDoorStatusEvnet(void)
{
    uint8_t buff_len = 0;
    uint8_t ctlModeIndex = 0;
    uint8_t com = 0;
    uint8_t i;

    //总楼层
    send_buff[buff_len++] = object_info.com;
    ctlModeIndex = buff_len;

    //填空 控制模式
    send_buff[buff_len++] = 0;

    com = object_info.com >MAXFLOOR? 0:object_info.com;
    for(i=1;i<(com+1);i++)
    {
        if (object_info.FloorBoardLoseCount[i] >=FLOOR_BOARD_TIMEOUT)
        {
            send_buff[buff_len++] = 0x22;
        }
        else
        {
            bsp_un8_def tmp_data;
            if (g_app_cfg.floorXType == FLOOR_X_TYPE_CABLE)
            {
                tmp_data.bt8_2.bit0 = elevator_info[MASTER_1].FxDoorSta[i].bt8_2.bit0?1:0;
                tmp_data.bt8_2.bit2 = elevator_info[MASTER_1].FxDoorSta[i].bt8_2.bit2;
                
                tmp_data.bt8_2.bit4 = elevator_info[MASTER_2].FxDoorSta[i].bt8_2.bit0?1:0;
                tmp_data.bt8_2.bit6 = elevator_info[MASTER_2].FxDoorSta[i].bt8_2.bit2;
            }
            else
            {
                tmp_data.u8 = 0;
            }
            send_buff[buff_len++] = tmp_data.u8;
            //低4bit->1号层面 高4bit->2号层面
          //  send_buff[buff_len] = elevator_info[MASTER_1].FxDoorSta[i].u8 &0x0F;
          //  send_buff[buff_len++] |= (elevator_info[MASTER_2].FxDoorSta[i].u8<<4) &0xF0;
        }
    }
    send_buff[buff_len++] = elevator_info[MASTER_1].F1Sta.u8;
    send_buff[buff_len++] = elevator_info[MASTER_2].F1Sta.u8;
    
    for(i=0;i<MASTER_MAX;i++)
    {
        //if (elevator_info[i].MCommSta==STA_COMM_OK)
        {
            send_buff[ctlModeIndex] = elevator_info[i==MASTER_1?MASTER_2:MASTER_1].MCommSta; //对笼通信状态
            APP_USER_packet(5002,&elevator_info[i],send_buff,buff_len,0);
        }
    }
	//LOG_HEX(send_buff,buff_len);

}


//发送状态
