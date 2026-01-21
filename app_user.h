#ifndef __APP_USER_H
#define __APP_USER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"
#define MAXFLOOR 50

#define SCH_NO1 0
#define SCH_NO2 1
#define FLOOR_BOARD_TIMEOUT          5


#define S_FLOOR_ACK_NUM 5 //应答楼层总数
#define S_FLOOR_ACK_TIME 3 //应答楼层总数
#define CODE_OPTIMIZE 0
#define USE_4007_SRC 1

enum
{
 CTL_MODE_AUTO=0,
 CTL_MODE_MANUL,
 CTL_MODE_CHECK,
 CTL_MODE_TRANSP,
 CTL_MODE_MAX,
};

enum
{
    MASTER_1=0, //左笼
    MASTER_2,    //右笼
    MASTER_MAX,

    MASTER_ALL = 252,  //都发送
    MASTER_SCHED = 253,  //调度
    MASTER_RAND = 254,  //随机
    MASTER_NULL = 255,  //都不发
};

enum
{
    DEV_MODE_ELEVATOR_ROBOT=0, //智能升降机
    DEV_MODE_ELEVATOR_LIFT_DOUBLE,    //智能物料提升机-双笼
    DEV_MODE_ELEVATOR_LIFT_SINGLE,    //智能物料提升机-单笼
    DEV_MODE_ELEVATOR_MAX,
};


enum
{
    STA_COMM_OK=0, //通信正常
    STA_COMM_NG,    //通信异常
    STA_COMM_MAX,
};




enum
{
    SETPER_POWER_YJ=1,        //之后是四个字节
    SETPER_POWER_BJ,
	SETPER_LEAK_CURRENT_YJ,
	SETPER_LEAK_CURRENT_BJ,
	SETPER_ENVIR_TEMPER_YJ,  //5
	SETPER_ENVIR_TEMPER_BJ,
	SETPER_CABLE_TEMPER_YJ,
	SETPER_CABLE_TEMPER_BJ,
	SETPER_CURRENT_K_A,
	SETPER_CURRENT_K_B,  //10
	SETPER_CURRENT_K_C,
	SETPER_LEAKAGE_CURRENT_K,
    SETPER_CURRENT_ZERO_A, 
    SETPER_CURRENT_ZERO_B, 
    SETPER_CURRENT_ZERO_C,   //15
	SETPER_LEAKAGE_CURRENT_ZERO,
	SETPER_YG_ZERO,
	SETPER_ENERGY_K,
	SETPER_CURRENT_RATIO_A,     //之后是两个字节
	SETPER_CURRENT_RATIO_B,  //20
	SETPER_CURRENT_RATIO_C,
	SETPER_VOLTAGE_RATIO_A,
	SETPER_VOLTAGE_RATIO_B,
	SETPER_VOLTAGE_RATIO_C,
	SETPER_LEAKAGE_CURRENT_RATIO,  //25
    SETPER_ENERGY_SET,  //26
    SETPER_CURRENT_THRESHOLD,  //27 
    SETPER_MAX  
};

enum
{
    APPLICENCE_NORMAL=0,  
    APPLICENCE_STOP,
    APPLICENCE_STOP_SPECIAL,
    APPLICENCE_TO_NORMAL,
    APPLICENCE_MAX,

};

enum
{
    INSPECTION_NULL=0,
    INSPECTION_TIMEOUT_START,
    INSPECTION_TIMEOUT,
    INSPECTION_START, 
    INSPECTION_STOP,
    INSPECTION_TIMEOUT_COUT,
};

typedef struct
{
    uint8_t floor:6;
    uint8_t gageExt:1 ; //笼外
    uint8_t dir:1 ; //接应方向
} supply_bit8_def;

typedef  union
{
    uint8_t u8;
    supply_bit8_def supply_bit;
} supply_def; 


typedef struct
{
    uint8_t upper:1 ; //上接应
    uint8_t lower:1 ; //下接应
} supplyFx_bit8_def;

