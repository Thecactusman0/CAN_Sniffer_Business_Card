#ifndef __CAN_H
#define __CAN_H

#include <stdint.h>
#include <debug.h>

//Using CAN1 on PA11 (RX) and PA12 (TX)
#define STANDARD_FRAME 0      // Standard frame format, 11-bit identifier
#define EXTENDED_FRAME 1      // Extended frame format, 29-bit identifier

#define STBY_PIN GPIO_Pin_8
#define STBY_PORT GPIOB
#define STBY_STATE_HIGH_SPEED 0
#define STBY_STATE_LISTEN_ONLY 1

//BIT TIMING @ 48MHz
#define CAN_SJW_ALL 1
#define CAN_BRP_1000 3
#define CAN_BS1_1000 13
#define CAN_BS2_1000 2
/*
#define CAN_BRP_500
#define CAN_BS1_500
#define CAN_BS2_500
#define CAN_BRP_250
#define CAN_BS1_250
#define CAN_BS2_250
#define CAN_BRP_100
#define CAN_BS1_100
#define CAN_BS2_100
*/
void CAN_Mode_Init(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, uint8_t mode);
void CAN_init_freq(uint16_t kbps);
CanRxMsg CAN_Receive_Msg(void);
uint8_t CAN_Send_Msg(uint32_t id, uint8_t *msg, uint8_t len, uint8_t frame_format);
#endif