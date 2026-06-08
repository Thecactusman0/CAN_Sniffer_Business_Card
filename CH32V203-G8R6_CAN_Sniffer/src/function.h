#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdlib.h>
#include "ch32v20x.h"
#include <board_api.h>
#include <tusb.h>
#include <debug.h>
#include "can.h"

#define LED_GPIO_PORT GPIOA
#define LED_STATUS_PIN GPIO_Pin_0
#define LED_RX_PIN GPIO_Pin_1
#define LED_TX_PIN GPIO_Pin_2

#define LED_ON 0
#define LED_OFF 1

#define SET_RX_LED(State) GPIO_WriteBit(LED_GPIO_PORT, LED_RX_PIN, State)
#define SET_TX_LED(State) GPIO_WriteBit(LED_GPIO_PORT, LED_TX_PIN, State)
#define SET_STATUS_LED(State) GPIO_WriteBit(LED_GPIO_PORT, LED_STATUS_PIN, State)

void init_leds();

#endif