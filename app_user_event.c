#include "app_config.h"
#include "app_user_event.h"
#include "app_user_packet.h"
#include "app_user_utils.h"

typedef struct
{
    uint8_t commOk[MASTER_MAX];   //ͨ״̬
    uint8_t AutoModeSta[MASTER_MAX];  //Զ״̬
    uint8_t NofullSta[MASTER_MAX];  //δԱ״̬
    uint8_t stopErrSta[MASTER_MAX];  //쳣ͣ
    uint8_t smartSta[MASTER_MAX];  //״̬  
    uint8_t sched_No;           //
} st_sched_info;

enum
{
    SCHEDSTA_NG=0, 
    SCHEDSTA_OK,  
    SCHEDSTA_XX //
};

st_sched_info stSchedList[] = {
    //ͨ״̬                      Զ״̬                        δԱ״̬                       쳣ͣ״̬              ״̬                        
    {SCHEDSTA_OK, SCHEDSTA_OK,      SCHEDSTA_OK,SCHEDSTA_OK,     SCHEDSTA_OK,SCHEDSTA_OK,    SCHEDSTA_OK,SCHEDSTA_OK,   SCHEDSTA_OK,SCHEDSTA_OK,        MASTER_SCHED},
                                                                    
    {SCHEDSTA_OK, SCHEDSTA_OK,      SCHEDSTA_OK,SCHEDSTA_OK,     SCHEDSTA_OK,SCHEDSTA_OK,    SCHEDSTA_OK,SCHEDSTA_OK,   SCHEDSTA_OK,SCHEDSTA_NG,        MASTER_ALL},
    {SCHEDSTA_OK, SCHEDSTA_OK,      SCHEDSTA_OK,SCHEDSTA_OK,     SCHEDSTA_OK,SCHEDSTA_OK,    SCHEDSTA_OK,SCHEDSTA_OK,   SCHEDSTA_NG,SCHEDSTA_OK,        MASTER_ALL},
    {SCHEDSTA_OK, SCHEDSTA_OK,      SCHEDSTA_OK,SCHEDSTA_OK,     SCHEDSTA_OK,SCHEDSTA_OK,    SCHEDSTA_OK,SCHEDSTA_OK,   SCHEDSTA_NG,SCHEDSTA_NG,        MASTER_ALL},

    {SCHEDSTA_OK, SCHEDSTA_XX,      SCHEDSTA_OK,SCHEDSTA_XX,     SCHEDSTA_OK,SCHEDSTA_XX,    SCHEDSTA_OK,SCHEDSTA_XX,   SCHEDSTA_XX,SCHEDSTA_XX,        MASTER_1},
    {SCHEDSTA_XX, SCHEDSTA_OK,      SCHEDSTA_XX,SCHEDSTA_OK,     SCHEDSTA_XX,SCHEDSTA_OK,    SCHEDSTA_XX,SCHEDSTA_OK,   SCHEDSTA_XX,SCHEDSTA_XX,        MASTER_2},
};

