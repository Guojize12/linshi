# Build System Integration Guide

## Overview
This document provides guidance on integrating the refactored app_user modules into your build system.

## Files to Compile

### New Source Files (10 files)
Add these files to your compilation units:

1. `app_user_globals.c` - Global variables and timers
2. `app_user_resend.c` - Resend mechanism
3. `app_user_packet.c` - Packet building/transmission
4. `app_user_event.c` - Event handling and key control
5. `app_user_status.c` - Status management
6. `app_user_inspect.c` - Inspection mode
7. `app_user_proto.c` - Protocol handlers (largest module)
8. `app_user_utils.c` - Utility functions
9. `app_user_floor.c` - Floor board communication
10. `app_user_rx.c` - Data reception

### Updated File
- `app_user.c` - Now only contains orchestration functions

### Header Files (included automatically)
All corresponding `.h` files will be included by the compiler when processing the `.c` files.

## Makefile Example

```makefile
# Add to your SOURCES variable
SOURCES += app_user.c \
           app_user_globals.c \
           app_user_resend.c \
           app_user_packet.c \
           app_user_event.c \
           app_user_status.c \
           app_user_inspect.c \
           app_user_proto.c \
           app_user_utils.c \
           app_user_floor.c \
           app_user_rx.c
```

## CMakeLists.txt Example

```cmake
# Add to your target sources
target_sources(${TARGET_NAME} PRIVATE
    app_user.c
    app_user_globals.c
    app_user_resend.c
    app_user_packet.c
    app_user_event.c
    app_user_status.c
    app_user_inspect.c
    app_user_proto.c
    app_user_utils.c
    app_user_floor.c
    app_user_rx.c
)
```

## Keil µVision Example

In your Keil project:
1. Right-click on the source group
2. Add Existing Files
3. Select all 11 `.c` files (including updated app_user.c)

## IAR Embedded Workbench Example

In your IAR project:
1. Right-click on the project
2. Add → Add Files
3. Select all 11 `.c` files

## GCC Command Line Example

```bash
gcc -c app_user.c \
    app_user_globals.c \
    app_user_resend.c \
    app_user_packet.c \
    app_user_event.c \
    app_user_status.c \
    app_user_inspect.c \
    app_user_proto.c \
    app_user_utils.c \
    app_user_floor.c \
    app_user_rx.c \
    [other flags...]
```

## Include Paths

No changes needed - all headers are in the same directory as the source files.

## Dependencies

### Module Dependencies (informational)
- `app_user.c` → includes all module headers
- `app_user_packet.c` → depends on `app_user_globals.h`, `app_user_resend.h`
- `app_user_event.c` → depends on `app_user_globals.h`, `app_user_packet.h`
- `app_user_proto.c` → depends on most other modules
- Other modules have minimal cross-dependencies

## Verification After Build

After integrating and building:

1. ✅ Check that all 11 `.c` files compile without errors
2. ✅ Verify no "undefined reference" errors
3. ✅ Ensure no "multiple definition" errors
4. ✅ Test that existing functionality still works

## Common Issues

### Multiple Definition Errors
**Cause:** Global variables defined in multiple translation units.
**Solution:** Ensure you're compiling the split modules correctly. Each global should be defined once (in `app_user_globals.c` or `app_user_resend.c`) and declared as `extern` elsewhere.

### Undefined Reference Errors
**Cause:** Missing source file in build.
**Solution:** Ensure all 11 `.c` files are added to your build system.

### Include Errors
**Cause:** Missing include paths.
**Solution:** All headers are in the same directory - ensure the directory is in your include path.

## Rollback Plan

If issues occur, the original file is backed up as `app_user.c.backup` (117KB).

To rollback:
```bash
cp app_user.c.backup app_user.c
# Remove the 10 new module pairs from your build
```

## Summary

- ✅ Add 10 new `.c` files to your build
- ✅ Keep updated `app_user.c` in build
- ✅ No changes to include paths needed
- ✅ No changes to public API (app_user.h)
- ✅ Test after building

For questions or issues, refer to `REFACTORING_SUMMARY.md` and `VERIFICATION_REPORT.md`.
