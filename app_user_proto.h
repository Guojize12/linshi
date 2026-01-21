#ifndef __APP_USER_PROTO_H
#define __APP_USER_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "app_user.h"

// Communication structures
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

extern user_com_def g_comm;
extern uint8_t send_buff[255];

// Function declarations for protocol handlers
void APP_COMM_Handle_R(void);
void APP_COMM_4000(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4001(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4002(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4003(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4004(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4005(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4006(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4007(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_IEEE754_to_buff(IEEE754 *value, uint8_t *buf);
void APP_COMM_4007_setParam(uint8_t number);
void APP_COMM_4008(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_5008_setParameter(uint8_t number);
uint8_t APP_COMM_4009_getPerSize(uint8_t type);
void APP_COMM_AppEnble(uint8_t sta);
void APP_COMM_4009_SetAppLicence(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4009(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4010(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4012(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_4013(uint8_t index, user_com_def *com_info, uint8_t *buf, uint16_t len);
void APP_COMM_Handle_W(void);

#ifdef __cplusplus
}
#endif
#endif /* __APP_USER_PROTO_H */