void APP_USER_SendKeyEvnet(uint8_t floor,uint8_t direction)
{
    uint8_t buff_len = 0;
    send_buff[buff_len++] = object_info.com;
    send_buff[buff_len++] = floor;
    send_buff[buff_len++] = direction;
    g_comm.dnum = floor;
	uint8_t sched_No;
   
    if(APP_USER_SupplyListHave(floor,direction))
    {
        LOG("APP_USER_SendKeyEvnet Have supply floor:%d dir:%d\n",floor, direction);
        return;
    }

    // elevator_info[MASTER_1].MCommSta=STA_COMM_OK;
    // elevator_info[MASTER_2].MCommSta=STA_COMM_OK;
    // elevator_info[MASTER_1].ctlMode=0;
    // elevator_info[MASTER_2].ctlMode=0;
    // elevator_info[MASTER_1].smart_mode=1;
    // elevator_info[MASTER_2].smart_mode=1;
    // elevator_info[MASTER_1].fullLoad=0;
    // elevator_info[MASTER_2].fullLoad=0;

    sched_No = MASTER_NULL;

    LOG("INFO No.1:  commentSta=%d ctlMode=%d full=%d stopErr=%d smart=%d\n",elevator_info[MASTER_1].MCommSta,
                        elevator_info[MASTER_1].ctlMode,elevator_info[MASTER_1].fullLoad,elevator_info[MASTER_1].errStop,elevator_info[MASTER_1].smart_mode);
    LOG("INFO No.2:  commentSta=%d ctlMode=%d full=%d stopErr=%d smart=%d\n",elevator_info[MASTER_2].MCommSta,
                         elevator_info[MASTER_2].ctlMode,elevator_info[MASTER_2].fullLoad,elevator_info[MASTER_2].errStop,elevator_info[MASTER_2].smart_mode);

    st_sched_info tmp_schedInfo = {0};
    tmp_schedInfo.commOk[MASTER_1] = elevator_info[MASTER_1].MCommSta==STA_COMM_OK ?SCHEDSTA_OK:SCHEDSTA_NG;
    tmp_schedInfo.commOk[MASTER_2] = elevator_info[MASTER_2].MCommSta==STA_COMM_OK ?SCHEDSTA_OK:SCHEDSTA_NG;

    tmp_schedInfo.AutoModeSta[MASTER_1] = elevator_info[MASTER_1].ctlMode==0 ?SCHEDSTA_OK:SCHEDSTA_NG;
    tmp_schedInfo.AutoModeSta[MASTER_2] = elevator_info[MASTER_2].ctlMode==0 ?SCHEDSTA_OK:SCHEDSTA_NG;

    tmp_schedInfo.NofullSta[MASTER_1] = elevator_info[MASTER_1].fullLoad==0?SCHEDSTA_OK:SCHEDSTA_NG;
    tmp_schedInfo.NofullSta[MASTER_2] = elevator_info[MASTER_2].fullLoad==0?SCHEDSTA_OK:SCHEDSTA_NG;

    tmp_schedInfo.stopErrSta[MASTER_1] = elevator_info[MASTER_1].errStop==0?SCHEDSTA_OK:SCHEDSTA_NG;
    tmp_schedInfo.stopErrSta[MASTER_2] = elevator_info[MASTER_2].errStop==0?SCHEDSTA_OK:SCHEDSTA_NG;

    tmp_schedInfo.smartSta[MASTER_1] = elevator_info[MASTER_1].smart_mode?SCHEDSTA_OK:SCHEDSTA_NG;
    tmp_schedInfo.smartSta[MASTER_2] = elevator_info[MASTER_2].smart_mode?SCHEDSTA_OK:SCHEDSTA_NG;


// LOG("INFO No.2:  commentSta=%d ctlMode=%d full=%d stopErr=%d smart=%d\n",tmp_schedInfo.commOk[MASTER_2] ,
//                          tmp_schedInfo.AutoModeSta[MASTER_2],tmp_schedInfo.NofullSta[MASTER_2],tmp_schedInfo.stopErrSta[MASTER_2],tmp_schedInfo.smartSta[MASTER_2]);

    if (g_app_cfg.appLicence[MASTER_1] !=APPLICENCE_NORMAL || g_app_cfg.appLicence[MASTER_2] !=APPLICENCE_NORMAL)
    {
        if (g_app_cfg.appLicence[MASTER_1] ==APPLICENCE_NORMAL)
        {
            sched_No == MASTER_1;
        }
        else if (g_app_cfg.appLicence[MASTER_2] ==APPLICENCE_NORMAL)
        {
            sched_No == MASTER_2;
        }
    }
    else
    {
        for (uint8_t i=0; i< sizeof(stSchedList)/sizeof(st_sched_info);i++)
        {
    // LOG("INFO No.2:  commentSta=%d ctlMode=%d full=%d stopErr=%d smart=%d\n",tmp_schedInfo.commOk[MASTER_2] ,
    //                          stSchedList[i].AutoModeSta[MASTER_2],stSchedList[i].NofullSta[MASTER_2],stSchedList[i].stopErrSta[MASTER_2],stSchedList[i].smartSta[MASTER_2]);

            if((tmp_schedInfo.commOk[MASTER_1] == stSchedList[i].commOk[MASTER_1]) || (stSchedList[i].commOk[MASTER_1] == SCHEDSTA_XX))
            if((tmp_schedInfo.commOk[MASTER_2] == stSchedList[i].commOk[MASTER_2]) || (stSchedList[i].commOk[MASTER_2] == SCHEDSTA_XX))

            if((tmp_schedInfo.AutoModeSta[MASTER_1] == stSchedList[i].AutoModeSta[MASTER_1]) || (stSchedList[i].AutoModeSta[MASTER_1] == SCHEDSTA_XX))
            if((tmp_schedInfo.AutoModeSta[MASTER_2] == stSchedList[i].AutoModeSta[MASTER_2]) || (stSchedList[i].AutoModeSta[MASTER_2] == SCHEDSTA_XX))

            if((tmp_schedInfo.NofullSta[MASTER_1] == stSchedList[i].NofullSta[MASTER_1]) || (stSchedList[i].NofullSta[MASTER_1] == SCHEDSTA_XX))
            if((tmp_schedInfo.NofullSta[MASTER_2] == stSchedList[i].NofullSta[MASTER_2]) || (stSchedList[i].NofullSta[MASTER_2] == SCHEDSTA_XX))

            if((tmp_schedInfo.stopErrSta[MASTER_1] == stSchedList[i].stopErrSta[MASTER_1]) || (stSchedList[i].stopErrSta[MASTER_1] == SCHEDSTA_XX))
            if((tmp_schedInfo.stopErrSta[MASTER_2] == stSchedList[i].stopErrSta[MASTER_2]) || (stSchedList[i].stopErrSta[MASTER_2] == SCHEDSTA_XX))

            if((tmp_schedInfo.smartSta[MASTER_1] == stSchedList[i].smartSta[MASTER_1] ) || (stSchedList[i].smartSta[MASTER_1] == SCHEDSTA_XX))
            if((tmp_schedInfo.smartSta[MASTER_2] == stSchedList[i].smartSta[MASTER_2]) || (stSchedList[i].smartSta[MASTER_2] == SCHEDSTA_XX))


            {
                sched_No = stSchedList[i].sched_No;
                LOG("INFO find No.%d %d\n",sched_No, i);
                break;
            }
        }
    }
    
    if(sched_No <MASTER_MAX)
    {
        APP_USER_packet(5000,&elevator_info[sched_No],send_buff,buff_len,1);
        LOG("INFO No.%d\n",sched_No);


        if(sched_No == MASTER_1) object_info.sched_No1=1;
        else object_info.sched_No1=0;
        if(sched_No == MASTER_2) object_info.sched_No2=1;
        else object_info.sched_No2=0;

        object_info.sched_time = BSP_TIMER_Ticks_Get()/1000;

    }
    else if(sched_No == MASTER_ALL)
    {
        APP_USER_packet(5000,&elevator_info[MASTER_1],send_buff,buff_len,1);
        APP_USER_packet(5000,&elevator_info[MASTER_2],send_buff,buff_len,1);
        LOG("INFO ALL\n");

        object_info.sched_No1=1;
        object_info.sched_No2=1;
        object_info.sched_time = BSP_TIMER_Ticks_Get()/1000;

    }
    else if(sched_No == MASTER_SCHED)
    {
        sched_No = APP_ELEVATOR_sched(floor,direction);
        APP_USER_packet(5000,&elevator_info[sched_No],send_buff,buff_len,1);
        LOG("INFO No.%d sched\n",sched_No);

         if(sched_No == MASTER_1) object_info.sched_No1=1;
        else object_info.sched_No1=0;
        if(sched_No == MASTER_2) object_info.sched_No2=1;
        else object_info.sched_No2=0;
        object_info.sched_time = BSP_TIMER_Ticks_Get()/1000;
    }
    else
    {
        ; //ת
        LOG("ERR don't sched. No.1: smart=%d commentSta=%d\n",elevator_info[MASTER_1].smart_mode,elevator_info[MASTER_1].MCommSta);
        LOG("ERR don't sched. No.2: smart=%d commentSta=%d\n",elevator_info[MASTER_2].smart_mode,elevator_info[MASTER_2].MCommSta);
    }
}

