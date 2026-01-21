#ifndef __APP_USER_UTILS_H
#define __APP_USER_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "app_user.h"

// Function declarations
uint8_t APP_USER_did2Index(uint32_t did);
void APP_USER_supply_update(uint8_t index, uint8_t floor);
void APP_COMM_clearFlooXList(uint8_t index);
void APP_COMM_supplyStaFx(uint8_t index,uint8_t flootID, uint8_t sta);
void APP_USER_UNLOCK(uint8_t index, uint8_t floor, uint8_t sta);
uint8_t APP_USER_findMapFloor2Num(int8_t floor);
void APP_USER_saveMapFloor(void);
void APP_USER_resetMapFloor(void);
void APP_USER_getMapFloor(void);
void APP_USER_updateMapFloor(void);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USER_UTILS_H */
