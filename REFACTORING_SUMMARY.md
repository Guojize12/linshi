# app_user.c Refactoring Summary

## Overview
Successfully split the monolithic `app_user.c` (3876 lines) into 10 focused modules plus the main orchestration file.

## Created Modules

### 1. app_user_globals.h / app_user_globals.c
**Purpose:** Global variables and timers
**Content:**
- `user_com_def g_comm` - Global communication structure
- `uint8_t send_buff[255]` - Send buffer
- `st_object_info object_info` - Object info
- `st_elevator_info elevator_info[2]` - Elevator info array
- Timers: `g_timer_FxLoop`, `g_timer_sendEnergy`, `g_timer_FloorBoardLose`, 
  `g_timer_MUpdate`, `g_timer_StartCheck`, `g_timer_Inspection`, `g_timer_InspectionHeart`
- `extern Timer g_timer_ledcloce` (defined elsewhere)

### 2. app_user_resend.h / app_user_resend.c  
**Purpose:** Resend mechanism with timeout callbacks
**Content:**
- `bsp_send_def` structure (lines 4-11)
- `user_send` variable
- `APP_USER_timeout_callback0-4()` functions
- `g_to_cb[]` callback array
- `to_cb_init()` function

### 3. app_user_packet.h / app_user_packet.c
**Purpose:** Packet building and transmission
**Content:**
- UART macros (APP_WIRELESS_UART, APP_USER_UART, etc.)
- `APP_USER_Send()` - Send via user UART
- `APP_WIRELESS_Send()` - Send via wireless UART  
- `APP_USER_packet()` - Build and send packet
- `APP_USER_packet_ack()` - Build and send ACK packet
- `APP_USER_Reply()` - Send reply
- `APP_USER_Reply_4000()` - Send 4000 reply

### 4. app_user_event.h / app_user_event.c
**Purpose:** Event handling, key events, LED control
**Content:**
- `st_sched_info` structure and `stSchedList[]` array
- `APP_USER_SupplyListHave()` - Check if floor has supply
- `APP_USER_SendKeyEvnet()` - Send key event (scheduling logic)
- `APP_USER_SendCancelKeyEvnet()` - Cancel key event
- `APP_COMM_KeyLedOn()` - Turn on key LED
- `APP_COMM_KeyLedOff()` - Turn off key LED
- `APP_COMM_CanalKey()` - Cancel key
- `APP_COMM_F1Key()` - F1 key control

### 5. app_user_status.h / app_user_status.c
**Purpose:** Control mode and status management
**Content:**
- `APP_USER_SetCtlMode()` - Set control mode
- `APP_USER_recoverErr()` - Recover from error
- `APP_USER_SendEnergy()` - Send energy data
- `APP_USER_SendDoorStatusEvnet()` - Send door status event

### 6. app_user_inspect.h / app_user_inspect.c
**Purpose:** Inspection mode control
**Content:**
- `INSPERSTION_CTL_*` enum
- `APP_USER_SendCheckOption()` - Send check option
- `APP_USER_SendSimulationInCall()` - Send simulation call
- `APP_COMM_setInspectionTrigger()` - Set inspection trigger
- `APP_USER_inspectionStart()` - Start inspection
- `APP_USER_inspectionReady()` - Ready for inspection
- `APP_USER_inspectionHeart()` - Inspection heartbeat
- `APP_USER_inspectionTrigger()` - Trigger inspection
- `APP_USER_inspectionForceStop()` - Force stop inspection
- `APP_USER_inspectionStop()` - Stop inspection
- `APP_COMM_4009_SetInspectionAllowUpdate()` - Update inspection allow flag
- `APP_COMM_4009_SetInspectionEnable()` - Enable/disable inspection

### 7. app_user_proto.h / app_user_proto.c
**Purpose:** All protocol handlers (4000-4013 commands)
**Content:**
- `APP_COMM_Handle_R()` - Handle read commands
- `APP_COMM_Handle_W()` - Handle write commands
- `APP_COMM_4000()` through `APP_COMM_4013()` - Individual command handlers
- `APP_COMM_5008_setParameter()` - Set parameter
- `APP_COMM_AppEnble()` - Enable/disable app
- `APP_COMM_4009_getPerSize()` - Get parameter size
- `APP_COMM_4009_SetAppLicence()` - Set app license
- `APP_COMM_IEEE754_to_buff()` - Convert IEEE754 to buffer
- `APP_COMM_4007_setParam()` - Set 4007 parameters