typedef  union
{
    uint8_t u8;
    supplyFx_bit8_def bit;
} supplyFx_def; 


typedef struct
{
    uint8_t enable:1;  //楼层使能
    uint8_t support_Sta:1;    //接应状态
    uint8_t support_upper:1;  //上接应
    uint8_t support_lower:1;  //下接应
    uint8_t unlock:1;  //解锁状态
    uint8_t keyLED:1;  //按键灯状态
} st_floorLR_info; 

typedef struct
{
    int8_t cFloor;  //楼层别名
    st_floorLR_info floorLR[2]; //区别左右笼
} st_floor_info; 


typedef struct
{
    uint8_t index;
    bsp_id_def  did; //笼设备DID
    uint8_t currentFloor;
    uint8_t currentFloorSta;
    uint8_t errStop; //异常停靠

    supplyFx_def supportFx[MAXFLOOR+1]; //按楼层的接应信息

    uint8_t support_data[MAXFLOOR+1]; //接应列表 原始
    uint8_t support_Disp[6]; //接应楼层显示
    uint8_t support_Num; //接应楼层数量
    bsp_un8_def FxDoorSta[MAXFLOOR+1]; //层门状态

    uint32_t FxDoorBit[2]; //层门状态
    uint32_t FxDoorLock[2]; //层门锁状态

    uint64_t FxDoorLockTick[MAXFLOOR+1]; //

    bsp_un8_def F1Sta; //地龙状态



    uint8_t smart_mode; //吊笼智能模式:0,不支持,1支持双笼

    IEEE754 floorHigh; //电梯高度M
    IEEE754 temperSenser; //温度传感器 
    uint8_t MCommSta; //与采集板的通信状态
    uint8_t ctlMode;  //控制模式
    bsp_un16_def slave_dataComm; //发送给层级数据的公用部
    uint8_t FloorACK; //设置应对的楼层
    Timer timer_MStatus;

    uint8_t startCheckNum; //开机自检项目数
    uint8_t startCheckIndex[64]; //开机自检项目index
    uint8_t startCheckSta[64]; //开机自检项目状态

    uint8_t box_arrive; //笼到位
    uint8_t F1_door; //底栏门
    uint8_t F1_doorCheck; //底栏门插入

    uint8_t F1_Fangjia; //防夹
    uint8_t F1_FangjiaCheck; //防夹插入

    uint32_t SampleBoxRevCount; //接受采集板计数
    uint32_t powerEle; //动力电


    uint8_t displayRED_Num; //显示状态红
    uint8_t displayGRE_Num; //显示状态绿
    uint8_t displayBLU_Num; //显示状态蓝

    IEEE754 sampleDevSta; //采集主机传回的状态
    IEEE754 ctlDevSta; //控制传回的状态
    uint16_t  logoType; //LOGO

    bsp_un8_def FloorLED[MAXFLOOR+1];  //bit0=上  bit1=下  超时清除层面按键灯用
    uint8_t FloorLock_F; //层门锁控制的楼层

    uint32_t weightLoad:1; //重量满载
    uint32_t peopleLoad:1; //人员满载
    uint32_t fullLoad:1; //满载
    uint32_t statusDispEn:1; //状态显示使能
    uint32_t statusVoiceEn:1; //状态播报使能

    uint32_t firstSupport:1; //

    uint32_t inspection_enable:1; 
    uint32_t inspection_allow:1; 
    uint32_t inspection_status:1; 
    uint32_t waitCallDisp:2; 
    int8_t callFloorX; 


} st_elevator_info;



