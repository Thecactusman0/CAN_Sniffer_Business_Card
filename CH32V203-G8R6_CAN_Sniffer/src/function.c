#include "function.h"
#include <stdio.h>

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

void print_clock_info(void)
{
    char buffer[100];
    int len;
    
    if (tud_cdc_n_connected(0))
    {
        // Print SystemCoreClock
        len = sprintf(buffer, "SystemCoreClock: %lu Hz (%lu MHz)\r\n", 
                      SystemCoreClock, SystemCoreClock / 1000000);
        for (int i = 0; i < len; i++)
            tud_cdc_n_write_char(0, buffer[i]);
        tud_cdc_n_write_flush(0);
        
        // Read APB1 prescaler
        uint32_t apb1_presc = (RCC->CFGR0 >> 8) & 0x07;
        uint32_t apb1_div_table[] = {1, 1, 1, 1, 2, 4, 8, 16};
        uint32_t apb1_clock = SystemCoreClock / apb1_div_table[apb1_presc];
        
        len = sprintf(buffer, "APB1 Prescaler: %lu, Divisor: %lu\r\n", 
                      apb1_presc, apb1_div_table[apb1_presc]);
        for (int i = 0; i < len; i++)
            tud_cdc_n_write_char(0, buffer[i]);
        tud_cdc_n_write_flush(0);
        
        len = sprintf(buffer, "APB1 Clock: %lu Hz (%lu kHz)\r\n", 
                      apb1_clock, apb1_clock / 1000);
        for (int i = 0; i < len; i++)
            tud_cdc_n_write_char(0, buffer[i]);
        tud_cdc_n_write_flush(0);
        
        // Read APB2 prescaler
        uint32_t apb2_presc = (RCC->CFGR0 >> 11) & 0x07;
        uint32_t apb2_div_table[] = {1, 1, 1, 1, 2, 4, 8, 16};
        uint32_t apb2_clock = SystemCoreClock / apb2_div_table[apb2_presc];
        
        len = sprintf(buffer, "APB2 Clock: %lu Hz (%lu kHz)\r\n\r\n", 
                      apb2_clock, apb2_clock / 1000);
        for (int i = 0; i < len; i++)
            tud_cdc_n_write_char(0, buffer[i]);
        tud_cdc_n_write_flush(0);
    }
}
