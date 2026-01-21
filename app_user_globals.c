#include "app_user_globals.h"

//����
user_com_def g_comm = {0};
uint8_t send_buff[255] = {0};

// Global objects
st_object_info object_info = {0};
st_elevator_info elevator_info[2] = {0};

// Timer definitions
Timer g_timer_FxLoop = {0};
Timer g_timer_sendEnergy = {0};
Timer g_timer_FloorBoardLose = {0};
Timer g_timer_MUpdate = {0};
Timer g_timer_StartCheck = {0};
Timer g_timer_Inspection = {0};
Timer g_timer_InspectionHeart = {0};
