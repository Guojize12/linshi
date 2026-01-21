#ifndef __APP_USER_STATUS_H
#define __APP_USER_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"
#include "app_user_globals.h"

void APP_USER_SendDoorStatusEvnet(void);
void APP_USER_SendEnergy(void);

#ifdef __cplusplus
}
#endif
#endif  /* __APP_USER_STATUS_H */