### 8. app_user_utils.h / app_user_utils.c
**Purpose:** Utility functions
**Content:**
- `APP_USER_did2Index()` - Convert DID to index
- `APP_USER_supply_update()` - Update supply status
- `APP_COMM_clearFlooXList()` - Clear floor list
- `APP_COMM_supplyStaFx()` - Supply station function
- `APP_USER_UNLOCK()` - Unlock function
- `APP_USER_findMapFloor2Num()` - Find floor mapping
- `APP_USER_saveMapFloor()` - Save floor mapping
- `APP_USER_resetMapFloor()` - Reset floor mapping
- `APP_USER_getMapFloor()` - Get floor mapping
- `APP_USER_updateMapFloor()` - Update floor mapping

### 9. app_user_floor.h / app_user_floor.c
**Purpose:** Floor board communication and control
**Content:**
- `APP_USER_Handle03()` - Handle 03 command
- `APP_USER_Handle06()` - Handle 06 command
- `APP_USER_Handle10()` - Handle 10 command
- `APP_RS485_Rec_Handle()` - RS485 receive handler
- `APP_USER_RS485HANDLE()` - RS485 handler
- `APP_USER_SlaveDataHandle()` - Slave data handler
- `APP_USER_FxLoop_SetStatus()` - Set FxLoop status
- `APP_USER_FxLoop()` - Floor loop function
- `APP_USER_FxLockUpdate()` - Update floor lock
- `APP_USER_FloorBoardLose()` - Handle floor board loss

### 10. app_user_rx.h / app_user_rx.c
**Purpose:** Receive data parsing and handling
**Content:**
- `APP_COMM_Rx_Analysis()` - Analyze received data
- `APP_WIRELESS_Rx_Analysis()` - Analyze wireless received data
- `APP_USER_Rx_Handle()` - User RX handler
- `APP_WIRELESS_Rx_Handle()` - Wireless RX handler

### 11. app_user.c (Updated)
**Purpose:** Main orchestration and initialization
**Content:**
- All module includes
- `APP_USER_sendEnergy()` - Energy send wrapper
- `APP_USER_MUpdate()` - Master update
- `APP_USER_info_Init()` - Initialize info
- `APP_USER_UpdateComm_sta()` - Update communication status
- `APP_USER_MStatusNo1_Handle()` - Master 1 status handler
- `APP_USER_MStatusNo2_Handle()` - Master 2 status handler
- `APP_USER_TIME_Handle()` - Time handler
- `APP_USER_startCheck_Handle()` - Start check handler
- `APP_USER_Init()` - Main initialization

## Module Sizes
```
app_user.c:          324 lines (down from 3876)
app_user_globals.c:   18 lines
app_user_resend.c:    62 lines
app_user_packet.c:   153 lines
app_user_event.c:    344 lines
app_user_status.c:   185 lines
app_user_inspect.c:  283 lines
app_user_proto.c:   1771 lines
app_user_utils.c:    216 lines
app_user_floor.c:    647 lines
app_user_rx.c:       152 lines
Total .c files:     4155 lines (includes headers and blank lines)
```

## Header Files
All modules have corresponding `.h` files with:
- Proper include guards (`#ifndef __APP_USER_MODULE_H`)
- C++ compatibility (`#ifdef __cplusplus`)
- Forward declarations and `extern` declarations
- Function prototypes

## Key Principles Maintained
✅ **Zero Logic Changes** - All code preserved exactly as-is  
✅ **No Omissions** - Every function, variable, comment preserved  
✅ **Single Definition** - Global variables defined once, extern elsewhere  
✅ **Chinese Comments** - All preserved exactly  
✅ **Clean Dependencies** - Proper module includes  

## Compilation Notes
- Each module includes only necessary headers
- Circular dependencies avoided through proper forward declarations
- All modules can be compiled independently
- Main app_user.c includes all module headers

## Migration Path
1. Original file backed up as `app_user.c.backup`
2. All new modules created in same directory
3. Build system should be updated to include all new `.c` files
4. No changes needed to calling code - all public APIs remain in `app_user.h`
