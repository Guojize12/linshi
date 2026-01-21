#include "app_config.h"
#include "app_user.h"
#include "app_user_inspect.h"
#include "app_user_globals.h"
#include "app_user_packet.h"
#include "app_user_event.h"

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

