#ifndef __APP_USER_RX_H
#define __APP_USER_RX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "app_user.h"

// Communication structures (forward declaration)
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

// Function declarations
void APP_COMM_Rx_Analysis(void);
void APP_WIRELESS_Rx_Analysis(void);
void APP_USER_Rx_Handle(void);
void APP_WIRELESS_Rx_Handle(void);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USER_RX_H */
