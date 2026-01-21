#ifndef __APP_USER_PACKET_H
#define __APP_USER_PACKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "app_user.h"

// UART macros
#ifdef USE_UART4
#define APP_WIRELESS_UART          BSP_UART4
#define APP_WIRELESS_UART_RX_LEN        UART4_RX_LEN
#define	APP_WIRELESS_UART_BUF			 g_uart_buf[APP_WIRELESS_UART]
#endif

#ifdef USE_UART5
#define APP_USER_UART          BSP_UART5
#define APP_USER_UART_RX_LEN        UART5_RX_LEN
#endif

#define	APP_USER_UART_BUF			 g_uart_buf[APP_USER_UART]

#define MAX_TIME 5

// Function declarations
void APP_USER_Send(uint8_t *buf, uint16_t buf_len);
void APP_WIRELESS_Send(uint8_t *buf, uint16_t buf_len);
void APP_USER_packet(uint16_t cmd,st_elevator_info *elev_info,uint8_t *buf,uint8_t len,uint8_t type);//type 0 ����Ӧ��  1  ��Ӧ��
void APP_USER_packet_ack(user_com_def *comm,uint8_t *buf,uint8_t len);
void APP_USER_Reply(user_com_def *comm,uint8_t pack_num);
void APP_USER_Reply_4000(user_com_def *comm,uint8_t pack_num,uint8_t floor);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USER_PACKET_H */
