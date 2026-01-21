#ifndef __APP_USER_PACKET_H
#define __APP_USER_PACKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"
#include "app_user_globals.h"

void APP_USER_Send(uint8_t *buf, uint16_t buf_len);
void APP_WIRELESS_Send(uint8_t *buf, uint16_t buf_len);
void APP_USER_packet(uint16_t cmd,st_elevator_info *elev_info,uint8_t *buf,uint8_t len,uint8_t type);
void APP_USER_packet_ack(user_com_def *comm,uint8_t *buf,uint8_t len);
void APP_USER_Reply_4000(user_com_def *comm,uint8_t pack_num,uint8_t floor);
void APP_USER_Reply(user_com_def *comm,uint8_t pack_num);

#ifdef __cplusplus
}
#endif
#endif  /* __APP_USER_PACKET_H */
