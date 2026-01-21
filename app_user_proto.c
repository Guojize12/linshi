#include "app_config.h"
#include "app_user_proto.h"
#include "app_user_resend.h"
#include "app_user_packet.h"
#include "app_user_utils.h"
#include "app_user_inspect.h"

#define ENERGY_ALL_LEN ((SETPER_CURRENT_RATIO_A-1)*4+(SETPER_MAX-SETPER_CURRENT_RATIO_A)*2)
static uint8_t energy_all_buf[ENERGY_ALL_LEN]= {0};

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
                BSP_TIMER_Stop(&user_send.g_timer_cb[i]);//ֹͣʱٷ
                // LOG("%d cmd=%d [12]=%d,[13]=%d, %d\n",i,comm->cmd,user_send.g_send_buf[i][12],user_send.g_send_buf[i][13],buff[2]);
            }
            else if(user_send.g_send_buf[i][10]==buff[0])
            {
                BSP_TIMER_Stop(&user_send.g_timer_cb[i]);//ֹͣʱٷ
            }
        }
		//		LOG("comm->cmd %d  \n", comm->cmd);
        
        switch (comm->cmd)
        {
        case 5000://Ӧ𰴼¼-
            // flootID = buff[1];
            // direction = buff[2];

            flootID = buff[0];
            direction = buff[1];
			LOG("5000 ack flooID=%d dir=%d bao=%d\n",flootID,direction,buff[2]);

            break;
        case 5001://ӻȡ-
            flootID = buff[0];
            direction = buff[1];   
            LOG("5001 ack flooID=%d dir=%d bao=%d\n",flootID,direction,buff[2]); 
            //APP_COMM_CanalKey(index, flootID, direction);
            break;
        case 5002://ӻͲ״̬-
            break;

        case 5003://ӻȡ-
            break;
        case 5004://ӻͲ״̬-
             break;

        case 5009://ӻͲ״̬-
             LOG("5009 ack index%d\n",index); 
             break;
        case 5013://ѲӦ
             LOG("5013 ack index%d\n",index); 
             break;
        default:
            break;
        }
    }

}

//ȡ
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
            elevator_info[i].currentFloor=buff[0]; //ʵʱ¥

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


            elevator_info[i].logoType = buff[7];  //̱ʾ

            //˫
            if (elevator_info[MASTER_1].MCommSta ==STA_COMM_OK && elevator_info[MASTER_2].MCommSta ==STA_COMM_OK )
            {
                //˫logһ£ʾLOGO
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
                tmpBit16.u16 = buff[8];  //״̬
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


     //ʾӦ
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

    com=buff[0];  //ʵʱ¥

    if(com >MAXFLOOR)
    {  
        LOG("SetMode err c=%d\n",com);
        return;
    }
    object_info.com = com;
   
    LOG("SetMode did=%d com=%d\n",comm->did.deviceid,com);
   
}

