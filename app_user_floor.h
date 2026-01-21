#ifndef __APP_USER_FLOOR_H
#define __APP_USER_FLOOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "app_user.h"

// Function declarations
void APP_USER_Handle06(void);
void APP_USER_Handle03(void);
void APP_USER_Handle10(void);
void APP_RS485_Rec_Handle(void);
void APP_USER_RS485HANDLE(void);
void APP_USER_SlaveDataHandle(void);
void APP_USER_FxLoop_SetStatus(uint8_t sta);
void APP_USER_FxLoop(void);
void APP_USER_FxLockUpdate(void);
void APP_USER_FloorBoardLose(void);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USER_FLOOR_H */
