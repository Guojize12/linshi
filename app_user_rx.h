#ifndef __APP_USER_RX_H
#define __APP_USER_RX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "app_user.h"

// Function declarations
void APP_COMM_Rx_Analysis(uint8_t *buff,uint16_t size);
void APP_WIRELESS_Rx_Analysis(uint8_t *buff,uint16_t size);
void APP_USER_Rx_Handle(void);
void APP_WIRELESS_Rx_Handle(void);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USER_RX_H */
