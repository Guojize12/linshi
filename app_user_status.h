#ifndef __APP_USER_STATUS_H
#define __APP_USER_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "app_user.h"

// Function declarations
void APP_USER_SetCtlMode(uint8_t index,uint8_t ctlMode);
void APP_USER_recoverErr(uint8_t index);
void APP_USER_SendEnergy(uint8_t index);
void APP_USER_SendDoorStatusEvnet(void);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USER_STATUS_H */