typedef struct
{

    uint8_t com;  //总楼层
    uint8_t sendID; //右数据变化需要更新数据

    st_floor_info xFloor[MAXFLOOR+1];
    uint8_t xFloorMapNum;
    uint64_t xFloorMapChangTick;
    uint8_t xFloorMapChang;
	bsp_un8_def keyUpper[MAXFLOOR+1]; //实际的层机按键状态 bit0 短按 bit1 长按
    bsp_un8_def keyLower[MAXFLOOR+1];
    uint8_t FloorBoardLoseCount[MAXFLOOR+1]; //层机通信丢失计数
    bsp_un16_def slave_data[MAXFLOOR+1];
    uint16_t times[MAXFLOOR];

    uint8_t FDLoop_Nnm;  //向层机寻址
    uint8_t MUpdateNum;   //向采集主机更新状态 寻址  

    uint8_t startCheckNum; //开机自检项目数
    uint8_t startCheckIndex[64]; //开机自检项目index
    uint8_t startCheckSta[64]; //开机自检项目状态

    IEEE754 currentEle[3]; //三向电流
    IEEE754 currentPeak; //漏电流
    IEEE754 temperCable; //线缆温度
    IEEE754 temperEnvir; //环境温度 


    uint32_t displayRED; //显示状态红
    uint32_t displayORG; //显示状态绿
    uint32_t displayBLU; //显示状态蓝

    uint32_t sched_time; //被调度时间
    uint8_t sched_No1:1; //1笼被调度
    uint8_t sched_No2:1; //2笼被调度

    uint8_t keyFloor; 
    uint8_t keyFloorDir:2; 
    uint64_t keyFloorTick; 
    uint8_t keyFilter:1; 
    uint8_t checkWindowIndex; 
    uint8_t checkWindowWarring; 
    uint64_t checkWindowWarringTick; 

    uint8_t enble; 

    uint8_t inspection_count; 
    uint8_t inspection_status; 
    uint8_t inspection_event; 
    uint8_t inspection_enable; 
    uint64_t inspection_timeout;
    uint8_t inspection_keyType; 
    uint8_t inspection_allow:1; 
    uint8_t inspection_atPage:1; 
    uint8_t dwinAvailable:1; 
    uint8_t waitCallDisp:2; 
    uint8_t isSetDisp:2; 
    uint8_t mainPage; 
    uint8_t floorXEnable:1; 
    

} st_object_info;

extern st_object_info object_info;
extern st_elevator_info elevator_info[2];
void APP_USER_Init(void);
void APP_USER_Rx_Handle(void);
void APP_WIRELESS_Rx_Handle(void);
void APP_USER_packet(uint16_t cmd,st_elevator_info *elev_info,uint8_t *buf,uint8_t len,uint8_t type);//type 0 不等应答  1  等应答
void BSP_USER_Key_Handle(void);
void APP_USER_RS485HANDLE(void);
void APP_USER_SendKeyEvnet(uint8_t floor,uint8_t direction);
void APP_USER_SendCancelKeyEvnet(uint8_t floor,uint8_t direction);
void APP_USER_SendDoorStatusEvnet(void);
void APP_USER_ToSample_SetModeEvnet(st_elevator_info *elev_info);
void APP_COMM_5008_setParameter(uint8_t number);
void APP_COMM_4007_setParam(uint8_t number);
void APP_USER_UNLOCK(uint8_t index, uint8_t floor, uint8_t sta);
void APP_USER_SetCtlMode(uint8_t index,uint8_t ctlMode);
void APP_USER_recoverErr(uint8_t index);
void APP_USER_SendCheckOption(uint8_t index,uint16_t opt, uint8_t event);
//模拟笼内呼叫
void APP_USER_SendSimulationInCall(uint8_t index,uint8_t floor);
void APP_COMM_AppEnble(uint8_t sta); //

void APP_USER_saveMapFloor(void);
void APP_USER_getMapFloor(void);
void APP_USER_resetMapFloor(void);
uint8_t APP_USER_findMapFloor2Num(int8_t floor);
void APP_USER_FxLoop_SetStatus(uint8_t sta);
void APP_USER_inspectionStop(uint8_t index);
void APP_USER_inspectionTrigger(void);
void APP_USER_inspectionForceStop(void);
void APP_COMM_4009_SetInspectionAllowUpdate(uint8_t index);
#ifdef __cplusplus
}
#endif
#endif  /* __APP_USER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
