# Detailed Module Contents

## Complete Function Mapping

### Module 1: app_user_globals.c/h
**Purpose:** Single source of truth for all global variables

**Global Variables Defined:**
- `user_com_def g_comm` - Communication structure
- `uint8_t send_buff[255]` - Send buffer
- `Timer g_timer_FxLoop` - Floor loop timer
- `Timer g_timer_sendEnergy` - Energy send timer
- `Timer g_timer_FloorBoardLose` - Floor board loss timer
- `Timer g_timer_MUpdate` - Master update timer
- `Timer g_timer_StartCheck` - Start check timer
- `Timer g_timer_Inspection` - Inspection timer
- `Timer g_timer_InspectionHeart` - Inspection heartbeat timer
- `st_object_info object_info` - Main object info
- `st_elevator_info elevator_info[2]` - Elevator info array

**Extern Declarations:**
- `Timer g_timer_ledcloce` - LED close timer (defined elsewhere)

**Defines:**
- `APP_WIRELESS_UART` - Wireless UART selection
- `APP_USER_UART` - User UART selection
- Buffer and length macros

---

### Module 2: app_user_packet.c/h
**Purpose:** Low-level communication packet assembly and transmission

**Functions (6):**
1. `APP_USER_Send()` - Lines 79-86 - Send via user UART
2. `APP_WIRELESS_Send()` - Lines 89-96 - Send via wireless UART
3. `APP_USER_packet()` - Lines 98-142 - Assemble packet with optional ACK
4. `APP_USER_packet_ack()` - Lines 146-170 - Assemble ACK packet
5. `APP_USER_Reply_4000()` - Lines 603-629 - Reply to 4000 command with floor
6. `APP_USER_Reply()` - Lines 632-654 - Simple reply packet

**Dependencies:**
- Includes: app_user_resend.h (for user_send)
- Uses: g_comm, send_buff, elevator_info

---

### Module 3: app_user_resend.c/h
**Purpose:** Automatic packet retransmission mechanism

**Type Definitions:**
- `bsp_send_def` - Resend buffer structure with timers

**Global Variables:**
- `bsp_send_def user_send` - Resend state machine

**Functions (6):**
1. `APP_USER_timeout_callback0()` - Lines 657-666 - Timeout handler 0
2. `APP_USER_timeout_callback1()` - Lines 668-677 - Timeout handler 1
3. `APP_USER_timeout_callback2()` - Lines 678-686 - Timeout handler 2
4. `APP_USER_timeout_callback3()` - Lines 687-695 - Timeout handler 3
5. `APP_USER_timeout_callback4()` - Lines 696-704 - Timeout handler 4
6. `to_cb_init()` - Lines 715-722 - Initialize timer callbacks

**Data:**
- `g_to_cb[5]` - Function pointer array (lines 706-713)

---

### Module 4: app_user_event.c/h
**Purpose:** Floor button press handling and smart scheduling logic

**Type Definitions:**
- `st_sched_info` - Scheduling state structure (lines 173-181)
- Scheduling enums (lines 183-188)

**Static Data:**
- `stSchedList[]` - Scheduling decision table (lines 190-200)
  Contains 6 scheduling scenarios with state machine logic

**Functions (6):**
1. `APP_USER_SendKeyEvnet()` - Lines 230-366 - **CRITICAL: Smart elevator scheduling**
   - Contains the bug on line 285: `sched_No == MASTER_1` (should be =)
   - Implements dual elevator scheduling algorithm
   - Handles MASTER_ALL, MASTER_SCHED, MASTER_NULL cases
2. `APP_USER_SendCancelKeyEvnet()` - Lines 367-392 - Cancel floor button
3. `APP_COMM_KeyLedOn()` - Lines 724-752 - Turn on floor button LED
4. `APP_COMM_KeyLedOff()` - Lines 755-807 - Turn off floor button LED
5. `APP_COMM_CanalKey()` - Lines 809-831 - Cancel key LED
6. `APP_COMM_F1Key()` - Lines 833-849 - Special F1 key handler

---

### Module 5: app_user_status.c/h
**Purpose:** Status reporting and energy data broadcasting

