#ifndef __APP_USER_INSPECT_H
#define __APP_USER_INSPECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"
#include "app_user_globals.h"

void APP_USER_SendCheckOption(uint8_t index,uint16_t opt, uint8_t event);
void APP_USER_SendSimulationInCall(uint8_t index,uint8_t floor);
void APP_USER_inspectionStart(uint8_t index);
void APP_USER_inspectionReady(void);
void APP_USER_inspectionHeart(void);
void APP_USER_inspectionTrigger(void);
void APP_USER_inspectionForceStop(void);
void APP_USER_inspectionStop(uint8_t index);
void APP_COMM_4009_SetInspectionAllowUpdate(uint8_t index);
void APP_COMM_4009_SetInspectionEnable(uint8_t index,uint8_t enable);
void APP_COMM_setInspectionTrigger(uint8_t status);

#ifdef __cplusplus
}
#endif
#endif  /* __APP_USER_INSPECT_H */
