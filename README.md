# app_user Module - Refactored

This directory contains the refactored `app_user` module, split from a monolithic 3876-line C file into 9 focused modules.

## 📋 Quick Start

**For Developers:**
```bash
# Read the quick reference first
cat QUICK_REFERENCE.md

# Understand what changed
cat REFACTORING_SUMMARY.md

# Find specific functions
cat MODULE_CONTENTS.md
```

**For Build Integration:**
```bash
# See Makefile for source list
cat Makefile

# Verify the refactoring
bash verify_refactor.sh
```

## 📦 Module Structure

| Module | Purpose | Lines | Functions |
|--------|---------|-------|-----------|
| **globals** | Global variables & timers | 17 | 0 |
| **packet** | Communication packets | 146 | 6 |
| **resend** | Retry mechanism | 81 | 6 |
| **event** | Key/event handlers | 326 | 6 |
| **status** | Status reporting | 148 | 2 |
| **inspect** | Inspection mode | 276 | 11 |
| **proto** | Protocol handlers | 1734 | 24 |
| **utils** | Utility functions | 295 | 14 |
| **main** | Main loop & RS485 | 824 | 24 |

## 🔧 Build Instructions

Replace `app_user.c` in your build with these 9 source files:

```c
// Source files
app_user_globals.c
app_user_packet.c
app_user_resend.c
app_user_event.c
app_user_status.c
app_user_inspect.c
app_user_proto.c
app_user_utils.c
app_user_main.c

// Headers (include as needed)
app_user_globals.h
app_user_packet.h
app_user_resend.h
app_user_event.h
app_user_status.h
app_user_inspect.h
app_user_proto.h
app_user_utils.h
app_user_main.h
```

## 🎯 Entry Point

```c
#include "app_user_main.h"

int main(void) {
    // Initialize the app_user module
    APP_USER_Init();
    
    // Main loop
    while(1) {
        APP_USER_RS485HANDLE();
        APP_USER_TIME_Handle();
    }
}
```

## ✅ Preservation Guarantees

- ✓ **ALL 93 functions** preserved exactly (no logic changes)
- ✓ **ALL Chinese comments** preserved (encoding intact)
- ✓ **Known bugs** preserved (e.g., line 285: `sched_No == MASTER_1`)
- ✓ **Duplicate functions** preserved (APP_COMM_4009_SetAppLicence × 2)
- ✓ **Single definitions** for all global variables
- ✓ **Identical behavior** to original monolithic file

## 📚 Documentation

| File | Description |
|------|-------------|
| **QUICK_REFERENCE.md** | Developer quick reference guide |
| **REFACTORING_SUMMARY.md** | High-level overview |
| **MODULE_CONTENTS.md** | Detailed function mapping |
| **FINAL_VERIFICATION.md** | Verification checklist |
| **SPLIT_SUMMARY.txt** | ASCII completion report |
| **FILES_CREATED.txt** | Complete file listing |

## 🔍 Finding Functions

### By Feature
- **Floor button press** → `APP_USER_SendKeyEvnet()` in `app_user_event.c`
- **Send packet** → `APP_USER_packet()` in `app_user_packet.c`
- **Protocol 4000** → `APP_COMM_4000()` in `app_user_proto.c`
- **Energy data** → `APP_USER_SendEnergy()` in `app_user_status.c`
- **Inspection mode** → `APP_USER_inspectionStart()` in `app_user_inspect.c`

### By Protocol Command
- Commands 4000-4013 → `app_user_proto.c`
- Command 5000 (key press) → `app_user_event.c`
- Command 5001 (cancel) → `app_user_event.c`
- Command 5008 (parameters) → `app_user_proto.c`
- Command 5009 (control mode) → `app_user_utils.c`

## 🐛 Known Issues

These are **intentionally preserved** from the original:

1. **Line 90 in app_user_event.c**: `sched_No == MASTER_1;` should be assignment `=` not comparison `==`
2. **Duplicate function**: `APP_COMM_4009_SetAppLicence()` appears twice in original code

## 📊 Statistics

- **Original file**: 3876 lines
- **Total modules**: 9 source + 9 header = 18 files
- **Total functions**: 93 (56 public, 37 internal)
- **Total lines**: 3847 in .c files + 260 in .h files = 4107 total
- **Reduction**: 29 lines (header consolidation only)

## 🚀 Migration Guide

### Old Code
```c
#include "app_user.h"

// Everything was in one file
APP_USER_SendKeyEvnet(floor, dir);
```

### New Code
```c
#include "app_user_event.h"  // Include specific module

// Functions moved to focused modules
APP_USER_SendKeyEvnet(floor, dir);
```

## 🔗 Dependencies

```
app_user_globals.h (base)
  ├─ app_user_packet.h
  │    └─ app_user_resend.h
  ├─ app_user_event.h
  ├─ app_user_status.h
  ├─ app_user_inspect.h
  ├─ app_user_proto.h
  ├─ app_user_utils.h
  └─ app_user_main.h (includes all)
```

## 📝 Backup

The original monolithic file is preserved as:
```
app_user.c.ORIGINAL (3876 lines)
```

## ⚙️ Verification

Run the verification script:
```bash
bash verify_refactor.sh
```

This checks:
- Function count matches
- Line count reasonable
- Chinese comments preserved
- Known bugs preserved
- No duplicate global definitions

## 📞 Support

See documentation files for detailed information:
- Architecture questions → `REFACTORING_SUMMARY.md`
- Function locations → `MODULE_CONTENTS.md`
- Usage examples → `QUICK_REFERENCE.md`

---

**Refactoring completed:** Preserving all code, comments, and bugs exactly as-is.
