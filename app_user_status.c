#include "app_config.h"
#include "app_user_status.h"
#include "app_user_packet.h"

#define TEXT_ENERGY 0

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
    //©
    tmp.bit32_2.bit4 = 1;
    //¶
    tmp.bit32_2.bit2 =2; 
#else


    //©
    tmp.bit32_2.bit4 =  g_energy_data.LEAKAGE_CURRENT_OVERLOAD_FLG? 2:0;
    //¶
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
   
           APP_USER_packet(5004,&elevator_info[i],send_buff,buff_len,0);//type 0 Ӧ  1  Ӧ
        }
    }
}


//״̬
void APP_USER_SendDoorStatusEvnet(void)
{
    uint8_t buff_len = 0;
    uint8_t ctlModeIndex = 0;
    uint8_t com = 0;
    uint8_t i;

    //¥
    send_buff[buff_len++] = object_info.com;
    ctlModeIndex = buff_len;

    // ģʽ
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
            //4bit->1Ų 4bit->2Ų
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
            send_buff[ctlModeIndex] = elevator_info[i==MASTER_1?MASTER_2:MASTER_1].MCommSta; //ͨ״̬
            APP_USER_packet(5002,&elevator_info[i],send_buff,buff_len,0);
        }
    }
	//LOG_HEX(send_buff,buff_len);

}
