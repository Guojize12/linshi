#ifndef __APP_USER_EVENT_H
#define __APP_USER_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "app_user.h"

// Scheduling structures
typedef struct
{
    uint8_t commOk[MASTER_MAX];   //ͨ��״̬
    uint8_t AutoModeSta[MASTER_MAX];  //�Զ�״̬
    uint8_t NofullSta[MASTER_MAX];  //δ��Ա״̬
    uint8_t stopErrSta[MASTER_MAX];  //�쳣ͣ��
    uint8_t smartSta[MASTER_MAX];  //����״̬  
    uint8_t sched_No;           //����������
} st_sched_info;

enum
{
    SCHEDSTA_NG=0, 
    SCHEDSTA_OK,  
    SCHEDSTA_XX //����
};

extern st_sched_info stSchedList[];

// Function declarations
uint8_t APP_USER_SupplyListHave(uint8_t floor,uint8_t direction);
void APP_USER_SendKeyEvnet(uint8_t floor,uint8_t direction);
void APP_USER_SendCancelKeyEvnet(uint8_t floor,uint8_t direction);
void APP_COMM_KeyLedOn(uint8_t index, uint8_t flootID, uint8_t direction);
void APP_COMM_KeyLedOff(uint8_t index, uint8_t flootID, uint8_t direction);
void APP_COMM_CanalKey(uint8_t index, uint8_t flootID, uint8_t direction);
void APP_COMM_F1Key(uint8_t index, uint8_t sta);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USER_EVENT_H */
