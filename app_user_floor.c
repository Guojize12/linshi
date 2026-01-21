#include "app_config.h"
#include "app_user.h"
#include "app_user_floor.h"
#include "app_user_globals.h"
#include "app_user_packet.h"
#include "app_user_utils.h"
#include "app_user_event.h"

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

