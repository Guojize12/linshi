#ifndef __APP_USER_GLOBALS_H
#define __APP_USER_GLOBALS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_user.h"

// ����
typedef struct {
    uint8_t dnum     ;
    uint8_t type     ;
    uint16_t cmd      ;
    uint8_t pid      ;
    uint8_t pid_len  ;
    bsp_id_def did  ;
    uint8_t head      ;
    uint8_t pack_num;
} user_com_def;

// Global variables
extern user_com_def g_comm;
extern uint8_t send_buff[255];
extern st_object_info object_info;
extern st_elevator_info elevator_info[2];

// Timers
extern Timer g_timer_FxLoop;
extern Timer g_timer_sendEnergy;
extern Timer g_timer_FloorBoardLose;
extern Timer g_timer_MUpdate;
extern Timer g_timer_StartCheck;
extern Timer g_timer_Inspection;
extern Timer g_timer_InspectionHeart;
extern Timer g_timer_ledcloce;

#ifdef __cplusplus
}
#endif
#endif  /* __APP_USER_GLOBALS_H */
