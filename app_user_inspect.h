#ifndef __APP_USER_INSPECT_H
#define __APP_USER_INSPECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "app_user.h"

// Inspection control enumerations
enum
{
    INSPERSTION_CTL_START=0,  
    INSPERSTION_CTL_STOP,
    INSPERSTION_CTL_MAX,
};

// Function declarations
void APP_USER_SendCheckOption(uint8_t index,uint16_t opt, uint8_t event);
void APP_USER_SendSimulationInCall(uint8_t index,uint8_t floor);
void APP_COMM_setInspectionTrigger(uint8_t index, uint8_t type, uint8_t sta);
void APP_USER_inspectionStart(uint8_t index);
void APP_USER_inspectionReady(uint8_t index);
void APP_USER_inspectionHeart(void);
void APP_USER_inspectionTrigger(void);
void APP_USER_inspectionForceStop(void);
void APP_USER_inspectionStop(uint8_t index);
void APP_COMM_4009_SetInspectionAllowUpdate(uint8_t index);
void APP_COMM_4009_SetInspectionEnable(uint8_t index, uint8_t event);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USER_INSPECT_H */
