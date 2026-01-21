#ifndef __APP_USER_MAIN_H
#define __APP_USER_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"
#include "app_user_globals.h"

void APP_USER_info_Init(void);
void APP_USER_Init(void);
void APP_USER_RS485HANDLE(void);
void APP_RS485_Rec_Handle (uint8_t *buffer,uint16_t len);
void APP_USER_Handle03(uint8_t addr,const uint8_t *buffer,uint8_t size);
void APP_USER_Handle06(uint8_t addr,const uint8_t *buffer,uint8_t size);
void APP_USER_Handle10(uint8_t addr,const uint8_t *buffer,uint8_t size);
void APP_COMM_Rx_Analysis(uint8_t *buff,uint16_t size);
void APP_WIRELESS_Rx_Analysis(uint8_t *buff,uint16_t size);
void APP_USER_Rx_Handle(void);
void APP_WIRELESS_Rx_Handle(void);
void APP_USER_SlaveDataHandle(void);
void APP_USER_FxLoop_SetStatus(uint8_t sta);
void APP_USER_FxLoop(void);
void APP_USER_sendEnergy(void);
void APP_USER_FxLockUpdate(void);
void APP_USER_MUpdate(void);
void APP_USER_MStatusNo1_Handle(void);
void APP_USER_MStatusNo2_Handle(void);
void APP_USER_FloorBoardLose(void);
void APP_USER_TIME_Handle(void);
void APP_USER_startCheck_Handle(void);

#ifdef __cplusplus
}
#endif
#endif  /* __APP_USER_MAIN_H */
