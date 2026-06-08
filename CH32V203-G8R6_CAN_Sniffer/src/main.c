#include "function.h"


int main(void) 
{
    board_init(); //init usb 
    init_leds();

    CAN_init_freq(100);
    
    // Wait a moment for USB to be ready
    for(uint32_t i = 0; i < 1000000; i++);
    
    // Print clock information
    
    
    uint32_t timeElapsed = 0;
    uint8_t led_state = LED_ON;
    while (1) 
    {
        tud_task(); // tinyusb device task
        uint8_t msg[] = {0x11};
        
        
        if(board_millis() - timeElapsed > 1000)
        {
            timeElapsed = board_millis();
            //SET_TX_LED(LED_ON);
            /*if (tud_cdc_n_connected(0)) 
            {
            uint8_t msg[] = "Hello World!\r\n";
            for (size_t i = 0; i < sizeof(msg) - 1; i++)
            {
                tud_cdc_n_write_char(0, msg[i]);
            }
            tud_cdc_n_write_flush(0);
            }*/
            CAN_Send_Msg(0x456,msg, 1, STANDARD_FRAME);
            
            // Toggle LED to show activity
            led_state = (led_state == LED_ON) ? LED_OFF : LED_ON;
            SET_TX_LED(led_state);
        }
        
    }
    return 0;
}
