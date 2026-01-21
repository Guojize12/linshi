#include "app_config.h"
#include "app_user.h"
#include "app_user_globals.h"
#include "app_user_packet.h"
#include "app_user_resend.h"
#include "app_user_event.h"
#include "app_user_status.h"
#include "app_user_inspect.h"
#include "app_user_proto.h"
#include "app_user_utils.h"
#include "app_user_floor.h"
#include "app_user_rx.h"

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

   //elevator_info[0].ctlDevSta.bit32_2.bit10  = status_text; //������
    // elevator_info[0].ctlDevSta.bit32_2.bit12  = status_text; //������

    elevator_info[0].ctlDevSta.bit32_2.bit2  = status_text; //���Ϲ�Ļ

    elevator_info[0].ctlDevSta.bit32_2.bit28  = status_text; //���Ϲ�Ļ
    
    LOG("TTTTT %d\n",status_text);

     status_text =status_text?0:1;
    
    // APP_DWIN_TX_String_Move(0x1520,0,0);
    // if(status_text)
    //     APP_DWIN_TX_String_Move(0x1520,"¥������˳������",16);
    // else
    //     APP_DWIN_TX_String_Move(0x15C0,"���ĿƼ�",8);
    // LOG("TTTTT %d\n",status_text);


    // APP_SOUND_User_play("��ã�?");
    // APP_SOUND_User_play("�Զ�ģʽ��");
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

    g_app_cfg.logoType = 1;  //�ϵ��ʼ��LOGO
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

//ͨ��״̬����

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


void APP_USER_TIME_Handle(void)
{
    
    APP_USER_FloorBoardLose();
    APP_USER_FxLockUpdate(); //���²�����״̬
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
