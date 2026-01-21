#ifndef __APP_USER_RESEND_H
#define __APP_USER_RESEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "app_user.h"

typedef struct
{
    Timer g_timer_cb[5];     //= {0};
    uint8_t g_send_max[5];     //= {0};
    uint8_t g_send_buf[5][255];//= {0};
    uint8_t g_send_buf_len[5]; //= {0};
    uint8_t g_send_index;      //=0;
} bsp_send_def;

extern bsp_send_def user_send;

void APP_USER_timeout_callback0(void);
void APP_USER_timeout_callback1(void);
void APP_USER_timeout_callback2(void);
void APP_USER_timeout_callback3(void);
void APP_USER_timeout_callback4(void);

extern void (*g_to_cb[5])(void);

void to_cb_init(void);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USER_RESEND_H */
