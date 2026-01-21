#include "app_config.h"
#include "app_user.h"

typedef struct
{
    Timer g_timer_cb[5]     ;//= {0};
    uint8_t g_send_max[5]     ;//= {0};
    uint8_t g_send_buf[5][255];//= {0};
    uint8_t g_send_buf_len[5] ;//= {0};
    uint8_t g_send_index      ;//=0;
} bsp_send_def;
bsp_send_def user_send;

//解析
typedef struct {

    uint8_t dnum     ;
    uint8_t type     ;
    uint16_t cmd      ;
    uint8_t pid      ;
    uint8_t pid_len  ;
    bsp_id_def did  ;
    uint8_t head      ;
    uint8_t pack_num;


} user_com_def;
user_com_def g_comm= {0};
uint8_t send_buff[255]= {0};


static Timer g_timer_FxLoop= {0};
static Timer g_timer_sendEnergy= {0};

static Timer g_timer_FloorBoardLose= {0};

static Timer g_timer_MUpdate= {0};

extern 		Timer g_timer_ledcloce ;

static Timer g_timer_StartCheck= {0};
static Timer g_timer_Inspection= {0};
static Timer g_timer_InspectionHeart= {0};




st_object_info object_info = {0};
st_elevator_info elevator_info[2] ={0};


#ifdef USE_UART4
#define APP_WIRELESS_UART          BSP_UART4
#define APP_WIRELESS_UART_RX_LEN        UART4_RX_LEN
#define	APP_WIRELESS_UART_BUF			 g_uart_buf[APP_WIRELESS_UART]
#endif

#ifdef USE_UART5
#define APP_USER_UART          BSP_UART5
#define APP_USER_UART_RX_LEN        UART5_RX_LEN
#endif

#define	APP_USER_UART_BUF			 g_uart_buf[APP_USER_UART]

enum
{
    INSPERSTION_CTL_START=0,  
    INSPERSTION_CTL_STOP,
    INSPERSTION_CTL_MAX,
};
void APP_COMM_CanalKey(uint8_t index, uint8_t flootID, uint8_t direction);
uint8_t APP_USER_did2Index(uint32_t did);
void APP_USER_UpdateComm_sta(uint8_t index,uint8_t sta);
void APP_COMM_F1Key(uint8_t index, uint8_t sta);
void APP_COMM_clearFlooXList(uint8_t index);
void APP_COMM_supplyStaFx(uint8_t index,uint8_t flootID, uint8_t sta);
void APP_USER_inspectionStart(uint8_t index);
// 发送
void APP_USER_Send(uint8_t *buf, uint16_t buf_len)
{
    if(g_app_cfg.appComAllow == 0)
    {
        return;
    }
    BSP_UART_Transmit(APP_USER_UART,buf,buf_len);
}

// 发送
void APP_WIRELESS_Send(uint8_t *buf, uint16_t buf_len)
{
    if(g_app_cfg.appComAllow == 0)
    {
        return;
    }
    BSP_UART_Transmit(APP_WIRELESS_UART,buf,buf_len);
}
//发送打包
void APP_USER_packet(uint16_t cmd,st_elevator_info *elev_info,uint8_t *buf,uint8_t len,uint8_t type)//type 0 不等应答  1  等应答
{
    uint8_t sendbuf[200]= {0};
    int numb =0;
    sendbuf[numb++]='#';
    sendbuf[numb++]='W';
    sendbuf[numb++]=g_app_cfg.dtp;   //类型
    numb++;

    sendbuf[numb++]=elev_info->did.id_buf[2];  //ID
    sendbuf[numb++]=elev_info->did.id_buf[1];  //ID
    sendbuf[numb++]=elev_info->did.id_buf[0];  //ID
    sendbuf[numb++]=0;  //ID

    sendbuf[numb++]=cmd>>8;//
    sendbuf[numb++]=cmd&0xff;//
    memcpy(sendbuf+numb,buf,len);// 发送
    numb+=len;
    if(type==1)
    {
        sendbuf[numb++]=user_send.g_send_index;//包号
    }
    sendbuf[3]=numb-4;//长度
    uint8_t bcc=BSP_Bcc(sendbuf,numb );
    sendbuf[numb++]=bcc;



    //LOG_HEX(sendbuf, numb);
    APP_USER_Send(sendbuf, numb);

    if(type==1)
    {
//		user_send.g_send_index_save[ user_send.g_send_index]= user_send.g_send_index;
        user_send.g_send_buf_len[user_send.g_send_index] = numb;//发送长度
        memcpy(user_send.g_send_buf[user_send.g_send_index],sendbuf,user_send.g_send_buf_len[user_send.g_send_index]);//保存发送
        BSP_TIMER_Start(&user_send.g_timer_cb[user_send.g_send_index]);//开启超时
        user_send.g_send_max[user_send.g_send_index]=0;//超时计数清0
        if(++user_send.g_send_index>=5)//判断缓存包位置,循环使用
        {
            user_send.g_send_index = 0;
        }
    }
    //
}


//发送打包
void APP_USER_packet_ack(user_com_def *comm,uint8_t *buf,uint8_t len)
{
    uint8_t sendbuf[200]= {0};
    int numb =0;
    sendbuf[numb++]='#';
    sendbuf[numb++]='R';
    sendbuf[numb++]=g_app_cfg.dtp;   //类型
    sendbuf[numb++]= 0;
    sendbuf[numb++]=comm->did.id_buf[2];  //ID
    sendbuf[numb++]=comm->did.id_buf[1];  //ID
    sendbuf[numb++]=comm->did.id_buf[0];  //ID
    sendbuf[numb++]=0;  //ID

    sendbuf[numb++]=comm->cmd>>8;//
    sendbuf[numb++]=comm->cmd&0xff;//
    memcpy(sendbuf+numb,buf,len);// 发送
    numb+=len;
    sendbuf[numb++] = comm->pack_num;
    sendbuf[3]=numb-4;//长度
    
    uint8_t bcc=BSP_Bcc(sendbuf,numb );
    sendbuf[numb++]=bcc;
    //LOG_HEX(sendbuf, numb);
    APP_USER_Send(sendbuf, numb);
}


typedef struct
{
    uint8_t commOk[MASTER_MAX];   //通信状态
    uint8_t AutoModeSta[MASTER_MAX];  //自动状态
    uint8_t NofullSta[MASTER_MAX];  //未满员状态
    uint8_t stopErrSta[MASTER_MAX];  //异常停靠
    uint8_t smartSta[MASTER_MAX];  //智能状态  
    uint8_t sched_No;           //调度左右笼
} st_sched_info;

enum
{
    SCHEDSTA_NG=0, 
    SCHEDSTA_OK,  
    SCHEDSTA_XX //任意
};

st_sched_info stSchedList[] = {
    //通信状态                      自动状态                        未满员状态                       异常停靠状态              智能状态                        调度左右笼
    {SCHEDSTA_OK, SCHEDSTA_OK,      SCHEDSTA_OK,SCHEDSTA_OK,     SCHEDSTA_OK,SCHEDSTA_OK,    SCHEDSTA_OK,SCHEDSTA_OK,   SCHEDSTA_OK,SCHEDSTA_OK,        MASTER_SCHED},
                                                                    
    {SCHEDSTA_OK, SCHEDSTA_OK,      SCHEDSTA_OK,SCHEDSTA_OK,     SCHEDSTA_OK,SCHEDSTA_OK,    SCHEDSTA_OK,SCHEDSTA_OK,   SCHEDSTA_OK,SCHEDSTA_NG,        MASTER_ALL},
    {SCHEDSTA_OK, SCHEDSTA_OK,      SCHEDSTA_OK,SCHEDSTA_OK,     SCHEDSTA_OK,SCHEDSTA_OK,    SCHEDSTA_OK,SCHEDSTA_OK,   SCHEDSTA_NG,SCHEDSTA_OK,        MASTER_ALL},
    {SCHEDSTA_OK, SCHEDSTA_OK,      SCHEDSTA_OK,SCHEDSTA_OK,     SCHEDSTA_OK,SCHEDSTA_OK,    SCHEDSTA_OK,SCHEDSTA_OK,   SCHEDSTA_NG,SCHEDSTA_NG,        MASTER_ALL},

    {SCHEDSTA_OK, SCHEDSTA_XX,      SCHEDSTA_OK,SCHEDSTA_XX,     SCHEDSTA_OK,SCHEDSTA_XX,    SCHEDSTA_OK,SCHEDSTA_XX,   SCHEDSTA_XX,SCHEDSTA_XX,        MASTER_1},
    {SCHEDSTA_XX, SCHEDSTA_OK,      SCHEDSTA_XX,SCHEDSTA_OK,     SCHEDSTA_XX,SCHEDSTA_OK,    SCHEDSTA_XX,SCHEDSTA_OK,   SCHEDSTA_XX,SCHEDSTA_XX,        MASTER_2},
};

uint8_t APP_USER_SupplyListHave(uint8_t floor,uint8_t direction)
{
    uint8_t supply_num,index;
    supply_def supply_tmp;

    uint8_t isSupply = 0;
    uint8_t SupplyDirUpper = 0;
    uint8_t SupplyDirLower = 0;
    for (index=0; index< MASTER_MAX; index++)
    {
        for (supply_num=0; supply_num< elevator_info[index].support_Num; supply_num++)
        {
            supply_tmp.u8 = elevator_info[index].support_data[supply_num];

            
            if(floor == supply_tmp.supply_bit.floor && supply_tmp.supply_bit.gageExt)
            {
                if(supply_tmp.supply_bit.dir == direction)
                {
                    isSupply = 1;
                }  
            }
        }
    }
    return isSupply;
}


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
        ; //不转发
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

    //接应列表里有这个层，就发送取消按键
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
            APP_USER_packet(5001,&elevator_info[index],send_buff,buff_len,1);//type 0 不等应答  1  等应答
    }
}

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
void APP_USER_SendCheckOption(uint8_t index,uint16_t opt, uint8_t event)
{
    uint8_t buff_len = 0;
    uint8_t ctlModeIndex = 0;
    uint8_t com = 0;
    uint8_t i;
    LOG("SendCheckOption index:%d %X %d --",index,opt,event);

    if(opt >= OPT_MAX || opt < OPT_RUNNING)
        return;

    send_buff[buff_len++] = opt;
    send_buff[buff_len++] = event==3?0:1;
    APP_USER_packet(5010,&elevator_info[index],send_buff,buff_len,0);

	LOG_HEX(send_buff,buff_len);
}

//模拟笼内呼叫
void APP_USER_SendSimulationInCall(uint8_t index,uint8_t floor)
{
    uint8_t buff_len = 0;
    uint8_t ctlModeIndex = 0;
    uint8_t com = 0;
    uint8_t i;
LOG("SendSimulationInCall index:%d ",index);
    send_buff[buff_len++] = floor;
    APP_USER_packet(5011,&elevator_info[index],send_buff,buff_len,1);
    
	LOG_HEX(send_buff,buff_len);
}