void APP_USER_SendCancelKeyEvnet(uint8_t floor,uint8_t direction)
{
    uint8_t i,index;
    uint8_t cancel_Floor = 0;
    uint8_t buff_len = 0;
    send_buff[buff_len++] = floor;
    send_buff[buff_len++] = direction;
    g_comm.dnum = floor;

    //Ӧб㣬ͷȡ
    for(index=0; index<MASTER_MAX;index++)
    {
        cancel_Floor = 0;
        for(i=0; i<elevator_info[index].support_Num;i++)
        {
            if((elevator_info[index].support_data[i] &0x3F) == floor)
            {
                cancel_Floor = 1;
                break;
            }
                
        }
        if(cancel_Floor)
            APP_USER_packet(5001,&elevator_info[index],send_buff,buff_len,1);//type 0 Ӧ  1  Ӧ
    }
}

//Ű
void APP_COMM_KeyLedOn(uint8_t index, uint8_t flootID, uint8_t direction)
{ 
    if(flootID==1)
    {
        if(direction == M_KEY_UPPER)
        {
             APP_COMM_F1Key(index,1);
            //LOG("keyled off index=%d floor=%d dir=%d\n",index,flootID,direction);
			elevator_info[index].FloorLED[1].bt8.bit0 = 1;
        }
				
       
    }
    else
    {
        if(direction == M_KEY_UPPER)
        {
            object_info.slave_data[flootID].bit16_2.bit4 = 1;
            elevator_info[index].FloorLED[flootID].bt8.bit0 = 1;
        }
        else if(direction == M_KEY_LOWER)
        {
            object_info.slave_data[flootID].bit16_2.bit6 = 1;
            elevator_info[index].FloorLED[flootID].bt8.bit1 = 1;
        }
    }

    //LOG("keyled on index=%d floor=%d dir=%d\n",index,flootID,direction);
}

