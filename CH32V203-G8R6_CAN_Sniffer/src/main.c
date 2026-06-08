#include "function.h"


int main(void) 
{
    board_init(); //init usb 
    init_leds();

    //CAN_init_freq(1000);
    uint32_t timeElapsed = 0;
    while (1) 
    {
        tud_task(); // tinyusb device task
        uint8_t msg = 0b01010101;
        //CAN_Send_Msg(0x800,&msg, 1, STANDARD_FRAME);
        
        
        if(board_millis() - timeElapsed > 1000)
        {
            timeElapsed = board_millis();
            SET_TX_LED(LED_ON);
            if (tud_cdc_n_connected(0)) 
            {
            uint8_t msg[] = "Hello World!\r\n";
            for (size_t i = 0; i < sizeof(msg) - 1; i++)
            {
                tud_cdc_n_write_char(0, msg[i]);
            }
            tud_cdc_n_write_flush(0);
            }
        }
        
        SET_STATUS_LED(LED_ON);
        
    }
    return 0;
}