void APP_USER_Reply_4000(user_com_def *comm,uint8_t pack_num,uint8_t floor)
{
    uint8_t buf_send[32]= {0};
    int numb =0;
    buf_send[numb++]='#';
    buf_send[numb++]='R';
    buf_send[numb++]=g_app_cfg.dtp;  //类型
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
    buf_send[numb++]=g_app_cfg.dtp;  //类型
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

#define MAX_TIME 5
void APP_USER_timeout_callback0(void)
{
    if(++user_send.g_send_max[0]<MAX_TIME) //发送次数有效
    {
        APP_USER_Send(user_send.g_send_buf[0],user_send.g_send_buf_len[0]);//发送缓存包
        BSP_TIMER_Start(&user_send.g_timer_cb[0]);//开启超时
        LOG("XXXXX resend 0\n");
        LOG_HEX(user_send.g_send_buf[1],user_send.g_send_buf_len[1]);
    }
}

void APP_USER_timeout_callback1(void)
{
    if(++user_send.g_send_max[1]<MAX_TIME) //发送次数有效
    {
        APP_USER_Send(user_send.g_send_buf[1],user_send.g_send_buf_len[1]);//发送缓存包
        BSP_TIMER_Start(&user_send.g_timer_cb[1]);//开启超时
        LOG("XXXXX resend 1\n");
        LOG_HEX(user_send.g_send_buf[1],user_send.g_send_buf_len[1]);
    }
}
void APP_USER_timeout_callback2(void)
{
    if(++user_send.g_send_max[2]<MAX_TIME) //发送次数有效
    {
        APP_USER_Send(user_send.g_send_buf[2],user_send.g_send_buf_len[2]);//发送缓存包
        BSP_TIMER_Start(&user_send.g_timer_cb[2]);//开启超时
        LOG("XXXXX resend 2\n");
    }
}
void APP_USER_timeout_callback3(void)
{
    if(++user_send.g_send_max[3]<MAX_TIME) //发送次数有效
    {
        APP_USER_Send(user_send.g_send_buf[3],user_send.g_send_buf_len[3]);//发送缓存包
        BSP_TIMER_Start(&user_send.g_timer_cb[3]);//开启超时
        LOG("XXXXX resend 3\n");
    }
}
void APP_USER_timeout_callback4(void)
{
    if(++user_send.g_send_max[4]<MAX_TIME) //发送次数有效
    {
        APP_USER_Send(user_send.g_send_buf[4],user_send.g_send_buf_len[4]);//发送缓存包
        BSP_TIMER_Start(&user_send.g_timer_cb[4]);//开启超时
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

//点亮层门按键灯
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

//熄灭层门按键灯
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



void APP_COMM_Handle_R(user_com_def *comm,uint8_t *buff,uint8_t len)
{   
    uint8_t flootID;
    uint8_t direction;
    uint8_t index;
	//LOG_HEX(buff,len);
	if(comm->type	== g_app_cfg.dtp)//TYPE=1  len=8
    {

        index = APP_USER_did2Index(comm->did.deviceid);
        if (index>=2)
            return;
         for (int i=0; i<5; i++)
        {
//            if((comm->cmd == 5000)
//                || (comm->cmd == 5001))
//            {
//                LOG("%d cmd=%d [12]=%d,[13]=%d, %d\n",i,comm->cmd,user_send.g_send_buf[i][12],user_send.g_send_buf[i][13],buff[2]);
//            }
//            
            
            
            if((comm->cmd == 5000 && user_send.g_send_buf[i][13]==buff[2])
                || (comm->cmd == 5001 && user_send.g_send_buf[i][12]==buff[2])
                || (comm->cmd == 5013 && user_send.g_send_buf[i][13]==buff[2]))
            {
                BSP_TIMER_Stop(&user_send.g_timer_cb[i]);//停止超时再发送
                // LOG("%d cmd=%d [12]=%d,[13]=%d, %d\n",i,comm->cmd,user_send.g_send_buf[i][12],user_send.g_send_buf[i][13],buff[2]);
            }
            else if(user_send.g_send_buf[i][10]==buff[0])
            {
                BSP_TIMER_Stop(&user_send.g_timer_cb[i]);//停止超时再发送
            }
        }
		//		LOG("comm->cmd %d  \n", comm->cmd);
        
        switch (comm->cmd)
        {
        case 5000://主机应答按键事件-底栏
            // flootID = buff[1];
            // direction = buff[2];

            flootID = buff[0];
            direction = buff[1];
			LOG("5000 ack flooID=%d dir=%d bao=%d\n",flootID,direction,buff[2]);

            break;
        case 5001://从机发送取消按键事-底栏
            flootID = buff[0];
            direction = buff[1];   
            LOG("5001 ack flooID=%d dir=%d bao=%d\n",flootID,direction,buff[2]); 
            //APP_COMM_CanalKey(index, flootID, direction);
            break;
        case 5002://从机发送层门状态-底栏
            break;

        case 5003://从机发送取消按键事-底栏
            break;
        case 5004://从机发送层门状态-底栏
             break;

        case 5009://从机发送层门状态-底栏
             LOG("5009 ack index%d\n",index); 
             break;
        case 5013://巡检应答
             LOG("5013 ack index%d\n",index); 
             break;
        default:
            break;
        }
    }

}


//取消按键
void APP_COMM_4000(user_com_def *comm,uint8_t *buff,uint8_t len)
{

    uint8_t floorID = buff[0];
    uint8_t direction = buff[1];
    uint8_t index;
    index = APP_USER_did2Index(comm->did.deviceid);
    if (index>=2)
        return;

    if(direction >2)
        return;
	//APP_COMM_CanalKey(index, floorID, direction);
}



void APP_COMM_4001(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    uint8_t i;
    bsp_un16_def tmpBit16;
    for (i=0;i<MASTER_MAX;i++)
    {
        if (comm->did.deviceid == elevator_info[i].did.deviceid)
        {
            elevator_info[i].currentFloor=buff[0]; //实时楼层

            //elevator_info[i].currentFloor= 3;
            elevator_info[i].floorHigh.u8_buf[3] =  buff[1];
            elevator_info[i].floorHigh.u8_buf[2] =  buff[2];
            elevator_info[i].floorHigh.u8_buf[1] =  buff[3];
            elevator_info[i].floorHigh.u8_buf[0] =  buff[4];
					
			

            tmpBit16.u8_buf[1] =buff[5];
            tmpBit16.u8_buf[0] =buff[6];
            elevator_info[i].slave_dataComm.bit16_2.bit0 = tmpBit16.bit16_2.bit0; //0x2002/2003

            if (tmpBit16.bit16_2.bit0 == 3)
            {
                elevator_info[i].errStop = 1;
                elevator_info[i].currentFloorSta = 0;
            }
            else
            {
                elevator_info[i].errStop = 0;
                elevator_info[i].currentFloorSta = tmpBit16.bit16_2.bit0;
            }


            elevator_info[i].logoType = buff[7];  //商标显示

            //双笼在线
            if (elevator_info[MASTER_1].MCommSta ==STA_COMM_OK && elevator_info[MASTER_2].MCommSta ==STA_COMM_OK )
            {
                //双笼log不一致，不显示LOGO
                if(elevator_info[MASTER_1].logoType !=elevator_info[MASTER_2].logoType)
                {
                    g_app_cfg.logoType = 1;
                    //LOG("Set LOGO %d\n",g_app_cfg.logoType);
                }
                else if(elevator_info[MASTER_1].logoType !=g_app_cfg.logoType)
                {
                    g_app_cfg.logoType = elevator_info[MASTER_1].logoType;
                    LOG("Set LOGO %d\n",g_app_cfg.logoType);
                    //APP_CONFIG_logoType_Set(buff[7]);
                }
            }
            else
            {
                if(elevator_info[i].logoType !=g_app_cfg.logoType)
                {
                    g_app_cfg.logoType = elevator_info[i].logoType;
                    LOG("Set LOGO %d\n",g_app_cfg.logoType);
                    //APP_CONFIG_logoType_Set(buff[7]);
                }
            }

            if(len >=9)
            {
                tmpBit16.u16 = buff[8];  //满载状态
                elevator_info[i].fullLoad = tmpBit16.bit16_2.bit0 ?1:0;
                elevator_info[i].weightLoad = tmpBit16.bit16.bit0;
                elevator_info[i].peopleLoad = tmpBit16.bit16.bit1;
                static uint8_t fullLoad = 0;
                if (fullLoad != elevator_info[i].fullLoad)
                {
                    LOG("No.%d fullLoad sta=%d %d\n",i,elevator_info[i].fullLoad,tmpBit16.bit16_2.bit0);
                    fullLoad = elevator_info[i].fullLoad;
                }
            }
						//LOG("No.%d weightLoad=%d peopleLoad=%d\n",i,elevator_info[i].weightLoad,elevator_info[i].peopleLoad);
            //LOG("4001 LOGO len=%d\n",len);
            //LOG_HEX(buff,len);
            
        }
        APP_COMM_4009_SetInspectionAllowUpdate(i);
    }
}

uint8_t APP_USER_did2Index(uint32_t did)
{
    uint16_t i;
    for (i=0;i<MASTER_MAX;i++)
    {
        if (did == elevator_info[i].did.deviceid)
        {
            return i;
        }
    }
    return 0xFF;
}

//??
uint8_t APP_USER_supply_update(uint8_t index,uint8_t data)
{

		uint8_t support_floor =  elevator_info[index].support_data[0] &0x7F;
	if(data==1 
            && support_floor==elevator_info[index].currentFloor 
            && elevator_info[index].currentFloorSta == 0)
    {
        elevator_info[index].support_Num = 0;
    }
    else
    {
        elevator_info[index].support_Num = data;
    }
    //LOG("######## support_Num=%d\n",elevator_info[index].support_Num);

}

void APP_COMM_clearFlooXList(uint8_t index)
{
	uint8_t i;
    APP_COMM_KeyLedOff(index,1, M_KEY_UPPER);
    for (i=2; i<MAXFLOOR; i++)
    {

        APP_COMM_supplyStaFx(index,i, 0);
        APP_COMM_KeyLedOff(index,i, M_KEY_UPPER);
        APP_COMM_KeyLedOff(index,i, M_KEY_LOWER);
    }

}

void APP_COMM_supplyStaFx(uint8_t index,uint8_t flootID, uint8_t sta)
{
    if (index == MASTER_1)
    {
        object_info.slave_data[flootID].bit16_2.bit8= sta;
    }
    else if (index == MASTER_2)
    {
        object_info.slave_data[flootID].bit16_2.bit10= sta;
    }  
}


void APP_COMM_4002(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    uint16_t i,supply_num,index;
    uint8_t tmp;
    uint8_t supply_dir;
    
    if (len <2)
        return;

    index = APP_USER_did2Index(comm->did.deviceid);
    if (index>=2)
        return;
	//LOG("####%d 4002 len=%d ",index,len);
    //LOG_HEX(buff,len);
		
    tmp = buff[0]& 0x0F;
    if (elevator_info[index].smart_mode != tmp)
    {
        elevator_info[index].smart_mode = tmp;
        LOG("No%d smart_mode=%d\n",index,elevator_info[index].smart_mode);
    }

    tmp = (buff[0]>>4)& 0x0F;
    if (elevator_info[index].ctlMode != tmp)
    {
        elevator_info[index].ctlMode = tmp;
        LOG("No%d ctl_mode=%d\n",index,elevator_info[index].ctlMode);
    }
    elevator_info[index].slave_dataComm.bit16_2.bit10 = tmp;


    memcpy(&elevator_info[index].support_data[0],&buff[2],buff[1]>MAXFLOOR?MAXFLOOR:buff[1]); //buff[1]  =support_Num


    
    APP_USER_supply_update(index, buff[1]);


     //层门显示接应
    uint8_t isSupply = 0;
    uint8_t SupplyDirUpper = 0;
    uint8_t SupplyDirLower = 0;
    supply_def supply_tmp;
    for (i=1; i<MAXFLOOR+1; i++)
    {
        isSupply = 0;
        SupplyDirUpper = 0;
        SupplyDirLower = 0;
        for (supply_num=0; supply_num< elevator_info[index].support_Num; supply_num++)
        {
            supply_tmp.u8 = elevator_info[index].support_data[supply_num];

            
            if(i == supply_tmp.supply_bit.floor && supply_tmp.supply_bit.gageExt)
            {
                isSupply = 1;
                if(supply_tmp.supply_bit.dir == 0)
                {
                    SupplyDirUpper = 1;
                    
                }
                else
                {
                    SupplyDirLower = 1;
                    
                }

                
            }
        }

        if(!isSupply)
        {
            APP_COMM_supplyStaFx(index,i, 0);
        }
        else
        {
            APP_COMM_supplyStaFx(index,i, 1);
        }


        if (SupplyDirUpper==0)
        {
            APP_COMM_KeyLedOff(index, i, M_KEY_UPPER);
            elevator_info[index].supportFx[i].bit.upper = 1;
        }
        else
        {
            APP_COMM_KeyLedOn(index, i, M_KEY_UPPER);
            elevator_info[index].supportFx[i].bit.upper = 0;
        }
        if (SupplyDirLower==0)
        {
            APP_COMM_KeyLedOff(index, i, M_KEY_LOWER);
            elevator_info[index].supportFx[i].bit.lower = 1;
        }
        else
        {
            APP_COMM_KeyLedOn(index, i, M_KEY_LOWER);
            elevator_info[index].supportFx[i].bit.lower = 0;
        }
        
    }
    
}

void APP_COMM_4003(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    uint8_t com;
    uint8_t index;
  

    index = APP_USER_did2Index(comm->did.deviceid);
    //LOG("SetMode len=%d index=%d\n",len,index);
    if (index>=2)
        return;

    com=buff[0];  //实时楼层

    if(com >MAXFLOOR)
    {  
        LOG("SetMode err c=%d\n",com);
        return;
    }
    object_info.com = com;
   
    LOG("SetMode did=%d com=%d\n",comm->did.deviceid,com);
   
}

void APP_USER_UNLOCK(uint8_t index, uint8_t floor, uint8_t sta)
{

    if(index==MASTER_1)
    {
        object_info.slave_data[floor].bit16.bit2 = sta;
    }
        
    else if(index==MASTER_2)
    {
        object_info.slave_data[floor].bit16.bit3 = sta;
    }

    if(sta)
    {
        elevator_info[index].FxDoorLockTick[floor] = BSP_TIMER_Ticks_Get();
        LOG("index %d floor=%d reset time %lld\n",index,floor,elevator_info[index].FxDoorLockTick[floor]);
    }
        
    //elevator_info[index].FloorACK = floor;
    LOG("UNLOCK L F=%d sta=%d 0x%X\n",floor,sta,object_info.slave_data[floor].u16);
}

void APP_COMM_4004(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    uint8_t floor;
    uint8_t status;
    uint8_t index;
    index = APP_USER_did2Index(comm->did.deviceid);
    if (index>=2)
        return;

    floor=buff[0]; //实时楼层
    status =  buff[1]; 

    if(floor >MAXFLOOR)
    {  
        return;
    }

    if(g_app_cfg.haveLock)
    {
        if(status)
        {
            APP_USER_UNLOCK(index, floor,1);
        }
        else
        {
            APP_USER_UNLOCK(index, floor,0);
        }
    }

    LOG("index=%d haveLock=%d unlock=%d  floor=%d \n",index,g_app_cfg.haveLock,status,floor);
    //LOG_HEX(buff,len);
}


void APP_COMM_4005(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    uint8_t status;
    uint8_t index,i;

    if(object_info.enble == 0)
        return;
    index = APP_USER_did2Index(comm->did.deviceid);

//LOG_HEX(buff,len);
    if (index>=2)
        return;

    if (buff[0] >64)
        return;

    elevator_info[index].startCheckNum = buff[0];
    if (len <(elevator_info[index].startCheckNum*2+1))
    {
        LOG("[ERROR] idx=%d len=%d startCheckNum=%d\n",index,len,elevator_info[index].startCheckNum);
        return;
    }

    for (i=0;i<elevator_info[index].startCheckNum;i++)
    {
        elevator_info[index].startCheckIndex[i] = buff[i+1];
        status = buff[i+1+elevator_info[index].startCheckNum];
        elevator_info[index].startCheckSta[i] = status;
    }

    //双笼都在线
    if ((elevator_info[MASTER_1].MCommSta == STA_COMM_OK) 
        && (elevator_info[MASTER_2].MCommSta == STA_COMM_OK))
    {
        if(elevator_info[MASTER_1].startCheckNum 
            && elevator_info[MASTER_2].startCheckNum 
            && (!object_info.startCheckNum))
        {
            object_info.startCheckNum = elevator_info[MASTER_1].startCheckNum < elevator_info[MASTER_2].startCheckNum ?
                                        elevator_info[MASTER_1].startCheckNum:elevator_info[MASTER_2].startCheckNum;
            
        } 
    }
    //1笼都在线
    else if(elevator_info[MASTER_1].MCommSta==STA_COMM_OK)
    {
        if(elevator_info[MASTER_1].startCheckNum 
            && (!object_info.startCheckNum))
        {
            object_info.startCheckNum = elevator_info[MASTER_1].startCheckNum;
        }
    }
    //2笼都在线
    else if(elevator_info[MASTER_2].MCommSta==STA_COMM_OK)
    {
        if(elevator_info[MASTER_2].startCheckNum 
            && (!object_info.startCheckNum))
        {
            object_info.startCheckNum = elevator_info[MASTER_2].startCheckNum;
        }
    }

    LOG("startCheckNum=%d No1=%d No2=%d\n",object_info.startCheckNum,
                                elevator_info[MASTER_1].startCheckNum,
                                elevator_info[MASTER_2].startCheckNum );


    status = 1;
    if(object_info.startCheckNum)
    {
        for (i=0;i<object_info.startCheckNum;i++)
        {
            //双笼都在线
            if ((elevator_info[MASTER_1].MCommSta == STA_COMM_OK) 
                && (elevator_info[MASTER_2].MCommSta == STA_COMM_OK))
            {
                if(elevator_info[MASTER_1].startCheckIndex[i] 
                        != elevator_info[MASTER_2].startCheckIndex[i])
                {
                    LOG("[ERROR] index diff,No1=%d No2=%d\n",
                                elevator_info[MASTER_1].startCheckIndex[i],
                                elevator_info[MASTER_2].startCheckIndex[i]  );
                    return;

                }
                object_info.startCheckIndex[i] = elevator_info[MASTER_1].startCheckIndex[i];
                //综合结构=检测中
                if(0 ==object_info.startCheckSta[i])
                {
                    //只要一个结果异常 界面就显示异常
                    if(elevator_info[MASTER_1].startCheckSta[i]==2 
                        || elevator_info[MASTER_2].startCheckSta[i]==2)
                    {
                        object_info.startCheckSta[i] = 2;
                    }
                    //显示正常
                    else if(elevator_info[MASTER_1].startCheckSta[i] 
                        || elevator_info[MASTER_2].startCheckSta[i])
                    {
                        object_info.startCheckSta[i] = 1;
                    }
                    else
                    {
                        object_info.startCheckSta[i] = 0;
                    }
                    LOG("[INFO] Set display i=%d idx=%d sta=%d\n",i,
                                object_info.startCheckIndex[i],
                                object_info.startCheckSta[i]);
                    APP_DWIN_SUB_PostSingle(object_info.startCheckNum,i,object_info.startCheckIndex[i],object_info.startCheckSta[i] );
                }
            }
            //1笼都在线
            else
            {
                uint8_t index;
                if(elevator_info[MASTER_1].MCommSta == STA_COMM_OK)
                    index = MASTER_1;
                else
                    index = MASTER_2;
                object_info.startCheckIndex[i] = elevator_info[index].startCheckIndex[i];
                //综合结构=检测中
                if(0 ==object_info.startCheckSta[i])
                {
                    //只要一个结果异常 界面就显示异常
                    if(elevator_info[index].startCheckSta[i]==2)
                    {
                        object_info.startCheckSta[i] = 2;
                    }
                    //显示正常
                    else if(elevator_info[index].startCheckSta[i])
                    {
                        object_info.startCheckSta[i] = 1;
                    }
                    else
                    {
                        object_info.startCheckSta[i] = 0;
                    }
                    LOG("[INFO] Set display i=%d idx=%d sta=%d\n",i,
                                object_info.startCheckIndex[i],
                                object_info.startCheckSta[i]);
                    APP_DWIN_SUB_PostSingle(object_info.startCheckNum,i,object_info.startCheckIndex[i],object_info.startCheckSta[i] );
                }
                
            }
        }

        //所有结果正常或异常，没有检测中的，等待3s跳到主界面
        if(status)
        {
            BSP_TIMER_Modify_Start(&g_timer_StartCheck, 3000);
        }
    }
}

void APP_COMM_4006(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    uint8_t index;
    IEEE754 *tmp_data_p;
    IEEE754 tmp_data;
    index = APP_USER_did2Index(comm->did.deviceid);

 
    if (index>=2)
        return;
#if 1

    tmp_data_p = (IEEE754 *)(&buff[0]);

    tmp_data.u8_buf[0] = tmp_data_p->u8_buf[3];
    tmp_data.u8_buf[1] = tmp_data_p->u8_buf[2];
    tmp_data.u8_buf[2] = tmp_data_p->u8_buf[1];
    tmp_data.u8_buf[3] = tmp_data_p->u8_buf[0];

    if(elevator_info[index].ctlDevSta.u32 != tmp_data.u32)
    {
        elevator_info[index].ctlDevSta.u32 = tmp_data.u32;
        // LOG("EEEE 4006 len=%d sampleDevSta:%X ctlDevSta:%X ",len,elevator_info[index].sampleDevSta.u32,elevator_info[index].ctlDevSta.u32 );
        // LOG_HEX(buff,len);
    }
    
    tmp_data_p = (IEEE754 *)(&buff[4]);

    tmp_data.u8_buf[0] = tmp_data_p->u8_buf[3];
    tmp_data.u8_buf[1] = tmp_data_p->u8_buf[2];
    tmp_data.u8_buf[2] = tmp_data_p->u8_buf[1];
    tmp_data.u8_buf[3] = tmp_data_p->u8_buf[0];
    if(elevator_info[index].sampleDevSta.u32 != tmp_data.u32)
    {
        elevator_info[index].sampleDevSta.u32 = tmp_data.u32;
        // LOG("EEEE 4006 len=%d sampleDevSta:%X ctlDevSta:%X ",len,elevator_info[index].sampleDevSta.u32,elevator_info[index].ctlDevSta.u32 );
        // LOG_HEX(buff,len);
    }
    
    APP_COMM_4009_SetInspectionAllowUpdate(index);
#endif

}   

#define ENERGY_ALL_LEN ((SETPER_CURRENT_RATIO_A-1)*4+(SETPER_MAX-SETPER_CURRENT_RATIO_A)*2)
static uint8_t energy_all_buf[ENERGY_ALL_LEN]= {0};

void APP_COMM_4007(user_com_def *comm,uint8_t *buff,uint8_t len,uint8_t pack_num)
{
    uint8_t i;
    IEEE754 tmpFlt;
    uint8_t buf_send[200]= {0};
    int numb =0;

    uint8_t index = APP_USER_did2Index(comm->did.deviceid);
//LOG_HEX(buff,len);
    if (index>=2)
        return;
#if USE_4007_SRC
    buf_send[numb++]='#';
    buf_send[numb++]='R';
    buf_send[numb++]=g_app_cfg.dtp;  //类型
    numb++;
    buf_send[numb++]=comm->did.id_buf[2];  //ID
    buf_send[numb++]=comm->did.id_buf[1];  //ID
    buf_send[numb++]=comm->did.id_buf[0];  //ID
    buf_send[numb++]=0;  //ID
    buf_send[numb++]=comm->cmd>>8;//
    buf_send[numb++]=comm->cmd&0xff;//

    ///////////////////////////
    //数据
    ///////////////////////////
    //SETPER_POWER_YJ
    tmpFlt.flt = (float)g_energy_data.POWER_YJ;
    buf_send[numb++] = tmpFlt.u8_buf[3];
    buf_send[numb++] = tmpFlt.u8_buf[2];
    buf_send[numb++] = tmpFlt.u8_buf[1];
    buf_send[numb++] = tmpFlt.u8_buf[0];

    //SETPER_POWER_BJ
    tmpFlt.flt = (float)g_energy_data.POWER_BJ;
    buf_send[numb++] = tmpFlt.u8_buf[3];
    buf_send[numb++] = tmpFlt.u8_buf[2];
    buf_send[numb++] = tmpFlt.u8_buf[1];
    buf_send[numb++] = tmpFlt.u8_buf[0];

    //SETPER_LEAK_CURRENT_YJ
    tmpFlt.flt = (float)g_energy_data.LEAKAGE_CURRENT_YJ;
    buf_send[numb++] = tmpFlt.u8_buf[3];
    buf_send[numb++] = tmpFlt.u8_buf[2];
    buf_send[numb++] = tmpFlt.u8_buf[1];
    buf_send[numb++] = tmpFlt.u8_buf[0];

    //SETPER_LEAK_CURRENT_BJ
    tmpFlt.flt = (float)g_energy_data.LEAKAGE_CURRENT_BJ;
    buf_send[numb++] = tmpFlt.u8_buf[3];
    buf_send[numb++] = tmpFlt.u8_buf[2];
    buf_send[numb++] = tmpFlt.u8_buf[1];
    buf_send[numb++] = tmpFlt.u8_buf[0];

    //SETPER_ENVIR_TEMPER_YJ
    tmpFlt.flt = (float)g_energy_data.ENVIR_TEMPER_YJ;
    buf_send[numb++] = tmpFlt.u8_buf[3];
    buf_send[numb++] = tmpFlt.u8_buf[2];
    buf_send[numb++] = tmpFlt.u8_buf[1];
    buf_send[numb++] = tmpFlt.u8_buf[0];

    //SETPER_ENVIR_TEMPER_BJ
    tmpFlt.flt = (float)g_energy_data.ENVIR_TEMPER_BJ;
    buf_send[numb++] = tmpFlt.u8_buf[3];
    buf_send[numb++] = tmpFlt.u8_buf[2];
    buf_send[numb++] = tmpFlt.u8_buf[1];
    buf_send[numb++] = tmpFlt.u8_buf[0];

    //SETPER_CABLE_TEMPER_YJ
    tmpFlt.flt = (float)g_energy_data.CABLE_TEMPER_YJ;
    buf_send[numb++] = tmpFlt.u8_buf[3];
    buf_send[numb++] = tmpFlt.u8_buf[2];
    buf_send[numb++] = tmpFlt.u8_buf[1];
    buf_send[numb++] = tmpFlt.u8_buf[0];

    //SETPER_CABLE_TEMPER_BJ
    tmpFlt.flt = (float)g_energy_data.CABLE_TEMPER_BJ;
    buf_send[numb++] = tmpFlt.u8_buf[3];
    buf_send[numb++] = tmpFlt.u8_buf[2];
    buf_send[numb++] = tmpFlt.u8_buf[1];
    buf_send[numb++] = tmpFlt.u8_buf[0];

    //SETPER_CURRENT_K_A
    for(i=0;i<3;i++)
    {
        buf_send[numb++] = g_energy_data.CURRENT_K[i].u8_buf[3];
        buf_send[numb++] = g_energy_data.CURRENT_K[i].u8_buf[2];
        buf_send[numb++] = g_energy_data.CURRENT_K[i].u8_buf[1];
        buf_send[numb++] = g_energy_data.CURRENT_K[i].u8_buf[0];
    }

    //SETPER_LEAKAGE_CURRENT_K
    buf_send[numb++] = g_energy_data.LEAKAGE_CURRENT_K.u8_buf[3];
    buf_send[numb++] = g_energy_data.LEAKAGE_CURRENT_K.u8_buf[2];
    buf_send[numb++] = g_energy_data.LEAKAGE_CURRENT_K.u8_buf[1];
    buf_send[numb++] = g_energy_data.LEAKAGE_CURRENT_K.u8_buf[0];


    //SETPER_CURRENT_ZERO_A
    for(i=0;i<3;i++)
    {
        buf_send[numb++] = g_energy_data.CURRENT_ZERO[i].u8_buf[3];
        buf_send[numb++] = g_energy_data.CURRENT_ZERO[i].u8_buf[2];
        buf_send[numb++] = g_energy_data.CURRENT_ZERO[i].u8_buf[1];
        buf_send[numb++] = g_energy_data.CURRENT_ZERO[i].u8_buf[0];
    }

    //SETPER_LEAKAGE_CURRENT_ZERO
    buf_send[numb++] = g_energy_data.LEAKAGE_CURRENT_ZERO.u8_buf[3];
    buf_send[numb++] = g_energy_data.LEAKAGE_CURRENT_ZERO.u8_buf[2];
    buf_send[numb++] = g_energy_data.LEAKAGE_CURRENT_ZERO.u8_buf[1];
    buf_send[numb++] = g_energy_data.LEAKAGE_CURRENT_ZERO.u8_buf[0];

    //SETPER_YG_ZERO
    buf_send[numb++] = g_energy_data.ENERGY_YG_ZERO.u8_buf[3];
    buf_send[numb++] = g_energy_data.ENERGY_YG_ZERO.u8_buf[2];
    buf_send[numb++] = g_energy_data.ENERGY_YG_ZERO.u8_buf[1];
    buf_send[numb++] = g_energy_data.ENERGY_YG_ZERO.u8_buf[0];

    //SETPER_ENERGY_K
    buf_send[numb++] = g_energy_data.ENERGY_K.u8_buf[3];
    buf_send[numb++] = g_energy_data.ENERGY_K.u8_buf[2];
    buf_send[numb++] = g_energy_data.ENERGY_K.u8_buf[1];
    buf_send[numb++] = g_energy_data.ENERGY_K.u8_buf[0];

    //SETPER_CURRENT_RATIO_A
    for(i=0;i<3;i++)
    {
        buf_send[numb++] = (uint16_t)(g_energy_data.CURRENT_RATIO[i]>>8);
        buf_send[numb++] = (uint16_t)(g_energy_data.CURRENT_RATIO[i] &0x00FF);
    }

    //SETPER_VOLTAGE_RATIO_A
    for(i=0;i<3;i++)
    {
        buf_send[numb++] = (uint16_t)(g_energy_data.VOLTAGE_RATIO[i]>>8);
        buf_send[numb++] = (uint16_t)(g_energy_data.VOLTAGE_RATIO[i] &0x00FF);
    }

    //SETPER_LEAKAGE_CURRENT_RATIO
    buf_send[numb++] = (uint16_t)(g_energy_data.LEAKAGE_CURRENT_RATIO>>8);
    buf_send[numb++] = (uint16_t)(g_energy_data.LEAKAGE_CURRENT_RATIO &0x00FF);

    //SETPER_ENERGY_SET
    buf_send[numb++] = g_energy_data.ENERGY_YG_ALL.u8_buf[3];
    buf_send[numb++] = g_energy_data.ENERGY_YG_ALL.u8_buf[2];
    buf_send[numb++] = g_energy_data.ENERGY_YG_ALL.u8_buf[1];
    buf_send[numb++] = g_energy_data.ENERGY_YG_ALL.u8_buf[0];

    //SETPER_CURRENT_THRESHOLD
    buf_send[numb++] = g_energy_data.CURRENT_THRESHOLD.u8_buf[3];
    buf_send[numb++] = g_energy_data.CURRENT_THRESHOLD.u8_buf[2];
    buf_send[numb++] = g_energy_data.CURRENT_THRESHOLD.u8_buf[1];
    buf_send[numb++] = g_energy_data.CURRENT_THRESHOLD.u8_buf[0];
    ///////////////////////////
    

    buf_send[numb++]=pack_num;
    buf_send[3]=numb-4;
    uint8_t bcc=BSP_Bcc(buf_send,numb );
    buf_send[numb++]=bcc;
    //LOG_HEX(buf_send, numb);
    APP_USER_Send(buf_send, numb);

#else
    //APP_USER_packet_ack(4007,&elevator_info[index],energy_all_buf,ENERGY_ALL_LEN);
#endif
}   




#if USE_4007_SRC

#else

void APP_COMM_IEEE754_to_buff(uint8_t *dst,IEEE754 *src,uint8_t len)
{
    if(len == 4)
    {
        dst[0] = src->u8_buf[3];
        dst[1] = src->u8_buf[2];
        dst[2] = src->u8_buf[1];
        dst[3] = src->u8_buf[0];
    }
    else if(len == 2)
    {
        dst[0] = src->u8_buf[1];
        dst[1] = src->u8_buf[0];
    }
    else if(len == 1)
    {
        dst[0] = src->u8_buf[0];
    }
    LOG("dstadd %p \n",dst);

}
void APP_COMM_4007_setParam(uint8_t number)
{
    IEEE754 tmpFlt;
    uint8_t len;
    if(number <1 || number >=SETPER_MAX)
        return;

    switch (number)
    {
    case SETPER_POWER_YJ:
        tmpFlt.flt = (float)g_energy_data.POWER_YJ;
        break;
    case SETPER_POWER_BJ:
        tmpFlt.flt = (float)g_energy_data.POWER_BJ;
        break;
    case SETPER_LEAK_CURRENT_YJ:
        tmpFlt.flt = (float)g_energy_data.LEAKAGE_CURRENT_YJ;
        break;
    case SETPER_LEAK_CURRENT_BJ:
        tmpFlt.flt = (float)g_energy_data.LEAKAGE_CURRENT_BJ;
        break;
    case SETPER_ENVIR_TEMPER_YJ:
        tmpFlt.flt = (float)g_energy_data.ENVIR_TEMPER_YJ;
        break;
    case SETPER_ENVIR_TEMPER_BJ:
        tmpFlt.flt = (float)g_energy_data.ENVIR_TEMPER_BJ;
        break;
    case SETPER_CABLE_TEMPER_YJ:
        tmpFlt.flt = (float)g_energy_data.CABLE_TEMPER_YJ;
        break;
    case SETPER_CABLE_TEMPER_BJ:
        tmpFlt.flt = (float)g_energy_data.CABLE_TEMPER_BJ;
        break;
    case SETPER_CURRENT_K_A:
        tmpFlt.flt = g_energy_data.CURRENT_K[0].flt;
        break;
    case SETPER_CURRENT_K_B:
        tmpFlt.flt = g_energy_data.CURRENT_K[1].flt;
        break;
    case SETPER_CURRENT_K_C:
        tmpFlt.flt = g_energy_data.CURRENT_K[2].flt;
        break;
    case SETPER_LEAKAGE_CURRENT_K:
        tmpFlt.flt = g_energy_data.LEAKAGE_CURRENT_K.flt;
        break;
    case SETPER_CURRENT_ZERO_A:
        tmpFlt.flt = g_energy_data.CURRENT_ZERO[0].flt;
        break;
    case SETPER_CURRENT_ZERO_B:
        tmpFlt.flt = g_energy_data.CURRENT_ZERO[1].flt;
        break;
    case SETPER_CURRENT_ZERO_C:
        tmpFlt.flt = g_energy_data.CURRENT_ZERO[2].flt;
        break;
    case SETPER_LEAKAGE_CURRENT_ZERO:
        tmpFlt.flt = g_energy_data.LEAKAGE_CURRENT_ZERO.flt;
        break;
    case SETPER_YG_ZERO:
        tmpFlt.flt = g_energy_data.ENERGY_YG_ZERO.flt;
        break;
    case SETPER_ENERGY_K:
        tmpFlt.flt = g_energy_data.ENERGY_K.flt;
        break;
    case SETPER_CURRENT_RATIO_A:
        tmpFlt.u32 = g_energy_data.CURRENT_RATIO[0];
        break;
    case SETPER_CURRENT_RATIO_B:
        tmpFlt.u32 = g_energy_data.CURRENT_RATIO[1];
        break;
    case SETPER_CURRENT_RATIO_C:
        tmpFlt.u32 = g_energy_data.CURRENT_RATIO[2];
        break;
    case SETPER_VOLTAGE_RATIO_A:
        tmpFlt.u32 = g_energy_data.VOLTAGE_RATIO[0];
        break;
    case SETPER_VOLTAGE_RATIO_B:
        tmpFlt.u32 = g_energy_data.VOLTAGE_RATIO[1];
        break;
    case SETPER_VOLTAGE_RATIO_C:
        tmpFlt.u32 = g_energy_data.VOLTAGE_RATIO[2];
        break;
    case SETPER_LEAKAGE_CURRENT_RATIO:
        tmpFlt.u32 = g_energy_data.LEAKAGE_CURRENT_RATIO;
        break;
    case SETPER_ENERGY_SET:
        tmpFlt.u32 = 1;
        break;
    default:
        break;
    }

    if(number <1 || number >=SETPER_MAX)
        return;
    else if(number <SETPER_CURRENT_RATIO_A)
        len = 4;
    else
        len = 2;
    APP_COMM_IEEE754_to_buff(&energy_all_buf[(number-1)*len],&tmpFlt,len);
    uint8_t j;
    LOG("LOG=%d number=%d\n", ENERGY_ALL_LEN,number);
    for(j=0;j<(ENERGY_ALL_LEN/4);j++)
    {
         LOG("indax=%d %p ", j,(char *)energy_all_buf+j*4);
        LOG_HEX((energy_all_buf+j*4),4);
    }
    if(ENERGY_ALL_LEN%4)
    {
        LOG("indax=%d %p ", j,(char *)energy_all_buf+j*4);
        LOG_HEX((energy_all_buf+j*4),ENERGY_ALL_LEN%4);
    }
    // LOG_HEX(energy_all_buf,ENERGY_ALL_LEN);

}   

#endif

void APP_COMM_4008(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    LOG_HEX(buff, len);
    IEEE754 *tmp_dataFlt_p;
    IEEE754 tmp_dataFlt;
    uint16_t tmp_dataW;
    uint32_t tmp_dataL;
    tmp_dataFlt_p = (IEEE754 *)(&buff[1]);

    uint8_t number;
    number = buff[0];

    if(number <=SETPER_ENERGY_K || number >=SETPER_ENERGY_SET)
    {
        tmp_dataFlt.u8_buf[0] = tmp_dataFlt_p->u8_buf[3];
        tmp_dataFlt.u8_buf[1] = tmp_dataFlt_p->u8_buf[2];
        tmp_dataFlt.u8_buf[2] = tmp_dataFlt_p->u8_buf[1];
        tmp_dataFlt.u8_buf[3] = tmp_dataFlt_p->u8_buf[0];
        tmp_dataL = (uint32_t)tmp_dataFlt.flt;
    }
    else
    {
        tmp_dataFlt.u8_buf[0] = tmp_dataFlt_p->u8_buf[1];
        tmp_dataFlt.u8_buf[1] = tmp_dataFlt_p->u8_buf[0];
        tmp_dataFlt.u8_buf[2] = 0;
        tmp_dataFlt.u8_buf[3] = 0;

        tmp_dataW = tmp_dataFlt.u32;
    }

    switch (number)
    {
    case SETPER_POWER_YJ:
        g_energy_data.POWER_YJ = tmp_dataFlt.flt;
        break;
    case SETPER_POWER_BJ:
        g_energy_data.POWER_BJ = tmp_dataFlt.flt;
        break;
    case SETPER_LEAK_CURRENT_YJ:
        g_energy_data.LEAKAGE_CURRENT_YJ = tmp_dataL;
        break;
    case SETPER_LEAK_CURRENT_BJ:
        g_energy_data.LEAKAGE_CURRENT_BJ = tmp_dataL;
        break;
    case SETPER_ENVIR_TEMPER_YJ:
        g_energy_data.ENVIR_TEMPER_YJ = tmp_dataL;
        break;
    case SETPER_ENVIR_TEMPER_BJ:
        g_energy_data.ENVIR_TEMPER_BJ = tmp_dataL;
        break;
    case SETPER_CABLE_TEMPER_YJ:
        g_energy_data.CABLE_TEMPER_YJ = tmp_dataL;
        break;
    case SETPER_CABLE_TEMPER_BJ:
        g_energy_data.CABLE_TEMPER_BJ = tmp_dataL;
        break;

    case SETPER_CURRENT_K_A:
    case SETPER_CURRENT_K_B:
    case SETPER_CURRENT_K_C:
        g_energy_data.CURRENT_K[number-SETPER_CURRENT_K_A].u32 = tmp_dataFlt.u32;
        break;
    case SETPER_LEAKAGE_CURRENT_K:
        g_energy_data.LEAKAGE_CURRENT_K.u32 = tmp_dataFlt.u32;
        break;

    case SETPER_CURRENT_ZERO_A:
    case SETPER_CURRENT_ZERO_B:
    case SETPER_CURRENT_ZERO_C:
        g_energy_data.CURRENT_ZERO[number-SETPER_CURRENT_ZERO_A].u32 = tmp_dataFlt.u32;
        break;
    case SETPER_LEAKAGE_CURRENT_ZERO:
        g_energy_data.LEAKAGE_CURRENT_ZERO.u32 = tmp_dataFlt.u32;
        break;
    case SETPER_YG_ZERO:
        g_energy_data.ENERGY_YG_ZERO.u32 = tmp_dataFlt.u32;
        break;
    case SETPER_ENERGY_K:
        g_energy_data.ENERGY_K.u32 = tmp_dataFlt.u32;
        break;

    case SETPER_CURRENT_RATIO_A:
    case SETPER_CURRENT_RATIO_B:
    case SETPER_CURRENT_RATIO_C:
        g_energy_data.CURRENT_RATIO[number-SETPER_CURRENT_RATIO_A] = tmp_dataW;
        break;
    case SETPER_VOLTAGE_RATIO_A:
    case SETPER_VOLTAGE_RATIO_B:
    case SETPER_VOLTAGE_RATIO_C:
        g_energy_data.VOLTAGE_RATIO[number-SETPER_VOLTAGE_RATIO_A] = tmp_dataW;
        break;
    case SETPER_LEAKAGE_CURRENT_RATIO:
        g_energy_data.LEAKAGE_CURRENT_RATIO = tmp_dataW;
        break;
    case SETPER_ENERGY_SET: //电能设置
        if (len >3)
        {
            //g_energy_data.ENERGY_YG_ALL.u32 = tmp_dataFlt.u32;
            APP_ENERGY_energySet(tmp_dataFlt.flt);
        }
        // if(tmp_dataW == 1)
        // {
        //     APP_ENERGY_energyClear();
        // }
        break;  
    case SETPER_CURRENT_THRESHOLD: //电流阈值
        if (len >3)
        {
            g_energy_data.CURRENT_THRESHOLD.u32 = tmp_dataFlt.u32;
        }
        // if(tmp_dataW == 1)
        // {
        //     APP_ENERGY_energyClear();
        // }
        break;  
          
    default:
        break;
    }
    APP_ENERGY_setSetInfo();
}   



void APP_COMM_5008_setParameter(uint8_t number)
{
    IEEE754 *tmp_dataFlt_p;
    IEEE754 tmpFlt;
    uint16_t tmp_dataW;
    uint32_t tmp_dataL;
    uint8_t i;
    uint8_t buf_send[20]= {0};
    int buff_len =0;


    buf_send[buff_len++] = number;
    switch (number)
    {
    case SETPER_POWER_YJ:
        tmpFlt.flt = (float)g_energy_data.POWER_YJ;
        buf_send[buff_len++] = tmpFlt.u8_buf[3];
        buf_send[buff_len++] = tmpFlt.u8_buf[2];
        buf_send[buff_len++] = tmpFlt.u8_buf[1];
        buf_send[buff_len++] = tmpFlt.u8_buf[0];
        break;
    case SETPER_POWER_BJ:
        tmpFlt.flt = (float)g_energy_data.POWER_BJ;
        buf_send[buff_len++] = tmpFlt.u8_buf[3];
        buf_send[buff_len++] = tmpFlt.u8_buf[2];
        buf_send[buff_len++] = tmpFlt.u8_buf[1];
        buf_send[buff_len++] = tmpFlt.u8_buf[0];
        break;
    case SETPER_LEAK_CURRENT_YJ:
        tmpFlt.flt = (float)g_energy_data.LEAKAGE_CURRENT_YJ;

        buf_send[buff_len++] = tmpFlt.u8_buf[3];
        buf_send[buff_len++] = tmpFlt.u8_buf[2];
        buf_send[buff_len++] = tmpFlt.u8_buf[1];
        buf_send[buff_len++] = tmpFlt.u8_buf[0];
        break;
    case SETPER_LEAK_CURRENT_BJ:
        tmpFlt.flt = (float)g_energy_data.LEAKAGE_CURRENT_BJ;

        buf_send[buff_len++] = tmpFlt.u8_buf[3];
        buf_send[buff_len++] = tmpFlt.u8_buf[2];
        buf_send[buff_len++] = tmpFlt.u8_buf[1];
        buf_send[buff_len++] = tmpFlt.u8_buf[0];
        break;
    case SETPER_ENVIR_TEMPER_YJ:
        tmpFlt.flt = (float)g_energy_data.ENVIR_TEMPER_YJ;

        buf_send[buff_len++] = tmpFlt.u8_buf[3];
        buf_send[buff_len++] = tmpFlt.u8_buf[2];
        buf_send[buff_len++] = tmpFlt.u8_buf[1];
        buf_send[buff_len++] = tmpFlt.u8_buf[0];
        break;
    case SETPER_ENVIR_TEMPER_BJ:
        tmpFlt.flt = (float)g_energy_data.ENVIR_TEMPER_BJ;

		buf_send[buff_len++] = tmpFlt.u8_buf[3];
		buf_send[buff_len++] = tmpFlt.u8_buf[2];
		buf_send[buff_len++] = tmpFlt.u8_buf[1];
		buf_send[buff_len++] = tmpFlt.u8_buf[0];
        break;
    case SETPER_CABLE_TEMPER_YJ:
        tmpFlt.flt = (float)g_energy_data.CABLE_TEMPER_YJ;

		buf_send[buff_len++] = tmpFlt.u8_buf[3];
		buf_send[buff_len++] = tmpFlt.u8_buf[2];
		buf_send[buff_len++] = tmpFlt.u8_buf[1];
		buf_send[buff_len++] = tmpFlt.u8_buf[0];
        break;
    case SETPER_CABLE_TEMPER_BJ:
        tmpFlt.flt = (float)g_energy_data.CABLE_TEMPER_BJ;

		buf_send[buff_len++] = tmpFlt.u8_buf[3];
		buf_send[buff_len++] = tmpFlt.u8_buf[2];
		buf_send[buff_len++] = tmpFlt.u8_buf[1];
		buf_send[buff_len++] = tmpFlt.u8_buf[0];;
        break;

    case SETPER_CURRENT_K_A:
    case SETPER_CURRENT_K_B:
    case SETPER_CURRENT_K_C:
        buf_send[buff_len++] = g_energy_data.CURRENT_K[number-SETPER_CURRENT_K_A].u8_buf[3];
        buf_send[buff_len++] = g_energy_data.CURRENT_K[number-SETPER_CURRENT_K_A].u8_buf[2];
        buf_send[buff_len++] = g_energy_data.CURRENT_K[number-SETPER_CURRENT_K_A].u8_buf[1];
        buf_send[buff_len++] = g_energy_data.CURRENT_K[number-SETPER_CURRENT_K_A].u8_buf[0];
        break;
    case SETPER_LEAKAGE_CURRENT_K:
        buf_send[buff_len++] = g_energy_data.LEAKAGE_CURRENT_K.u8_buf[3];
		buf_send[buff_len++] = g_energy_data.LEAKAGE_CURRENT_K.u8_buf[2];
		buf_send[buff_len++] = g_energy_data.LEAKAGE_CURRENT_K.u8_buf[1];
		buf_send[buff_len++] = g_energy_data.LEAKAGE_CURRENT_K.u8_buf[0];
        break;

    case SETPER_CURRENT_ZERO_A:
    case SETPER_CURRENT_ZERO_B:
    case SETPER_CURRENT_ZERO_C:
        buf_send[buff_len++] = g_energy_data.CURRENT_ZERO[number-SETPER_CURRENT_ZERO_A].u8_buf[3];
        buf_send[buff_len++] = g_energy_data.CURRENT_ZERO[number-SETPER_CURRENT_ZERO_A].u8_buf[2];
        buf_send[buff_len++] = g_energy_data.CURRENT_ZERO[number-SETPER_CURRENT_ZERO_A].u8_buf[1];
        buf_send[buff_len++] = g_energy_data.CURRENT_ZERO[number-SETPER_CURRENT_ZERO_A].u8_buf[0];

        break;
    case SETPER_LEAKAGE_CURRENT_ZERO:
        buf_send[buff_len++] = g_energy_data.LEAKAGE_CURRENT_ZERO.u8_buf[3];
		buf_send[buff_len++] = g_energy_data.LEAKAGE_CURRENT_ZERO.u8_buf[2];
		buf_send[buff_len++] = g_energy_data.LEAKAGE_CURRENT_ZERO.u8_buf[1];
		buf_send[buff_len++] = g_energy_data.LEAKAGE_CURRENT_ZERO.u8_buf[0];
        break;
    case SETPER_YG_ZERO:
        buf_send[buff_len++] = g_energy_data.ENERGY_YG_ZERO.u8_buf[3];
		buf_send[buff_len++] = g_energy_data.ENERGY_YG_ZERO.u8_buf[2];
		buf_send[buff_len++] = g_energy_data.ENERGY_YG_ZERO.u8_buf[1];
		buf_send[buff_len++] = g_energy_data.ENERGY_YG_ZERO.u8_buf[0];
        break;
    case SETPER_ENERGY_K:
        buf_send[buff_len++] = g_energy_data.ENERGY_K.u8_buf[3];
		buf_send[buff_len++] = g_energy_data.ENERGY_K.u8_buf[2];
		buf_send[buff_len++] = g_energy_data.ENERGY_K.u8_buf[1];
		buf_send[buff_len++] = g_energy_data.ENERGY_K.u8_buf[0];
        break;

    case SETPER_CURRENT_RATIO_A:
    case SETPER_CURRENT_RATIO_B:
    case SETPER_CURRENT_RATIO_C:

        buf_send[buff_len++] = (uint16_t)(g_energy_data.CURRENT_RATIO[number-SETPER_CURRENT_RATIO_A]>>8);
        buf_send[buff_len++] = (uint16_t)(g_energy_data.CURRENT_RATIO[number-SETPER_CURRENT_RATIO_A] &0x00FF);
		
        break;
    case SETPER_VOLTAGE_RATIO_A:
    case SETPER_VOLTAGE_RATIO_B:
    case SETPER_VOLTAGE_RATIO_C:
        buf_send[buff_len++] = (uint16_t)(g_energy_data.VOLTAGE_RATIO[number-SETPER_VOLTAGE_RATIO_A]>>8);
        buf_send[buff_len++] = (uint16_t)(g_energy_data.VOLTAGE_RATIO[number-SETPER_VOLTAGE_RATIO_A] &0x00FF);
        break;
    case SETPER_LEAKAGE_CURRENT_RATIO:
        buf_send[buff_len++] = (uint16_t)(g_energy_data.LEAKAGE_CURRENT_RATIO>>8);
		buf_send[buff_len++] = (uint16_t)(g_energy_data.LEAKAGE_CURRENT_RATIO &0x00FF);
        break;
    case SETPER_ENERGY_SET:
        buf_send[buff_len++] = g_energy_data.ENERGY_YG_ALL.u8_buf[3];
        buf_send[buff_len++] = g_energy_data.ENERGY_YG_ALL.u8_buf[2];
        buf_send[buff_len++] = g_energy_data.ENERGY_YG_ALL.u8_buf[1];
        buf_send[buff_len++] = g_energy_data.ENERGY_YG_ALL.u8_buf[0];
        break;
    case SETPER_CURRENT_THRESHOLD:
        buf_send[buff_len++] = g_energy_data.CURRENT_THRESHOLD.u8_buf[3];
        buf_send[buff_len++] = g_energy_data.CURRENT_THRESHOLD.u8_buf[2];
        buf_send[buff_len++] = g_energy_data.CURRENT_THRESHOLD.u8_buf[1];
        buf_send[buff_len++] = g_energy_data.CURRENT_THRESHOLD.u8_buf[0];
        break;
    default:
        break;
    }
    if (elevator_info[MASTER_1].MCommSta==STA_COMM_OK )
        APP_USER_packet(5008,&elevator_info[MASTER_1],buf_send,buff_len,1);//type 0 不等应答  1  等应答
    if (elevator_info[MASTER_2].MCommSta==STA_COMM_OK )
        APP_USER_packet(5008,&elevator_info[MASTER_2],buf_send,buff_len,1);//type 0 不等应答  1  等应答

    LOG_HEX(buf_send,buff_len);
}   

enum
{
    SETPER_4009_MODE=1,  
    SETPER_4009_ERR_RECOVER,
    SETPER_4009_SET_VOLUME,
    SETPER_4009_SET_PASSWD,
    SETPER_4009_APP_LICENCE,
    SETPER_4009_APP_VERSION_SOFT,
    SETPER_4009_APP_VERSION_HARD,
    SETPER_4009_INSPECTION,
    SETPER_4009_PHONE_1,
    SETPER_4009_PHONE_2,
    SETPER_4009_PHONE_3,
    SETPER_4009_MAX,

};



const uint8_t setper_4009_map[SETPER_4009_MAX][2] = {
    // number          size
    {SETPER_4009_MODE,  1},
    {SETPER_4009_ERR_RECOVER,  1},
    {SETPER_4009_SET_VOLUME,  4},
    {SETPER_4009_SET_PASSWD,  4},
    {SETPER_4009_APP_LICENCE,  1},
    {SETPER_4009_APP_VERSION_SOFT,  3},
    {SETPER_4009_APP_VERSION_HARD,  3},
    {SETPER_4009_INSPECTION,  1},
    {SETPER_4009_PHONE_1,  16},
    {SETPER_4009_PHONE_2,  16},
    {SETPER_4009_PHONE_3,  16},
};

uint8_t APP_COMM_4009_getPerSize(uint8_t number)
{
    uint8_t i;
    for(i=0; i<sizeof(setper_4009_map)/2;i++)
    {
        if(setper_4009_map[i][0] == number)
        {
            return setper_4009_map[i][1];
        }
    }
    return 0;
}

void APP_COMM_AppEnble(uint8_t sta)
{
    if(sta)
    {
        BSP_TIMER_checkKey_SetStatus(1);
        BSP_RN8209D_SetStatus(1);
        APP_ENERGY_SetStatus(1);
        APP_SOUND_SetStatus(1);
        APP_DWIN_SetStatus(1);
        APP_USER_FxLoop_SetStatus(1);
        object_info.enble = 1;
        BSP_TIMER_Modify_Start(&g_timer_FxLoop,100);
    }
    else
    {
        BSP_TIMER_checkKey_SetStatus(0);
        BSP_RN8209D_SetStatus(0);
        APP_ENERGY_SetStatus(0);
        APP_SOUND_SetStatus(0);
        //APP_DWIN_SetStatus(0);
        APP_USER_FxLoop_SetStatus(0);
        object_info.enble = 0;
    }
}
#if 0
void APP_COMM_4009_SetAppLicence(uint8_t index,uint8_t data, uint8_t boot)
{
    uint8_t reIndex,needSave;
    if(index == MASTER_1)
    {
        reIndex = MASTER_2;
    }
    else if(index == MASTER_2)
    {
        reIndex = MASTER_1;
    }
    needSave = APPLICENCE_MAX;
    LOG("Set app Licence index:%d sta:%d boot:%d\n",index, data, boot);
    switch (data)
    {
    case APPLICENCE_NORMAL:

        LOG("APPLICENCE_NORMAL g_app_cfg.appLicence[index]:%d g_app_cfg.appLicence[reindex]:%d\n",g_app_cfg.appLicence[index], g_app_cfg.appLicence[reIndex]);
        if ((g_app_cfg.appLicence[index] ==APPLICENCE_STOP && g_app_cfg.appLicence[reIndex] ==APPLICENCE_NORMAL)
         ||(boot && g_app_cfg.appLicence[MASTER_1] ==APPLICENCE_NORMAL && g_app_cfg.appLicence[MASTER_2] ==APPLICENCE_NORMAL))
        {
            APP_DWIN_SUB_Switch_Page(0);
            APP_COMM_AppEnble(1);
            needSave = APPLICENCE_NORMAL;
        }
        break;
    case APPLICENCE_STOP:
     LOG("APPLICENCE_STOP g_app_cfg.appLicence[index]:%d g_app_cfg.appLicence[reindex]:%d\n",g_app_cfg.appLicence[index], g_app_cfg.appLicence[reIndex]);
        if (g_app_cfg.appLicence[index] ==APPLICENCE_NORMAL ||boot)
        {
            APP_DWIN_SUB_Switch_Page(15);
            APP_COMM_AppEnble(0);
            needSave = APPLICENCE_STOP;
        }
        
        break;
    case APPLICENCE_STOP_SPECIAL:
        APP_DWIN_SUB_Switch_Page(0);
        APP_COMM_AppEnble(0);
        needSave = APPLICENCE_STOP_SPECIAL;
        break;
    case APPLICENCE_TO_NORMAL:
        LOG("APPLICENCE_TO_NORMAL g_app_cfg.appLicence[index]:%d g_app_cfg.appLicence[reindex]:%d\n",g_app_cfg.appLicence[index], g_app_cfg.appLicence[reIndex]);
        if (g_app_cfg.appLicence[index] ==APPLICENCE_STOP_SPECIAL && g_app_cfg.appLicence[reIndex] ==APPLICENCE_NORMAL)
        {
            APP_DWIN_SUB_Switch_Page(0);
            APP_COMM_AppEnble(1);
            needSave = APPLICENCE_NORMAL;
        }
        break;
    default:

        break;
    }

    if(boot ==0 && needSave <APPLICENCE_MAX)
    {
        APP_CONFIG_appLicence_Set(index, needSave);
    }

}

#else
//void APP_Switch_main_Page(void)
//{
//    if(g_app_cfg.devMode == DEV_MODE_ELEVATOR_ROBOT)
//    {
//        APP_DWIN_SUB_Switch_Page(0);
//    }
//    else if(g_app_cfg.devMode == DEV_MODE_ELEVATOR_LIFT_DOUBLE)
//    {
//        APP_DWIN_SUB_Switch_Page(18);
//    }
//    else if(g_app_cfg.devMode == DEV_MODE_ELEVATOR_LIFT_SINGLE)
//    {
//        APP_DWIN_SUB_Switch_Page(19);
//    }
//    else
//    {
//        APP_DWIN_SUB_Switch_Page(0);
//    }
//}


void APP_COMM_4009_SetAppLicence(uint8_t index,uint8_t data, uint8_t boot)
{
    uint8_t reIndex,needSave;
    if(index == MASTER_1)
    {
        reIndex = MASTER_2;
    }
    else if(index == MASTER_2)
    {
        reIndex = MASTER_1;
    }
    needSave = APPLICENCE_MAX;
    LOG("Set app Licence index:%d sta:%d boot:%d\n",index, data, boot);
    switch (data)
    {
    case APPLICENCE_NORMAL:

        LOG("APPLICENCE_NORMAL g_app_cfg.appLicence[%d]:%d\n",index,g_app_cfg.appLicence[index]);
        if ((g_app_cfg.appLicence[index] ==APPLICENCE_STOP) || boot)
        {
            //APP_Switch_main_Page();
            APP_COMM_AppEnble(1);
            needSave = APPLICENCE_NORMAL;
            //APP_DWIN_set_lock_page(index,0);
        }
        break;
    case APPLICENCE_STOP:
        LOG("APPLICENCE_STOP g_app_cfg.appLicence[%d]:%d\n",index,g_app_cfg.appLicence[index], g_app_cfg.appLicence[reIndex]);

        if (g_app_cfg.appLicence[index] ==APPLICENCE_NORMAL || boot)
        {
            if(g_app_cfg.appLicence[reIndex] == APPLICENCE_STOP  || g_app_cfg.devMode == DEV_MODE_ELEVATOR_LIFT_SINGLE)
            {
                //APP_DWIN_SUB_Switch_Page(15);
                APP_COMM_AppEnble(0);
            }

            //APP_DWIN_set_lock_page(index,1);
            needSave = APPLICENCE_STOP;

        }
        break;
    case APPLICENCE_STOP_SPECIAL:
        //APP_Switch_main_Page();
        APP_COMM_AppEnble(1);
        needSave = APPLICENCE_STOP_SPECIAL;
        //APP_DWIN_set_lock_page(index,0);
        break;
    case APPLICENCE_TO_NORMAL:
        LOG("APPLICENCE_TO_NORMAL g_app_cfg.appLicence[index]:%d g_app_cfg.appLicence[reindex]:%d\n",g_app_cfg.appLicence[index], g_app_cfg.appLicence[reIndex]);
        if (g_app_cfg.appLicence[index] ==APPLICENCE_STOP_SPECIAL || boot)
        {
             //APP_Switch_main_Page();
            APP_COMM_AppEnble(1);
            needSave = APPLICENCE_NORMAL;
            //APP_DWIN_set_lock_page(index,0);
        }
        break;
    default:

        break;
    }

    if(boot ==0 && needSave <APPLICENCE_MAX)
    {
        APP_CONFIG_appLicence_Set(index, needSave);
        
    }

}

#endif
void APP_COMM_4009_SetInspectionAllowUpdate(uint8_t index)
{
    uint8_t inspection_allow = 1;
    //LOG("setInspection  index:%d \n",index);

    if(elevator_info[index].inspection_enable)
    {

        if(object_info.inspection_status) //巡检中
        {
            if(elevator_info[index].errStop == 0  //是否异常停靠
                && elevator_info[index].FxDoorSta[1].bt8_2.bit0 == 0  //底栏门状态
                && elevator_info[index].ctlMode == CTL_MODE_AUTO)  //自动模式
            {
                elevator_info[index].inspection_allow = 1;	
            }
            else
            {
                elevator_info[index].inspection_allow = 0;
                inspection_allow = 0;
            }
        }
        else
        {
            if( elevator_info[index].currentFloor ==1 
                && elevator_info[index].errStop ==0
                && elevator_info[index].ctlMode == CTL_MODE_AUTO)  //自动模式
            {
                
                elevator_info[index].inspection_allow = 1;	
            }
            else
            {
                elevator_info[index].inspection_allow = 0;
                inspection_allow = 0;
            }
        }
    }
    else
    {
        elevator_info[index].inspection_allow = 0;
    }

    if(elevator_info[MASTER_1].inspection_enable || elevator_info[MASTER_2].inspection_enable)
    {
        if(elevator_info[MASTER_1].inspection_allow && elevator_info[MASTER_2].inspection_allow)
        {
            if(object_info.inspection_allow != inspection_allow)
                LOG("\nIndex:%d  inspection_allow=%d\n",index,inspection_allow);
            object_info.inspection_allow = 1;
        }
        else
        {
            object_info.inspection_allow = 0;
        }

    }
    else
    {
        object_info.inspection_allow = 0;
    }

    //  LOG("\n!!!%d  %02X %02X %02X\n",index,elevator_info[index].inspection_enable,elevator_info[index].currentFloor,
	//		elevator_info[index].errStop);
	//	LOG("***%d  %08X %08X %02X\n\n",index,elevator_info[index].ctlDevSta.u32,elevator_info[index].sampleDevSta.u32,elevator_info[index].F1Sta.u8);

    //巡检执行中
    if(object_info.inspection_status &&  object_info.inspection_allow == 0)
    {
		LOG("Inspection err\n");
        APP_USER_inspectionForceStop();
        LOG("\n!!!%d  %02X %02X %02X\n",index,elevator_info[index].inspection_enable,elevator_info[index].currentFloor,
			elevator_info[index].errStop);
		LOG("***%d  %08X %08X %02X\n\n",index,elevator_info[index].ctlDevSta.u32,elevator_info[index].sampleDevSta.u32,elevator_info[index].F1Sta.u8);
    }
}

void APP_COMM_4009_SetInspectionEnable(uint8_t index,uint8_t enable)
{

    if(enable != elevator_info[index].inspection_enable)
    {
        APP_MEMORY_Write_U16(CFG_ADDR_INSP_EN_0+index, enable);
        LOG("Index:%d setInspection save enable:%d\n",index,enable);
    }
    elevator_info[index].inspection_enable = enable;
    
    LOG("Index:%d setInspection enable:%d\n",index,enable);

    if(elevator_info[MASTER_1].inspection_enable || elevator_info[MASTER_2].inspection_enable)
    {
        object_info.inspection_enable=1;
    }
    if(elevator_info[MASTER_1].inspection_enable==0 && elevator_info[MASTER_2].inspection_enable==0)
    {
        object_info.inspection_enable=0;
    }

    APP_COMM_4009_SetInspectionAllowUpdate(index);
}

void APP_COMM_4009(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    //LOG_HEX(buff, len);
    IEEE754 *tmp_dataFlt_p;
    IEEE754 tmp_dataFlt;
    tmp_dataFlt_p = (IEEE754 *)(&buff[1]);

    uint8_t number;
    uint8_t perSize;
    number = buff[0];
    uint8_t index = APP_USER_did2Index(comm->did.deviceid);

        
    switch (APP_COMM_4009_getPerSize(number))
    {
    case 1:
        tmp_dataFlt.u8_buf[0] = tmp_dataFlt_p->u8_buf[0];
        tmp_dataFlt.u8_buf[1] = 0;
        tmp_dataFlt.u8_buf[2] = 0;
        tmp_dataFlt.u8_buf[3] = 0;
        break;
    case 2:
        tmp_dataFlt.u8_buf[0] = tmp_dataFlt_p->u8_buf[1];
        tmp_dataFlt.u8_buf[1] = tmp_dataFlt_p->u8_buf[0];
        tmp_dataFlt.u8_buf[2] = 0;
        tmp_dataFlt.u8_buf[3] = 0;
        break;
    case 3:
        tmp_dataFlt.u8_buf[0] = tmp_dataFlt_p->u8_buf[2];
        tmp_dataFlt.u8_buf[1] = tmp_dataFlt_p->u8_buf[1];
        tmp_dataFlt.u8_buf[2] = tmp_dataFlt_p->u8_buf[0];
        tmp_dataFlt.u8_buf[3] = 0;
        break;
    case 4:
        tmp_dataFlt.u8_buf[0] = tmp_dataFlt_p->u8_buf[3];
        tmp_dataFlt.u8_buf[1] = tmp_dataFlt_p->u8_buf[2];
        tmp_dataFlt.u8_buf[2] = tmp_dataFlt_p->u8_buf[1];
        tmp_dataFlt.u8_buf[3] = tmp_dataFlt_p->u8_buf[0];
        break;
    default:
        tmp_dataFlt.u32 = 0;
        break;
    }

    switch (number)
    {
    case SETPER_4009_SET_VOLUME:
        APP_SOUND_User_setVol(tmp_dataFlt.i32);
        break;
    case SETPER_4009_SET_PASSWD:
        APP_CONFIG_PASSWD_Set(tmp_dataFlt.i32);
        break;
    case SETPER_4009_APP_LICENCE:
       APP_COMM_4009_SetAppLicence(index, tmp_dataFlt.u32, 0);
    
    case SETPER_4009_INSPECTION:
       APP_COMM_4009_SetInspectionEnable(index, tmp_dataFlt.u32);
        if(object_info.inspection_atPage) //如果云集界面，则停止巡检
        {
            APP_USER_inspectionForceStop();
        }
        break;

    case SETPER_4009_PHONE_1:
        memcpy((char *)g_app_cfg.phone_num1 ,(char *)(&buff[1]),16);
        g_app_cfg.phone_num1[15] = 0xFF;
        APP_CONFIG_phone_Num_Set((char *)g_app_cfg.phone_num1,0);
        break;
    case SETPER_4009_PHONE_2:
        memcpy((char *)g_app_cfg.phone_num2 ,(char *)(&buff[1]),16);
        g_app_cfg.phone_num2[15] = 0xFF;
        APP_CONFIG_phone_Num_Set((char *)g_app_cfg.phone_num2,1);
        break;
    case SETPER_4009_PHONE_3:
        memcpy((char *)g_app_cfg.phone_num3 ,(char *)(&buff[1]),16);
        g_app_cfg.phone_num3[15] = 0xFF;
        APP_CONFIG_phone_Num_Set((char *)g_app_cfg.phone_num3,2);
        break;
    default:
        break;
    }

}


void APP_COMM_4010(user_com_def *comm,uint8_t *buff,uint8_t len)
{
   uint8_t i,iSize,needSave;
   uint8_t number,size;

    IEEE754 tmpFlt;
    IEEE754 tmpVer;
    uint8_t buf_send[200]= {0};
    int numb =0;

    uint8_t index = APP_USER_did2Index(comm->did.deviceid);
//LOG_HEX(buff,len);
    if (index>=2)
        return;
    
    for(i=0; i<sizeof(setper_4009_map)/2;i++)
    {
        number = setper_4009_map[i][0];
        size = setper_4009_map[i][1];
        switch (number)
        {
            case SETPER_4009_MODE:
                tmpFlt.u32 = 0;
                break;
            case SETPER_4009_ERR_RECOVER:
                tmpFlt.u32 = 0;
                break;
            case SETPER_4009_SET_VOLUME:
                tmpFlt.i32 = APP_SOUND_User_getVol();
                break;
            case SETPER_4009_SET_PASSWD:
                tmpFlt.i32 = APP_CONFIG_PASSWD_Get();
                break;
            case SETPER_4009_APP_LICENCE:
                tmpFlt.u32 = APP_CONFIG_appLicence_Get(index);
                break;
            case SETPER_4009_APP_VERSION_SOFT:
                tmpVer.u32 = 0;
                APP_VERSION_Get_Soft(tmpVer.u8_buf);
                tmpFlt.u8_buf[3] = 0;
                tmpFlt.u8_buf[2] = tmpVer.u8_buf[0];
                tmpFlt.u8_buf[1] = tmpVer.u8_buf[1];
                tmpFlt.u8_buf[0] = tmpVer.u8_buf[2];
                break;
            case SETPER_4009_APP_VERSION_HARD:
                tmpVer.u32 = 0;
                APP_VERSION_Get_Hard(tmpVer.u8_buf);
                tmpFlt.u8_buf[3] = 0;
                tmpFlt.u8_buf[2] = tmpVer.u8_buf[0];
                tmpFlt.u8_buf[1] = tmpVer.u8_buf[1];
                tmpFlt.u8_buf[0] = tmpVer.u8_buf[2];
                break;
            case SETPER_4009_INSPECTION:
                LOG("inspection_enable=%d index:%d\n",elevator_info[index].inspection_enable,index);
                tmpFlt.u8_buf[3] = 0;
                tmpFlt.u8_buf[2] = 0;
                tmpFlt.u8_buf[1] = 0;
                tmpFlt.u8_buf[0] = elevator_info[index].inspection_enable;
                break;
            case SETPER_4009_PHONE_1:
                for(iSize=0; iSize<16; iSize++)
                {
                    buf_send[numb++] = g_app_cfg.phone_num1[iSize];
                }
                break;
            case SETPER_4009_PHONE_2:
                for(iSize=0; iSize<16; iSize++)
                {
                    buf_send[numb++] = g_app_cfg.phone_num2[iSize];
                }
                break;
            case SETPER_4009_PHONE_3:
                for(iSize=0; iSize<16; iSize++)
                {
                    buf_send[numb++] = g_app_cfg.phone_num3[iSize];
                }
                break;
            default:

                break;
        }
        if(size <= 4)
        {
            while (size--)
            {
                buf_send[numb++] = tmpFlt.u8_buf[size];
            }
        }
    }

    APP_USER_packet_ack(comm,buf_send,numb);
}

void APP_COMM_4012(user_com_def *comm,uint8_t *buff,uint8_t len)
{
   uint8_t i,iSize;
   uint8_t number,xFloorMapChang;

    uint8_t index = APP_USER_did2Index(comm->did.deviceid);
    uint8_t tmpData;
    
//LOG_HEX(buff,len);
    if (index>=2)
        return;
    number = buff[0];
    if(number >MAXFLOOR && len <number+1)
        return;
    xFloorMapChang = 0;
    object_info.xFloorMapNum = number;
    LOG("mapFloor ");
    for(i=1; i<number+1;i++)
    {
        tmpData = object_info.xFloor[i].cFloor;
        if(tmpData != buff[i])
        {
            xFloorMapChang = 1;
        }
        object_info.xFloor[i].cFloor = buff[i];
        LOG(" %d:%d",i,object_info.xFloor[i].cFloor);
    }
    LOG("\n");
    if(xFloorMapChang)
    {
        APP_USER_saveMapFloor();
    }
}

enum
{
    SETPER_4013_INSPERSTION_CTL=1,  
    SETPER_4013_INSPERSTION_EVENT,
    SETPER_4013_INSPERSTION_HEAT,
    SETPER_4013_MAX,

};

void APP_COMM_setInspectionTrigger(uint8_t status)
{
    uint8_t index;
    uint8_t buf_send[10]= {0};
    int numb =0;
    buf_send[numb++] = SETPER_4013_INSPERSTION_CTL;
    buf_send[numb++] = (INSPERSTION_CTL_START == status) ? INSPERSTION_CTL_START: INSPERSTION_CTL_STOP;
    for(index=0;index <MASTER_MAX;index++)
    {
        if((object_info.inspection_allow &&elevator_info[index].inspection_allow) || status==INSPERSTION_CTL_STOP)
        {
            LOG("Index:%d Inspection %s... \n",index ,(INSPERSTION_CTL_START == status) ? "start" : "stop");	
            if(INSPERSTION_CTL_START == status)
            {       
                APP_USER_inspectionStart(index);
            }
            APP_USER_packet(5013,&elevator_info[index],buf_send,numb,1);
        }
        else
        {
            LOG("Index:%d Inspection not allow %s... all_allow:%d el_allow:%d \n",index ,
                (INSPERSTION_CTL_START == status) ? "start" : "stop",object_info.inspection_allow,elevator_info[index].inspection_allow);
        }
    }
    
}

//开始巡检 跳转界面
void APP_USER_inspectionStart(uint8_t index)
{
    elevator_info[index].inspection_status = 1;
    object_info.inspection_status = 1;
    APP_DWIN_SUB_Switch_Page(DWIN_PAGE_N17);
    object_info.inspection_atPage = 1;
    LOG("Index:%d Into inspection windows\n",index);
    BSP_TIMER_Modify_Start(&g_timer_InspectionHeart,120000);
}

//开始巡检准备 等待采集板应答 定时器Callback
void APP_USER_inspectionReady(void)
{
    
    if(object_info.inspection_status == 0)
    {
        LOG("Ready timeout inspection\n");
        APP_COMM_setInspectionTrigger(INSPERSTION_CTL_START);
    }
    else
    {
        LOG("Don't rep inspection\n");
        //APP_USER_inspectionForceStop();
    }

    //APP_USER_inspectionStart(0);
}

void APP_USER_inspectionHeart(void)
{
    LOG("Heart timeout inspection\n");
    APP_USER_inspectionForceStop();
}

//开始巡检准备 等待采集板应答
void APP_USER_inspectionTrigger(void)
{
    if(object_info.inspection_enable)
    {
        APP_COMM_4009_SetInspectionAllowUpdate(0);
        APP_COMM_4009_SetInspectionAllowUpdate(1);
        LOG("----Inspection not allow %d %d %d\n",object_info.inspection_allow,elevator_info[0].inspection_allow ,elevator_info[1].inspection_allow);
        if(object_info.inspection_allow)
        {
            object_info.inspection_event = INSPECTION_START;
            object_info.inspection_count = 0;
            //APP_DWIN_SUB_Display_16();
            BSP_TIMER_Modify_Start(&g_timer_Inspection,1000);
            APP_DWIN_SUB_Switch_Page(DWIN_PAGE_N16);
            APP_DWIN_play_inspection_status(INSPECTION_START);
            LOG("Inspection ready...\n");
        }
        else
        {
            LOG("Inspection not allow\n");
        }

    }
    else
    {
        LOG("Inspection not enable\n");
    }
		
		LOG("###%d  %02X %02X %02X\n",0,elevator_info[0].inspection_enable,elevator_info[0].currentFloor,
			elevator_info[0].errStop);
		LOG("@@@%d  %08X %08X %02X\n",0,elevator_info[0].ctlDevSta.u32,elevator_info[0].sampleDevSta.u32,elevator_info[0].F1Sta.u8);

		LOG("###%d  %02X %02X %02X\n",1,elevator_info[1].inspection_enable,elevator_info[1].currentFloor,
			elevator_info[1].errStop);
		LOG("@@@%d  %08X %08X %02X\n",1,elevator_info[1].ctlDevSta.u32,elevator_info[1].sampleDevSta.u32,elevator_info[1].F1Sta.u8);



}

//强制停止巡检
void APP_USER_inspectionForceStop(void)
{
    LOG("ForceStop inspection\n");
    APP_USER_inspectionStop(MASTER_1);
    APP_USER_inspectionStop(MASTER_2);
    APP_COMM_setInspectionTrigger(INSPERSTION_CTL_STOP);
    BSP_TIMER_Stop(&g_timer_InspectionHeart);

}

//停止巡检
void APP_USER_inspectionStop(uint8_t index)
{
    elevator_info[index].inspection_status = 0;
    LOG("Index:%d Stop inspection windows \n",index);
    if(elevator_info[MASTER_1].inspection_status == 0 
        && elevator_info[MASTER_2].inspection_status == 0)
    {   
        object_info.inspection_status = 0;
        BSP_TIMER_Stop(&g_timer_Inspection);
        object_info.inspection_event = INSPECTION_NULL;
        APP_DWIN_SUB_Switch_Page(DWIN_PAGE_N0);
        object_info.inspection_atPage = 0;
        APP_DWIN_SUB_Event_clean();  //清除巡检过程中残留的播报状态
        LOG("exit inspection windows\n");
    }
}

void APP_COMM_4013(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    //LOG_HEX(buff, len);
    uint8_t buf_send[10]= {0};
    char voice_buf[10] = {0};
    int numb =0;

    uint8_t number;
    uint8_t ret = 0; //默认成功
    uint8_t data_tmp;
    number = buff[0];
    data_tmp = buff[1];
    uint8_t index = APP_USER_did2Index(comm->did.deviceid);

    switch (number)
    {
    case SETPER_4013_INSPERSTION_CTL:
        if(data_tmp == 1) //巡检停止
        {
            LOG("Index:%d Stop inspection\n",index);
            APP_USER_inspectionStop(index);
        }
        else if(data_tmp == 2) //巡检异常停止
        {
            LOG("Index:%d Abort inspection\n",index);
            APP_USER_inspectionForceStop();
        }
        else if(data_tmp == 0) //巡检开始
        {
            APP_USER_inspectionTrigger();
        }
        break;
    case SETPER_4013_INSPERSTION_EVENT:
        if (data_tmp <= 30 && object_info.inspection_allow)
        {
            LOG("Index:%d CountDown:%d inspection\n",index,data_tmp);
            BSP_TIMER_Modify_Start(&g_timer_InspectionHeart,120000);
						if(object_info.inspection_count !=0 && data_tmp >= object_info.inspection_count)
								break;
            object_info.inspection_count = data_tmp;
            if(data_tmp == 30)
            {
                object_info.inspection_event = INSPECTION_TIMEOUT_START;
                APP_DWIN_play_inspection_status(INSPECTION_TIMEOUT_START);
            }
            else if(data_tmp == 0)
            {
                if (object_info.inspection_status==0)
                {
                    object_info.inspection_event = INSPECTION_START;
                    APP_USER_inspectionTrigger();
                }

            }
            else if(data_tmp == 12)
            {
                object_info.inspection_event = INSPECTION_TIMEOUT;
                APP_DWIN_play_inspection_status(INSPECTION_TIMEOUT);
            }
            else if(data_tmp <= 10)
            {
                object_info.inspection_event = INSPECTION_TIMEOUT_COUT;
                APP_DWIN_play_inspection_status(INSPECTION_TIMEOUT_COUT);
            } 
            APP_DWIN_SUB_Switch_Page(DWIN_PAGE_N16);
            object_info.inspection_atPage = 1;
        }
        else
        {
             ret = 1;  //返回不允许巡检
        }
        break;
    case SETPER_4013_INSPERSTION_HEAT: //巡检心跳
        BSP_TIMER_Modify_Start(&g_timer_InspectionHeart,120000);
        LOG("Index:%d Heart inspection\n",index);
        break;    
    default:
        break;
    }
    buf_send[numb++] = number;
    buf_send[numb++] = ret;

    APP_USER_packet_ack(comm,buf_send,numb);
}

void APP_COMM_Handle_W(user_com_def *comm,uint8_t *buff,uint8_t len)
{
//
    //if(comm->type== g_app_cfg.dtp)//TYPE=1  len=8    
    comm->pack_num = buff[len-2];
    if(comm->type== 100)//TYPE=1  len=8                 {
    {
//                    LOGT(" cmd  pack is [%d][%d]\n",cmd_1,APP_USER_UART_BUF.rxBuf[10]);
        switch (comm->cmd)
        {
        case 4000://采集主机 按键取消事件
            if(buff[0]>MAXFLOOR)
                return;
            APP_COMM_4000(comm,buff,len);
            APP_USER_Reply_4000(comm,buff[1],buff[0]);
            break;
        case 4001://发送部分实时数据到从机
            if(buff[0]>MAXFLOOR)
                return;
            APP_COMM_4001(comm,buff,len); //实时数据 无需应答
            //APP_USER_Reply(comm,buff[len-2],buff[0]);
            break;
        case 4002:
            APP_COMM_4002(comm,buff,len); //实时数据 无需应答
            //APP_USER_Reply(comm,buff[len-2],buff[0]);
            break;
        case 4003://设置楼层和模式
            APP_COMM_4003(comm,buff,len);
            APP_USER_Reply(comm,buff[len-2]);
            break;
        case 4004://开层门锁
            APP_COMM_4004(comm,buff,len);
            APP_USER_Reply(comm,buff[len-2]);
            break;
        case 4005://自检列表
            APP_COMM_4005(comm,buff,len);
            APP_USER_Reply(comm,buff[len-2]);
            break;
        case 4006://电能实时数据
            APP_COMM_4006(comm,buff,len);
            break;
        case 4007://获取电能配置数据
            APP_COMM_4007(comm,buff,len,buff[len-2]);
            break;
        case 4008://获取电能配置数据
            APP_COMM_4008(comm,buff,len);
            break;
        case 4009://采集板->底栏设置
            APP_COMM_4009(comm,buff,len);
            APP_USER_Reply(comm,buff[len-2]);
            break;
        case 4010://采集板->底栏获取设置
            APP_COMM_4010(comm,buff,len);
            break;
        case 4012://采集板->底栏设置映射
            APP_COMM_4012(comm,buff,len);
			APP_USER_Reply(comm,buff[len-2]);
            break;
        case 4013://采集板->底栏巡检
            APP_COMM_4013(comm,buff,len);
            break;
        default:
            break;
        }
    }
}

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

void APP_USER_Handle06(uint8_t addr,const uint8_t *buffer,uint8_t size)
{
    bsp_un16_def src = {0};
    src.u8_buf[1] = buffer[0];
    src.u8_buf[0] = buffer[1];
    uint16_t reg = src.u16; //寄存器地址
    src.u8_buf[1] = buffer[2];
    src.u8_buf[0] = buffer[3];
    uint16_t data = src.u16; //寄存器地址
    switch(reg)
    {
    case 0: //开始
        break;
    }
    BSP_MODBUS_SendRtu_06(addr,reg,data);
}


void APP_USER_Handle03(uint8_t addr,const uint8_t *buffer,uint8_t size)
{

    object_info.times[addr]=0;
    uint8_t currentFlooer = addr;    
    uint8_t tmpData,i;

    bsp_un16_def data; //寄存器地址
    data.u8_buf[1] = buffer[1];
    data.u8_buf[0] = buffer[2];
    if(size==2)
    {

        //有应答
        if (object_info.sendID == addr)
        {
            object_info.sendID = 0;
        }

        //更新各个层级通信状态
        object_info.FloorBoardLoseCount[addr] = 0;

#if 0
        for(i=0;i <MASTER_MAX;i++)
        {
            if (elevator_info[i].FloorACK == addr)
            {
                // if(object_info.slave_data[addr].u16 & (i==MASTER_1?S_DOOR_LOCK_1:S_DOOR_LOCK_2))
                // {
                //     object_info.slave_data[addr].u16 &= (i==MASTER_1?S_DOOR_LOCK_MASK_1:S_DOOR_LOCK_MASK_2);
                //     LOG("UNLOCK ACK %d\n",i);
                // }


                if(object_info.slave_data[addr].bit16.bit2)
                    object_info.slave_data[addr].bit16.bit2 = 0;
                if(object_info.slave_data[addr].bit16.bit3)
                    object_info.slave_data[addr].bit16.bit3 = 0;

                elevator_info[i].FloorACK = 0;
            }
        }
#endif      
        //当前楼层上按键按下
        if (data.bit16.bit0)
        {
            if (!object_info.keyUpper[currentFlooer].bt8.bit0 )
            
            {
                LOG("key Send flooID=%d dir=%d\n",currentFlooer,M_KEY_UPPER);
                APP_USER_SendKeyEvnet(currentFlooer, M_KEY_UPPER);
                object_info.keyUpper[currentFlooer].bt8.bit0 = 1;
                
                //调试
                object_info.keyFloor = currentFlooer;
                object_info.keyFloorDir = 1;
                object_info.keyFloorTick =  BSP_TIMER_Ticks_Get();
                //APP_DWIN_SendKeyEvnet(currentFlooer);
            }
        }
        else 
        {
            object_info.keyUpper[currentFlooer].bt8.bit0 = 0;
        }

        //当前楼层下按键按下
        if (data.bit16.bit2)
        {
            if (!object_info.keyLower[currentFlooer].bt8.bit0)
            {
                LOG("key Send flooID=%d dir=%d\n",currentFlooer,M_KEY_LOWER);
                APP_USER_SendKeyEvnet(currentFlooer, M_KEY_LOWER);
                object_info.keyLower[currentFlooer].bt8.bit0 = 1;

                //调试
                object_info.keyFloor = currentFlooer;
                object_info.keyFloorDir = 2;
                object_info.keyFloorTick =  BSP_TIMER_Ticks_Get();
                //APP_DWIN_SendKeyEvnet(currentFlooer);
            }
            
        }
        else 
        {
            object_info.keyLower[currentFlooer].bt8.bit0 = 0;
        }

        //长按 当前楼层上按键按下
        if (data.bit16.bit1)
        {
            if (!object_info.keyUpper[currentFlooer].bt8.bit1)
            {
  
                LOG("Longkey Send flooID=%d dir=%d keyU=%d\n",currentFlooer,M_KEY_UPPER,object_info.keyUpper[currentFlooer].u8);
                APP_USER_SendCancelKeyEvnet(currentFlooer, M_KEY_UPPER);
                object_info.keyUpper[currentFlooer].bt8.bit1 = 1;
            }
        }
        else
        {
            object_info.keyUpper[currentFlooer].bt8.bit1 = 0;
        }

        //当前楼层下按键按下
        if (data.bit16.bit3)
        {
            if (!object_info.keyLower[currentFlooer].bt8.bit1)
            {
                LOG("Longkey Send flooID=%d dir=%d keyL=%d\n",currentFlooer,M_KEY_LOWER,object_info.keyLower[currentFlooer].u8);
                APP_USER_SendCancelKeyEvnet(currentFlooer, M_KEY_LOWER);
                object_info.keyLower[currentFlooer].bt8.bit1 = 1;
            }
        }
        else
        {
            object_info.keyLower[currentFlooer].bt8.bit1 = 0;
        }

        // LOG("le03 %d keyU=%d keyL=%d slave4=%d slave6=%d\n",currentFlooer,
        //         object_info.keyUpper[currentFlooer].u8,object_info.keyLower[currentFlooer].u8,
        //         object_info.slave_data[currentFlooer].bit16_2.bit4,object_info.slave_data[currentFlooer].bit16_2.bit6);

        //门未关状态处理
        tmpData = data.bit16_2.bit4;
        if(tmpData != (elevator_info[MASTER_1].FxDoorSta[currentFlooer].bt8_2.bit0))
        {
            LOG("FDoor No.1 flooID=%d dir=%d\n",currentFlooer,tmpData);
            elevator_info[MASTER_1].FxDoorSta[currentFlooer].bt8_2.bit0 = tmpData;

            APP_USER_SendDoorStatusEvnet();
        }
        
        tmpData =  data.bit16_2.bit6;
        if(tmpData != (elevator_info[MASTER_2].FxDoorSta[currentFlooer].bt8_2.bit0))
        {
            LOG("FDoor No.2 flooID=%d dir=%d\n",currentFlooer,tmpData);
            elevator_info[MASTER_2].FxDoorSta[currentFlooer].bt8_2.bit0 = tmpData;
            APP_USER_SendDoorStatusEvnet();
        }

        //层门锁处理
        tmpData = data.bit16_2.bit8;
        //LOG("FDoor Lock No.1 flooID=%d sta=%02X\n",currentFlooer,tmpData);
        if(tmpData != (elevator_info[MASTER_1].FxDoorSta[currentFlooer].bt8_2.bit2))
        {
            //LOG("FDoor Lock No.1 flooID=%d sta=%02X\n",currentFlooer,tmpData);
            elevator_info[MASTER_1].FxDoorSta[currentFlooer].bt8_2.bit2 = tmpData;


            if(tmpData)
            {
                APP_USER_setBit(elevator_info[MASTER_1].FxDoorLock,currentFlooer);
            } 
            else
            {
                APP_USER_clearBit(elevator_info[MASTER_1].FxDoorLock,currentFlooer);
            }
                
            
            APP_USER_SendDoorStatusEvnet();
        }
        
        tmpData =  data.bit16_2.bit10;
        if(tmpData != (elevator_info[MASTER_2].FxDoorSta[currentFlooer].bt8_2.bit2))
        {
            //LOG("FDoor Lock No.2 flooID=%d sta=%02X\n",currentFlooer,tmpData);
            elevator_info[MASTER_2].FxDoorSta[currentFlooer].bt8_2.bit2 = tmpData;

            if(tmpData)
            {
                APP_USER_setBit(elevator_info[MASTER_2].FxDoorLock,currentFlooer);
            }
                
            else
            {
                APP_USER_clearBit(elevator_info[MASTER_2].FxDoorLock,currentFlooer);
            }
                
            APP_USER_SendDoorStatusEvnet();
        }
        
    }
}
void APP_USER_Handle10(uint8_t addr,const uint8_t *buffer,uint8_t size)
{

    object_info.times[addr]=0;

}
void APP_RS485_Rec_Handle (uint8_t *buffer,uint16_t len)
{
    {
        switch(buffer[1])
        {
        case 0x03:
            APP_USER_Handle03(buffer[0],buffer+2,buffer[2]);
            break;
        case 0x06:
            APP_USER_Handle06(buffer[0],buffer+2,4);
            break;
        case 0x10:
            APP_USER_Handle10(buffer[0],buffer+2,4);
            break;
        default:
            break;
        }
    }
}
void APP_USER_RS485HANDLE(void)
{
    if(g_app_cfg.appComAllow == 0)
    {
        return;
    }

    if(object_info.enble == 0)
    {
        return;
    }

    if(BSP_UART_Rec_Read(BSP_RS485_UART)==USR_EOK)
    {
        //LOG_HEX(BSP_RS485_UART_BUF.rxBuf,BSP_RS485_UART_BUF.rxLen);
        if (BSP_RS485_UART_BUF.rxLen > 4)
        {
            uint16_t crc = BSP_CRC16(BSP_RS485_UART_BUF.rxBuf,BSP_RS485_UART_BUF.rxLen-2);
            if(BSP_RS485_UART_BUF.rxBuf[BSP_RS485_UART_BUF.rxLen-2]==(uint8_t)(crc>>8) && BSP_RS485_UART_BUF.rxBuf[BSP_RS485_UART_BUF.rxLen-1]==(uint8_t)crc)//crc校验
            {
                APP_RS485_Rec_Handle(BSP_RS485_UART_BUF.rxBuf,BSP_RS485_UART_BUF.rxLen);
                BSP_TIMER_Modify_Start(&g_timer_FxLoop,5);

            }
        }
        //LOG_HEX(BSP_RS485_UART_BUF.rxBuf,BSP_RS485_UART_BUF.rxLen);
    }
}

void APP_USER_SlaveDataHandle(void)
{
    uint8_t tx_num = 0;
    uint8_t  tx_buf[16]= {0};
    int8_t disFloor;


    if (object_info.sendID)
    {
        //SendID 的机器有异常
    }
    
    
    //2001 楼层信息
    disFloor = object_info.xFloor[elevator_info[MASTER_1].currentFloor].cFloor;
    tx_buf[tx_num++] = disFloor;
    disFloor = object_info.xFloor[elevator_info[MASTER_2].currentFloor].cFloor;
    tx_buf[tx_num++] = disFloor;

    //2002 左笼独有
    tx_buf[tx_num++] = elevator_info[MASTER_1].slave_dataComm.u8_buf[1];
    tx_buf[tx_num++] = elevator_info[MASTER_1].slave_dataComm.u8_buf[0];

    //2003 右笼独有
    tx_buf[tx_num++] = elevator_info[MASTER_2].slave_dataComm.u8_buf[1];
    tx_buf[tx_num++] = elevator_info[MASTER_2].slave_dataComm.u8_buf[0];

    //2004 楼层独有
    tx_buf[tx_num++] = object_info.slave_data[object_info.FDLoop_Nnm].u8_buf[1];
    tx_buf[tx_num++] = object_info.slave_data[object_info.FDLoop_Nnm].u8_buf[0];

    //2005 楼层独有,楼层映射
    disFloor = object_info.xFloor[object_info.FDLoop_Nnm].cFloor;
    tx_buf[tx_num++] = (int)0;
    tx_buf[tx_num++] = disFloor;


    object_info.sendID = object_info.FDLoop_Nnm;

    BSP_MODBUS_SendRtu_10(object_info.FDLoop_Nnm,0x2001,tx_buf,tx_num/2);
	//BSP_MODBUS_SendRtu_10(2,0x2001,tx_buf,4);
    //if(object_info.FDLoop_Nnm ==3)
    //LOG_HEX(tx_buf,tx_num);



}
void APP_USER_FxLoop_SetStatus(uint8_t sta)
{
    if(sta)
    {
        if(g_app_cfg.floorXType == FLOOR_X_TYPE_CABLE)
        {
            BSP_TIMER_Modify_Start(&g_timer_FxLoop,100);
        }
        object_info.floorXEnable = 1;
    }
    else
    {
        if(g_app_cfg.floorXType == FLOOR_X_TYPE_CABLE)
        {
            BSP_TIMER_Stop(&g_timer_FxLoop);
        }
        object_info.floorXEnable = 0;
    }
}

void APP_USER_FxLoop(void)
{
    uint8_t com = 0;
    uint16_t loopInterval = 10;
    if (object_info.enble == 0)
    {
        return;
    }
    
    

	//LOG("%d appComAllow =%d\n",(uint32_t)HAL_GetTick(),g_app_cfg.appComAllow);

    com = object_info.com >MAXFLOOR?MAXFLOOR:object_info.com;
    APP_USER_SlaveDataHandle();
    object_info.FDLoop_Nnm++;
    if(object_info.FDLoop_Nnm>com)
    {
        object_info.FDLoop_Nnm = 2;
        //LOGT("####pmj\n");
    }

    loopInterval = 500/com;
    //LOGT("***\n");
    BSP_TIMER_Modify_Start(&g_timer_FxLoop,50);


}

void APP_USER_sendEnergy(void)
{

    if(g_app_cfg.appComAllow == 0)
    {
        return;
    }

     if(object_info.enble == 0)
     {
        return;
     }

    APP_USER_SendEnergy();

}

void APP_USER_FxLockUpdate(void)
{
    uint8_t index,floor;
    uint8_t com = object_info.com >MAXFLOOR?MAXFLOOR:object_info.com;
    for(index=0;index <MASTER_MAX;index++)
    {
        for(floor=2;floor <com+1;floor++)
        {
            if(APP_USER_getBit(elevator_info[index].FxDoorLock,floor))
            {
                if((BSP_TIMER_Ticks_Get()-elevator_info[index].FxDoorLockTick[floor])>20000)
                {
                    APP_USER_UNLOCK(index, floor,0);
                    LOG("index%d floor=%d timeout Lock\n",index,floor);
                }
            }
            else
            {
                //elevator_info[index].FxDoorLockTick[floor] = BSP_TIMER_Ticks_Get();
            }
#if 0
            if(floor ==3)
            {
                
                LOG("index%d floor=%d LOCK %X %X  slave:%d %d\n",index,floor,elevator_info[index].FxDoorLock[1],elevator_info[index].FxDoorLock[0],
                        object_info.slave_data[floor].bit16.bit2,object_info.slave_data[floor].bit16.bit3);
            }
#endif
            
        }
    }
}



#define TIME_TEXT 0

void APP_USER_MUpdate(void)
{

    
    APP_USER_SendDoorStatusEvnet();
    
	
	//LOG("TTTTT %d\n",HAL_GetTick());


    // static uint8_t data_tmp = 50;
    // static uint8_t event_tmp1;
    // data_tmp--;

    //  if (data_tmp <= 30)
    // {
        
    //     object_info.inspection_count = data_tmp;
    //     if(data_tmp == 30)
    //     {
    //         object_info.inspection_event = INSPECTION_TIMEOUT_START;
    //     }
    //     else if(data_tmp == 0)
    //     {
    //             object_info.inspection_event = INSPECTION_START;

    //     }
    //     else if(data_tmp == 12)
    //     {
    //         object_info.inspection_event = INSPECTION_TIMEOUT;
    //     }
    //     else if(data_tmp <= 10)
    //     {
    //         object_info.inspection_event = INSPECTION_TIMEOUT_COUT;
    //     }    
    // }
    // if(data_tmp == 0)
    //     data_tmp = 50;
    //object_info.inspection_event = INSPECTION_START;
    
#if TIME_TEXT
 uint8_t com = 0;
    com = object_info.com >MAXFLOOR?MAXFLOOR:object_info.com;
	APP_ELEVATOR_sched_virtul();
    static uint8_t buff_text[2] = {0};
    user_com_def comm;
    buff_text[0] = 3;
   if (buff_text[1])
   {
       buff_text[1] = 0;
   }
   else
    {
        buff_text[1] = 1;
    }
    comm.did.deviceid = 1;
    //APP_COMM_4004(&comm,buff_text,2);

#if 0
    static uint8_t status_text = 1;
    //elevator_info[0].sampleDevSta.u32 = status_text;
    //elevator_info[0].ctlDevSta.bit32_2.bit0  = status_text;

    //elevator_info[0].ctlDevSta.bit32_2.bit14 = status_text;
    //elevator_info[0].ctlDevSta.bit32_2.bit0  = status_text?0:1;

   //elevator_info[0].ctlDevSta.bit32_2.bit10  = status_text; //出料门
    // elevator_info[0].ctlDevSta.bit32_2.bit12  = status_text; //进料门

    elevator_info[0].ctlDevSta.bit32_2.bit2  = status_text; //进料光幕

    elevator_info[0].ctlDevSta.bit32_2.bit28  = status_text; //出料光幕
    
    LOG("TTTTT %d\n",status_text);

     status_text =status_text?0:1;
    
    // APP_DWIN_TX_String_Move(0x1520,0,0);
    // if(status_text)
    //     APP_DWIN_TX_String_Move(0x1520,"楼层运行顺序若出",16);
    // else
    //     APP_DWIN_TX_String_Move(0x15C0,"迪文科技",8);
    // LOG("TTTTT %d\n",status_text);


    // APP_SOUND_User_play("锟斤拷茫锟?");
    // APP_SOUND_User_play("锟皆讹拷模式锟斤拷");
#endif
//user_com_def comm;
// static uint8_t buff_text[7] = {0};
// buff_text[0] = 3;
// buff_text[1] = 0;
// buff_text[2] = 1;
// buff_text[3] = 3;

// buff_text[4] = 0;
// buff_text[5] = 1;
// buff_text[6] = 2;

// comm.did.deviceid = 1;
//     APP_COMM_4005(&comm,buff_text,7);


#endif

}

void APP_USER_info_Init(void)
{
    int8_t i;
    LOG("devMode %d\n",g_app_cfg.devMode);
    APP_DWIN_returnMainPage();

    LOG("mainPage %d\n",object_info.mainPage);

    g_app_cfg.logoType = 1;  //上电初始无LOGO
    elevator_info[MASTER_1].index =MASTER_1;
    elevator_info[MASTER_2].index =MASTER_2;

    elevator_info[MASTER_1].did.deviceid =g_app_cfg.MDidL.deviceid;
    elevator_info[MASTER_2].did.deviceid =g_app_cfg.MDidR.deviceid;

    elevator_info[MASTER_1].currentFloor =1;
    elevator_info[MASTER_2].currentFloor= 1;

    elevator_info[MASTER_1].currentFloorSta = 0; 
    elevator_info[MASTER_2].currentFloorSta = 0; 
    object_info.com = 255; 
    elevator_info[MASTER_1].ctlMode = 255;
    elevator_info[MASTER_2].ctlMode = 255;

    APP_USER_UpdateComm_sta(MASTER_1,STA_COMM_NG);
    APP_USER_UpdateComm_sta(MASTER_2,STA_COMM_NG);

    elevator_info[MASTER_1].inspection_enable = APP_MEMORY_Read_U16(CFG_ADDR_INSP_EN_0)?1:0;
    elevator_info[MASTER_2].inspection_enable = APP_MEMORY_Read_U16(CFG_ADDR_INSP_EN_1)?1:0;

    if(elevator_info[MASTER_1].inspection_enable || elevator_info[MASTER_2].inspection_enable)
    {
        object_info.inspection_enable=1;
    }
    if(elevator_info[MASTER_1].inspection_enable==0 && elevator_info[MASTER_2].inspection_enable==0)
    {
        object_info.inspection_enable=0;
    }
    LOG("Inspection_enable %d %d\n",elevator_info[MASTER_1].inspection_enable,elevator_info[MASTER_2].inspection_enable);

    object_info.FloorBoardLoseCount[1] = 0;
    for(i=2;i<MAXFLOOR+1;i++)
    {
        object_info.FloorBoardLoseCount[i] = FLOOR_BOARD_TIMEOUT;
    }
    // for(i=0; i<MAXFLOOR+1 ;i++)
    // {
    //     object_info.xFloor[i].cFloor = i;
    // }

    object_info.xFloorMapChang = 0;
}

//通信状态更新
void APP_USER_UpdateComm_sta(uint8_t index,uint8_t sta)
{
    uint8_t i;
    uint8_t indexOther;

    elevator_info[index].MCommSta = sta;
    if(elevator_info[index].MCommSta == STA_COMM_NG)
    {
        elevator_info[index].firstSupport = 1;
        elevator_info[index].ctlMode = 0;
        elevator_info[index].slave_dataComm.u16=0; 
        elevator_info[index].support_Num= 0;
        
        elevator_info[index].slave_dataComm.bit16_2.bit14 =1;
        elevator_info[index].sampleDevSta.u32 = 0;
        elevator_info[index].ctlDevSta.u32 = 0;
        elevator_info[index].floorHigh.u32 = 0;

        elevator_info[index].statusDispEn = 0;
        elevator_info[index].statusVoiceEn = 0;

        APP_COMM_clearFlooXList(index);
        BSP_GPIO_DataInit();
        LOG("No%d comm ERR %d\n",index,elevator_info[index].firstSupport);
    }
    else
    {
        elevator_info[index].slave_dataComm.bit16_2.bit14 =0;

        elevator_info[index].statusDispEn = 1;
        elevator_info[index].statusVoiceEn = 1;
        
    }
}

void APP_USER_MStatusNo1_Handle(void)
{
    //APP_COMM_ERR_STA_L(1);
    
    APP_USER_UpdateComm_sta(MASTER_1,STA_COMM_NG);
}

void APP_USER_MStatusNo2_Handle(void)
{
    APP_USER_UpdateComm_sta(MASTER_2,STA_COMM_NG);
    
    //APP_COMM_ERR_STA_R(1);
}

uint8_t APP_USER_findMapFloor2Num(int8_t floor)
{

    uint8_t i;
    for(i=1;i<object_info.xFloorMapNum+1;i++)
    {
        if(object_info.xFloor[i].cFloor == floor)
        {
            return i;
        }
    }
    return 0;
    
}


void APP_USER_saveMapFloor(void)
{
    //三分钟没有更新列表 存储
    uint8_t mapFloorList[MAPFLOOR_SAVE_NUM];
    uint8_t i;
    LOG("saveMapFloor  ");
    mapFloorList[0] = object_info.xFloorMapNum;
    LOG("len:%d\n  ",mapFloorList[0]);
    for(i=1;i<object_info.xFloorMapNum+1;i++)
    {
        mapFloorList[i] = object_info.xFloor[i].cFloor;
        LOG(" %d:%d",i,object_info.xFloor[i].cFloor);

         if(i%10 == 0)
        {
            LOG("\n  ");
        }
    }
    LOG("\n");
    //第一个字节存长度，第二个字节以后存别名
    APP_CONFIG_mapFLoorX_Set(mapFloorList,object_info.xFloorMapNum+1);
    
}

void APP_USER_resetMapFloor(void)
{
    //三分钟没有更新列表 存储
    uint8_t i;
    LOG("resetMapFloor\n");
    object_info.xFloorMapNum = MAXFLOOR;
    for(i=1;i<object_info.xFloorMapNum+1;i++)
    {
        object_info.xFloor[i].cFloor = i;
    }
    APP_USER_saveMapFloor();
}

void APP_USER_getMapFloor(void)
{
    //三分钟没有更新列表 存储
    uint8_t mapFloorList[MAPFLOOR_SAVE_NUM];
    uint8_t num = 0;
    uint8_t needReset = 0;
    uint8_t i;

    APP_CONFIG_mapFLoorX_Get(mapFloorList,MAPFLOOR_SAVE_NUM);

    // for(i=0;i<MAPFLOOR_SAVE_NUM;i++)
    // {
    //     LOG(" %d:%d",i,mapFloorList[i]);
    //     if(i%10 == 0)
    //     {
    //         LOG("\n  ");
    //     }
    // }
    // LOG("\n  ");
    if(mapFloorList[0] > MAPFLOOR_SAVE_NUM)
    {
        LOG("getMapFloor %X\n  ",mapFloorList[0]);
        APP_USER_resetMapFloor();
        return;
    }
    LOG("getMapFloor\n  ");
    object_info.xFloorMapNum = mapFloorList[0];
    for(i=1;i<object_info.xFloorMapNum+1;i++)
    {
        object_info.xFloor[i].cFloor = mapFloorList[i];
        LOG(" %d:%d",i,object_info.xFloor[i].cFloor);
        if(object_info.xFloor[i].cFloor ==0 
        || object_info.xFloor[i].cFloor <-10 
        || object_info.xFloor[i].cFloor >99)
        {
            needReset = 1;
        }

        if(i%10 == 0)
        {
            LOG("\n  ");
        }
    }
    LOG("\n");
    if(needReset)
    {
        APP_USER_resetMapFloor();
    }
}

void APP_USER_updateMapFloor(void)
{
    //三分钟没有更新列表 存储
    uint8_t mapFloorList[MAPFLOOR_SAVE_NUM];
    uint8_t num = 0;
    uint8_t i;
    if (object_info.xFloorMapChang && BSP_TIMER_Ticks_Get()-object_info.xFloorMapChangTick > 3*60000)
    {
        object_info.xFloorMapChang = 0;

        LOG("updateMapFloor\n");
        
       APP_USER_saveMapFloor();
    }
}

void APP_USER_FloorBoardLose(void)
{
    
    uint8_t i,com;
    com = object_info.com >MAXFLOOR? MAXFLOOR:object_info.com;
    for(i=2;i<(com+1);i++)
    {
        
        if(object_info.FloorBoardLoseCount[i] <FLOOR_BOARD_TIMEOUT)
        {
            object_info.FloorBoardLoseCount[i]++;
        }
    }
}

void APP_USER_TIME_Handle(void)
{
    
    APP_USER_FloorBoardLose();
    APP_USER_FxLockUpdate(); //更新层门锁状态
    BSP_UART_check();
    //APP_USER_updateMapFloor();
}

void APP_USER_startCheck_Handle(void)
{
    //APP_DWIN_SUB_Set_Page(0);
    //LOG("XXXXXXX\n");
    APP_DWIN_SUB_Switch_Page(0);
}


void APP_USER_Init(void)
{

//    BSP_TIMER_Init(&g_timer_realtimedata,APP_USER_Send_RealTimeData,500,500);
//    BSP_TIMER_Start(&g_timer_realtimedata);
    APP_USER_info_Init();
//elevator_info[0].currentFloor= 3;
//elevator_info[1].currentFloor= 3;
    BSP_TIMER_Init(&g_timer_FxLoop,APP_USER_FxLoop,10,0);
    BSP_TIMER_Start(&g_timer_FxLoop);

    BSP_TIMER_Init(&g_timer_sendEnergy,APP_USER_sendEnergy,1000,1000);
    BSP_TIMER_Start(&g_timer_sendEnergy);

    BSP_TIMER_Init(&elevator_info[MASTER_1].timer_MStatus,
                        APP_USER_MStatusNo1_Handle,3000,0);
    BSP_TIMER_Init(&elevator_info[MASTER_2].timer_MStatus,
                        APP_USER_MStatusNo2_Handle,3000,0);
    BSP_TIMER_Init(&g_timer_FloorBoardLose,APP_USER_TIME_Handle,1500,1500);
    BSP_TIMER_Start(&elevator_info[MASTER_1].timer_MStatus);
    BSP_TIMER_Start(&elevator_info[MASTER_2].timer_MStatus);
    BSP_TIMER_Start(&g_timer_FloorBoardLose);
#if TIME_TEXT
    BSP_TIMER_Init(&g_timer_MUpdate,APP_USER_MUpdate,15000,15000);
#else
    BSP_TIMER_Init(&g_timer_MUpdate,APP_USER_MUpdate,200,200);
#endif

    BSP_TIMER_Start(&g_timer_MUpdate);


    BSP_TIMER_Init(&g_timer_StartCheck,APP_USER_startCheck_Handle,30000,0);
    //BSP_TIMER_Modify_Start(&g_timer_StartCheck, 3000);

    BSP_TIMER_Init(&g_timer_Inspection,APP_USER_inspectionReady,1000,0);
    BSP_TIMER_Init(&g_timer_InspectionHeart,APP_USER_inspectionHeart,120000,0);
    //BSP_TIMER_Start(&g_timer_Inspection);

    BSP_GPIO_Set(0xB1,0);
    BSP_CONFIG_Init();
    to_cb_init();


    BSP_GPIO_JDQ_CTL(0,0);
    BSP_GPIO_JDQ_CTL(1,1);
    BSP_GPIO_JDQ_CTL(3,1);
    APP_DWIN_Init();

    BSP_RN8209D_Init();
    APP_SOUND_Init();
    APP_ENERGY_init();

    APP_COMM_4009_SetAppLicence(MASTER_1, g_app_cfg.appLicence[MASTER_1], 1);
    APP_COMM_4009_SetAppLicence(MASTER_2, g_app_cfg.appLicence[MASTER_2], 1);
#if 0
    APP_ELEVATOR_sched_virtul();
#endif
}
/*****END OF FILE****/
