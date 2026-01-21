#include "app_config.h"
#include "app_user.h"
#include "app_user_utils.h"
#include "app_user_globals.h"
#include "app_user_packet.h"

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

