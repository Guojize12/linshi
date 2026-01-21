# Quick Reference Guide - app_user Module Split

## 📁 Module Map

| Module | Purpose | Key Functions | Include This For |
|--------|---------|---------------|------------------|
| **globals** | Global state | (data only) | Any module needing globals |
| **packet** | Communication | `APP_USER_Send`, `APP_USER_packet` | Sending data |
| **resend** | Retry logic | `APP_USER_timeout_callback*` | Automatic retries |
| **event** | Button handling | `APP_USER_SendKeyEvnet` | Floor button presses |
| **status** | Reporting | `APP_USER_SendEnergy` | Status/energy data |
| **inspect** | Inspection mode | `APP_USER_inspectionStart` | Inspection features |
| **proto** | Protocol handlers | `APP_COMM_4000-4013` | Protocol parsing |
| **utils** | Utilities | `APP_USER_did2Index` | Helper functions |
| **main** | Entry point | `APP_USER_Init` | Main loop & initialization |

## 🔧 Common Tasks

### Want to send a key event?
```c
#include "app_user_event.h"
APP_USER_SendKeyEvnet(floor, direction);
```

### Want to send data to elevator?
```c
#include "app_user_packet.h"
APP_USER_packet(cmd, &elevator_info[index], buffer, len, 1);
```

### Want to access global variables?
```c
#include "app_user_globals.h"
// Now you have: object_info, elevator_info[], g_comm, send_buff[]
```

### Want to handle protocol command?
```c
#include "app_user_proto.h"
// Protocol handlers: APP_COMM_4000() through APP_COMM_4013()
```

### Want to initialize the module?
```c
#include "app_user_main.h"
APP_USER_Init();
```

## 📊 Module Dependencies

```
Level 0: app_user_globals.h (base)
         ↓
Level 1: app_user_packet.h, app_user_utils.h
         ↓
Level 2: app_user_resend.h, app_user_event.h, app_user_status.h
         ↓
Level 3: app_user_inspect.h, app_user_proto.h
         ↓
Level 4: app_user_main.h (top - includes all)
```

**Rule of thumb:** If in doubt, include `app_user_main.h` (it includes everything)

## 🐛 Known Issues (Preserved)

- **Line 90 in app_user_event.c**: `sched_No == MASTER_1;` should be `=` but intentionally kept
- **Duplicate function**: `APP_COMM_4009_SetAppLicence` appears twice (lines 2147 & 2231 in original)

## 🔍 Finding Functions

### By Protocol Command
- `4000` → `APP_COMM_4000()` in **proto**
- `5000` → Sent by `APP_USER_SendKeyEvnet()` in **event**
- `5001` → Sent by `APP_USER_SendCancelKeyEvnet()` in **event**

### By Feature
- **Key press** → **event** module
- **Door status** → **status** module
- **Energy data** → **status** module
- **Inspection** → **inspect** module
- **RS485** → **main** module
- **Modbus** → **main** module

## 🎯 Entry Points

| Function | Module | Called By | Purpose |
|----------|--------|-----------|---------|
| `APP_USER_Init()` | main | Application | Initialize everything |
| `APP_USER_Rx_Handle()` | main | UART ISR | Handle received data |
| `APP_USER_RS485HANDLE()` | main | Main loop | Process RS485 |
| `APP_USER_TIME_Handle()` | main | Timer | Periodic tasks |

## 📝 Global Variables

All in `app_user_globals.c`:
- `object_info` - System state
- `elevator_info[2]` - Elevator 1 & 2 state
- `g_comm` - Communication structure
- `send_buff[255]` - Send buffer
- `g_timer_*` - Various timers

## ⚙️ Build Configuration

### CMake
```cmake
set(APP_USER_SOURCES
    app_user_globals.c
    app_user_packet.c
    app_user_resend.c
    app_user_event.c
    app_user_status.c
    app_user_inspect.c
    app_user_proto.c
    app_user_utils.c
    app_user_main.c
)
```

### Makefile
```makefile
OBJS += app_user_globals.o \
        app_user_packet.o \
        app_user_resend.o \
        app_user_event.o \
        app_user_status.o \
        app_user_inspect.o \
        app_user_proto.o \
        app_user_utils.o \
        app_user_main.o
```

## 🚀 Migration Tips

1. **Old code** included `app_user.h` → **New code** include specific module header
2. **Static functions** stay in same module (not in headers)
3. **All public functions** have prototypes in corresponding `.h` file
4. **Timers** are in `globals` module
5. **Entry point** is `APP_USER_Init()` in `main` module

## 📞 Support

For questions about:
- **Scheduling logic** → See `app_user_event.c` (smart scheduling)
- **Protocol format** → See `app_user_packet.c` (packet structure)
- **Energy data** → See `app_user_status.c` (IEEE754 formatting)
- **State machine** → See `app_user_main.c` (main loops)
