#ifndef __APP_USER_PROTO_H
#define __APP_USER_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_config.h"
#include "app_user_globals.h"

void APP_COMM_Handle_R(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_Handle_W(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4000(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4001(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4002(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4003(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4004(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4005(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4006(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4007(user_com_def *comm,uint8_t *buff,uint8_t len,uint8_t pack_num);
void APP_COMM_4007_setParam(uint8_t number);
void APP_COMM_IEEE754_to_buff(uint8_t *dst,IEEE754 *src,uint8_t len);
void APP_COMM_4008(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_5008_setParameter(uint8_t number);
uint8_t APP_COMM_4009_getPerSize(uint8_t number);
void APP_COMM_AppEnble(uint8_t sta);
void APP_COMM_4009_SetAppLicence(uint8_t index,uint8_t data, uint8_t boot);
void APP_COMM_4009(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4010(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4012(user_com_def *comm,uint8_t *buff,uint8_t len);
void APP_COMM_4013(user_com_def *comm,uint8_t *buff,uint8_t len);

#ifdef __cplusplus
}
#endif
#endif  /* __APP_USER_PROTO_H */
