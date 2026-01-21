/*
 * app_user.c - Monolithic file has been split into focused modules
 *
 * This file previously contained 3876 lines with 70+ functions.
 * It has been refactored into the following modules:
 *
 * 1. app_user_globals.c/h    - Global variables and definitions
 * 2. app_user_packet.c/h     - Communication packet functions
 * 3. app_user_resend.c/h     - Resend mechanism
 * 4. app_user_event.c/h      - Key/event handlers  
 * 5. app_user_status.c/h     - Status reporting
 * 6. app_user_inspect.c/h    - Inspection functions
 * 7. app_user_proto.c/h      - Protocol handlers (4000-4013, 5008)
 * 8. app_user_utils.c/h      - Utility functions
 * 9. app_user_main.c/h       - Main loop and RS485 handlers
 *
 * All functions and logic have been preserved exactly as-is.
 * Include the header files and link with the corresponding .c files.
 */

#include "app_user_main.h"

/* End of refactored file - see module files for implementation */
