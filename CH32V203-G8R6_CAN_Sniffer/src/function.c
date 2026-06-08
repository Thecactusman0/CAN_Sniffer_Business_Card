#include "function.h"

void init_leds() 
{
    // Initialize GPIO for LEDs
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // Enable clock for GPIOA
    
    GPIO_InitStructure.GPIO_Pin = LED_STATUS_PIN | LED_RX_PIN | LED_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Push-pull output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // Set speed
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure); // Initialize GPIO with the specified settings
    
    // Turn off all LEDs initially (set each pin individually)
    GPIO_WriteBit(LED_GPIO_PORT, LED_STATUS_PIN, LED_OFF);
    GPIO_WriteBit(LED_GPIO_PORT, LED_RX_PIN, LED_OFF);
    GPIO_WriteBit(LED_GPIO_PORT, LED_TX_PIN, LED_OFF);
}