//ϨŰ
void APP_COMM_KeyLedOff(uint8_t index, uint8_t flootID, uint8_t direction)
{  
    if(flootID==1)
    {
        
        if(direction == M_KEY_UPPER)
        {
            if(elevator_info[MASTER_1].FloorLED[1].bt8.bit0==0 && elevator_info[MASTER_2].FloorLED[1].bt8.bit0==0 )
            //    && object_info.keyFilter != 1)
            {

                if(object_info.keyFilter != 1)
                {
                    APP_COMM_F1Key(index,0);
                    //LOG("keyled off 1 index=%d floor=%d dir=%d\n",index,flootID,direction);
                }
                else
                {
                    //LOG("keyled off 2 index=%d floor=%d dir=%d\n",index,flootID,direction);
                }
                //LOG("keyled off 1 index=%d floor=%d dir=%d %X %X\n",index,flootID,direction,elevator_info[0].FloorLED[0].u8,elevator_info[0].FloorLED[1].u8);
            }
			elevator_info[index].FloorLED[1].bt8.bit0 = 0;
						
        }
    }
    else
    {
        if(direction == M_KEY_UPPER)
        {
            if(elevator_info[MASTER_1].FloorLED[flootID].bt8.bit0==0 && elevator_info[MASTER_2].FloorLED[flootID].bt8.bit0==0)
            {
                object_info.slave_data[flootID].bit16_2.bit4 = 0;
                
            }
            elevator_info[index].FloorLED[flootID].bt8.bit0 = 0;
        }
        else if(direction == M_KEY_LOWER)
        {
            if(elevator_info[MASTER_1].FloorLED[flootID].bt8.bit1==0 && elevator_info[MASTER_2].FloorLED[flootID].bt8.bit1==0)
            {
                object_info.slave_data[flootID].bit16_2.bit6 = 0;
                
            }
            elevator_info[index].FloorLED[flootID].bt8.bit1 = 0;
        }
    }

   // LOG("keyled off index=%d floor=%d dir=%d\n",index,flootID,direction);
    // LOG("%d keyU=%d keyL=%d slave4=%d slave6=%d\n",flootID,
    //             object_info.keyUpper[flootID].u8,object_info.keyLower[flootID].u8,
    //             object_info.slave_data[flootID].bit16_2.bit4,object_info.slave_data[flootID].bit16_2.bit6);
}

void APP_COMM_CanalKey(uint8_t index, uint8_t flootID, uint8_t direction)
{  
    if(flootID==1)
    {
        APP_COMM_F1Key(index,0);
    }
    else
    {
        if(direction == M_KEY_UPPER)
        {
            object_info.slave_data[flootID].bit16_2.bit4 = 0;
            elevator_info[index].FloorLED[flootID].bt8.bit0 = 0;
        }
        else if(direction == M_KEY_LOWER)
        {
            object_info.slave_data[flootID].bit16_2.bit6 = 0;
            elevator_info[index].FloorLED[flootID].bt8.bit1 = 0;
        }
    }
    // LOG("%d keyU=%d keyL=%d slave4=%d slave6=%d\n",flootID,
    //             object_info.keyUpper[flootID].u8,object_info.keyLower[flootID].u8,
    //             object_info.slave_data[flootID].bit16_2.bit4,object_info.slave_data[flootID].bit16_2.bit6);
}

void APP_COMM_F1Key(uint8_t index, uint8_t sta)
{  

    //LOG("%d index%d key sta=%d\n",(uint32_t)HAL_GetTick(),index,sta);
    if(sta)
    {
        BSP_GPIO_LightUpper(1);
    }
    else
    {
        if(elevator_info[MASTER_1].FloorLED[1].bt8.bit0==0 || elevator_info[MASTER_2].FloorLED[1].bt8.bit0==0)
        {
            BSP_GPIO_LightUpper(0);
        }    
        
    }
}
