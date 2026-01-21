#ifndef __APP_USER_EVENT_H
#define __APP_USER_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"
#include "app_user_globals.h"

void APP_USER_SendKeyEvnet(uint8_t floor,uint8_t direction);
void APP_USER_SendCancelKeyEvnet(uint8_t floor,uint8_t direction);
void APP_COMM_KeyLedOn(uint8_t index, uint8_t flootID, uint8_t direction);
void APP_COMM_KeyLedOff(uint8_t index, uint8_t flootID, uint8_t direction);
void APP_COMM_CanalKey(uint8_t index, uint8_t flootID, uint8_t direction);
void APP_COMM_F1Key(uint8_t index, uint8_t sta);

#ifdef __cplusplus
}
#endif
#endif  /* __APP_USER_EVENT_H */