void APP_COMM_4004(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    uint8_t floor;
    uint8_t status;
    uint8_t index;
    index = APP_USER_did2Index(comm->did.deviceid);
    if (index>=2)
        return;

    floor=buff[0]; //ʵʱ¥
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

    //˫
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
    //1
    else if(elevator_info[MASTER_1].MCommSta==STA_COMM_OK)
    {
        if(elevator_info[MASTER_1].startCheckNum 
            && (!object_info.startCheckNum))
        {
            object_info.startCheckNum = elevator_info[MASTER_1].startCheckNum;
        }
    }
    //2
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
            //˫
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
                //ۺϽṹ=
                if(0 ==object_info.startCheckSta[i])
                {
                    //ֻҪһ쳣 ʾ쳣
                    if(elevator_info[MASTER_1].startCheckSta[i]==2 
                        || elevator_info[MASTER_2].startCheckSta[i]==2)
                    {
                        object_info.startCheckSta[i] = 2;
                    }
                    //ʾ
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
            //1
            else
            {
                uint8_t index;
                if(elevator_info[MASTER_1].MCommSta == STA_COMM_OK)
                    index = MASTER_1;
                else
                    index = MASTER_2;
                object_info.startCheckIndex[i] = elevator_info[index].startCheckIndex[i];
                //ۺϽṹ=
                if(0 ==object_info.startCheckSta[i])
                {
                    //ֻҪһ쳣 ʾ쳣
                    if(elevator_info[index].startCheckSta[i]==2)
                    {
                        object_info.startCheckSta[i] = 2;
                    }
                    //ʾ
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

        //н쳣ûмеģȴ3s
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
    buf_send[numb++]=g_app_cfg.dtp;  //
    numb++;
    buf_send[numb++]=comm->did.id_buf[2];  //ID
    buf_send[numb++]=comm->did.id_buf[1];  //ID
    buf_send[numb++]=comm->did.id_buf[0];  //ID
    buf_send[numb++]=0;  //ID
    buf_send[numb++]=comm->cmd>>8;//
    buf_send[numb++]=comm->cmd&0xff;//

    ///////////////////////////
    //
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
    case SETPER_ENERGY_SET: //
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
    case SETPER_CURRENT_THRESHOLD: //ֵ
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
        APP_USER_packet(5008,&elevator_info[MASTER_1],buf_send,buff_len,1);//type 0 Ӧ  1  Ӧ
    if (elevator_info[MASTER_2].MCommSta==STA_COMM_OK )
        APP_USER_packet(5008,&elevator_info[MASTER_2],buf_send,buff_len,1);//type 0 Ӧ  1  Ӧ

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
        if(object_info.inspection_atPage) //Ƽ棬ֹͣѲ
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

void APP_COMM_4013(user_com_def *comm,uint8_t *buff,uint8_t len)
{
    //LOG_HEX(buff, len);
    uint8_t buf_send[10]= {0};
    char voice_buf[10] = {0};
    int numb =0;

    uint8_t number;
    uint8_t ret = 0; //Ĭϳɹ
    uint8_t data_tmp;
    number = buff[0];
    data_tmp = buff[1];
    uint8_t index = APP_USER_did2Index(comm->did.deviceid);

    switch (number)
    {
    case SETPER_4013_INSPERSTION_CTL:
        if(data_tmp == 1) //Ѳֹͣ
        {
            LOG("Index:%d Stop inspection\n",index);
            APP_USER_inspectionStop(index);
        }
        else if(data_tmp == 2) //Ѳ쳣ֹͣ
        {
            LOG("Index:%d Abort inspection\n",index);
            APP_USER_inspectionForceStop();
        }
        else if(data_tmp == 0) //Ѳ쿪ʼ
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
             ret = 1;  //زѲ
        }
        break;
    case SETPER_4013_INSPERSTION_HEAT: //Ѳ
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
        case 4000://ɼ ȡ¼
            if(buff[0]>MAXFLOOR)
                return;
            APP_COMM_4000(comm,buff,len);
            APP_USER_Reply_4000(comm,buff[1],buff[0]);
            break;
        case 4001://Ͳʵʱݵӻ
            if(buff[0]>MAXFLOOR)
                return;
            APP_COMM_4001(comm,buff,len); //ʵʱ Ӧ
            //APP_USER_Reply(comm,buff[len-2],buff[0]);
            break;
        case 4002:
            APP_COMM_4002(comm,buff,len); //ʵʱ Ӧ
            //APP_USER_Reply(comm,buff[len-2],buff[0]);
            break;
        case 4003://¥ģʽ
            APP_COMM_4003(comm,buff,len);
            APP_USER_Reply(comm,buff[len-2]);
            break;
        case 4004://
            APP_COMM_4004(comm,buff,len);
            APP_USER_Reply(comm,buff[len-2]);
            break;
        case 4005://Լб
            APP_COMM_4005(comm,buff,len);
            APP_USER_Reply(comm,buff[len-2]);
            break;
        case 4006://ʵʱ
            APP_COMM_4006(comm,buff,len);
            break;
        case 4007://ȡ
            APP_COMM_4007(comm,buff,len,buff[len-2]);
            break;
        case 4008://ȡ
            APP_COMM_4008(comm,buff,len);
            break;
        case 4009://ɼ->
            APP_COMM_4009(comm,buff,len);
            APP_USER_Reply(comm,buff[len-2]);
            break;
        case 4010://ɼ->ȡ
            APP_COMM_4010(comm,buff,len);
            break;
        case 4012://ɼ->ӳ
            APP_COMM_4012(comm,buff,len);
			APP_USER_Reply(comm,buff[len-2]);
            break;
        case 4013://ɼ->Ѳ
            APP_COMM_4013(comm,buff,len);
            break;
        default:
            break;
        }
    }
}