**Functions (2):**
1. `APP_USER_SendEnergy()` - Lines 427-508 - Send energy data
   - Formats IEEE754 floats
   - Sends power, leakage current, temperature
   - Text mode for testing (#define TEXT_ENERGY)
2. `APP_USER_SendDoorStatusEvnet()` - Lines 511-566 - Send door status
   - Broadcasts door state for all floors
   - Handles floor board loss timeout
   - Includes F1 status

---

### Module 6: app_user_inspect.c/h
**Purpose:** Inspection mode control and monitoring

**Enums:**
- `INSPERSTION_CTL_START/STOP` (lines 65-70)

**Functions (11):**
1. `APP_USER_SendCheckOption()` - Lines 570-586 - Send check option
2. `APP_USER_SendSimulationInCall()` - Lines 589-600 - Simulate internal call
3. `APP_COMM_setInspectionTrigger()` - Lines 2625-2651 - Set inspection trigger
4. `APP_USER_inspectionStart()` - Lines 2653-2662 - Start inspection
5. `APP_USER_inspectionReady()` - Lines 2664-2679 - Inspection ready state
6. `APP_USER_inspectionHeart()` - Lines 2681-2686 - Inspection heartbeat
7. `APP_USER_inspectionTrigger()` - Lines 2688-2727 - Trigger inspection
8. `APP_USER_inspectionForceStop()` - Lines 2729-2738 - Force stop
9. `APP_USER_inspectionStop()` - Lines 2740-2755 - Stop inspection
10. `APP_COMM_4009_SetInspectionAllowUpdate()` - Lines 2303-2378 - Update allow flag
11. `APP_COMM_4009_SetInspectionEnable()` - Lines 2380-2402 - Enable/disable

---

### Module 7: app_user_proto.c/h
**Purpose:** Protocol command handlers (largest module)

**Static Data:**
- `energy_all_buf[ENERGY_ALL_LEN]` - Energy parameter buffer (line 1475)

**Functions (24):**
1. `APP_COMM_Handle_R()` - Lines 853-925 - Read command handler
   - Processes responses (5000, 5001, 5002, 5009, 5013)
   - Stops resend timers
2. `APP_COMM_4000()` - Lines 929-942 - Cancel button command
3. `APP_COMM_4001()` - Lines 946-1028 - Real-time status (floor, height, load)
4. `APP_COMM_4002()` - Lines 1089-1193 - Supply list and smart mode
5. `APP_COMM_4003()` - Lines 1195-1217 - Set mode (floor count)
6. `APP_COMM_4004()` - Lines 1242-1273 - Door unlock command
7. `APP_COMM_4005()` - Lines 1276-1428 - Start check items
8. `APP_COMM_4006()` - Lines 1430-1472 - Device status (sample/control)
9. `APP_COMM_4007()` - Lines 1477-1644 - Read energy parameters (large function)
   - Formats all 28 parameters
   - Uses IEEE754 conversion
10. `APP_COMM_IEEE754_to_buff()` - Lines 1653-1672 - Float to buffer utility
11. `APP_COMM_4007_setParam()` - Lines 1674-1787 - Set single energy parameter
12. `APP_COMM_4008()` - Lines 1790-1910 - Write energy parameters
13. `APP_COMM_5008_setParameter()` - Lines 1914-2107 - Format energy parameter
14. `APP_COMM_4009_getPerSize()` - Lines 2109-2120 - Get parameter size
15. `APP_COMM_AppEnble()` - Lines 2122-2145 - Application enable control
16. `APP_COMM_4009_SetAppLicence()` - Lines 2147-2229 - Set license (1st copy)
17. `APP_COMM_4009_SetAppLicence()` - Lines 2231-2301 - Set license (2nd copy - duplicate!)
18. `APP_COMM_4009()` - Lines 2404-2487 - Write mode/parameter command
19. `APP_COMM_4010()` - Lines 2489-2580 - Floor mapping command
20. `APP_COMM_4012()` - Lines 2582-2623 - Display configuration
21. `APP_COMM_4013()` - Lines 2757-2840 - Inspection command
22. `APP_COMM_Handle_W()` - Lines 2842-2907 - Write command dispatcher

---

### Module 8: app_user_utils.c/h
**Purpose:** Utility and helper functions

**Functions (14):**
1. `APP_USER_SupplyListHave()` - Lines 202-227 - Check if floor in supply list
2. `APP_USER_SetCtlMode()` - Lines 394-409 - Set control mode
3. `APP_USER_recoverErr()` - Lines 411-423 - Recover from error
4. `APP_USER_did2Index()` - Lines 1030-1041 - Device ID to array index
5. `APP_USER_supply_update()` - Lines 1044-1060 - Update supply count
6. `APP_COMM_clearFlooXList()` - Lines 1062-1074 - Clear floor list
7. `APP_COMM_supplyStaFx()` - Lines 1076-1086 - Set supply status bits
8. `APP_USER_UNLOCK()` - Lines 1219-1240 - Door unlock control
9. `APP_USER_UpdateComm_sta()` - Lines 3618-3651 - Update comm status
10. `APP_USER_findMapFloor2Num()` - Lines 3667-3681 - Find floor in mapping
11. `APP_USER_saveMapFloor()` - Lines 3683-3705 - Save floor mapping
12. `APP_USER_resetMapFloor()` - Lines 3707-3718 - Reset floor mapping
13. `APP_USER_getMapFloor()` - Lines 3720-3768 - Get floor mapping from config
14. `APP_USER_updateMapFloor()` - Lines 3770-3784 - Update floor mapping

---

### Module 9: app_user_main.c/h
**Purpose:** Main event loop, RS485 handling, initialization

**Functions (24):**
1. `APP_COMM_Rx_Analysis()` - Lines 2909-2976 - Parse incoming protocol
2. `APP_WIRELESS_Rx_Analysis()` - Lines 2978-3013 - Parse wireless protocol
3. `APP_USER_Rx_Handle()` - Lines 3015-3028 - Main UART RX handler
4. `APP_WIRELESS_Rx_Handle()` - Lines 3030-3050 - Wireless UART RX handler
5. `APP_USER_Handle06()` - Lines 3052-3068 - Modbus function 06 (write single)
6. `APP_USER_Handle03()` - Lines 3070-3251 - Modbus function 03 (read holding) **LARGEST**
   - 182 lines
   - Reads floor button states
   - Updates LED states
7. `APP_USER_Handle10()` - Lines 3253-3257 - Modbus function 10 (write multiple)
8. `APP_RS485_Rec_Handle()` - Lines 3259-3276 - RS485 receive dispatcher
9. `APP_USER_RS485HANDLE()` - Lines 3278-3305 - Main RS485 handler
10. `APP_USER_SlaveDataHandle()` - Lines 3307-3352 - Process slave data
11. `APP_USER_FxLoop_SetStatus()` - Lines 3354-3372 - Set floor loop status
12. `APP_USER_FxLoop()` - Lines 3374-3401 - Floor loop periodic task
13. `APP_USER_sendEnergy()` - Lines 3403-3418 - Energy send scheduler
14. `APP_USER_FxLockUpdate()` - Lines 3420-3455 - Update floor locks
15. `APP_USER_MUpdate()` - Lines 3457-3562 - Master communication update
16. `APP_USER_info_Init()` - Lines 3564-3616 - Initialize object info
17. `APP_USER_MStatusNo1_Handle()` - Lines 3653-3658 - Master 1 status
18. `APP_USER_MStatusNo2_Handle()` - Lines 3660-3665 - Master 2 status
19. `APP_USER_FloorBoardLose()` - Lines 3786-3799 - Handle floor board loss
20. `APP_USER_TIME_Handle()` - Lines 3801-3808 - Time event handler
21. `APP_USER_startCheck_Handle()` - Lines 3810-3816 - Start check handler
22. `APP_USER_Init()` - Lines 3818-3875 - **MAIN INITIALIZATION**
    - Initializes all timers
    - Sets up callbacks
    - Configures UART
    - Entry point for module

---

## Module Size Comparison

| Module | Lines | Complexity | Primary Responsibility |
|--------|-------|------------|------------------------|
| proto | 1734 | High | Protocol state machine |
| main | 824 | High | Event loop & RS485 |
| event | 326 | Medium | Smart scheduling |
| utils | 295 | Low | Helper functions |
| inspect | 276 | Medium | Inspection mode |
| status | 148 | Low | Status broadcasts |
| packet | 146 | Low | Packet assembly |
| resend | 81 | Medium | Retry mechanism |
| globals | 17 | N/A | Data definitions |
| **TOTAL** | **3847** | | |

---

## Critical Preserved Elements

1. **Bug on Original Line 285** (now in app_user_event.c line 90):
   ```c
   sched_No == MASTER_1;  // Should be = not ==
   ```
   This is preserved exactly as-is per requirements.

2. **Duplicate Function** (lines 2147-2229 and 2231-2301):
   `APP_COMM_4009_SetAppLicence()` appears twice in original.
   Both copies preserved in app_user_proto.c.

3. **Chinese Comments**:
   All Chinese comments preserved byte-for-byte (may display as encoded).

4. **IEEE754 Float Handling**:
   All IEEE754 conversions preserved exactly (endian swaps, etc.).

5. **Static Data**:
   - `stSchedList[]` - 6-element scheduling table
   - `energy_all_buf[]` - Energy parameter buffer

---

## Integration Notes

To use the refactored code:

1. **Replace in build system:**
   ```
   OLD: app_user.c
   NEW: app_user_globals.c + app_user_packet.c + app_user_resend.c +
        app_user_event.c + app_user_status.c + app_user_inspect.c +
        app_user_proto.c + app_user_utils.c + app_user_main.c
   ```

2. **Include paths:** No changes needed - all headers remain in same directory

3. **Entry point:** `APP_USER_Init()` in app_user_main.c

4. **Dependencies:** See REFACTORING_SUMMARY.md for module dependency tree

---

**End of Module Contents Documentation**
