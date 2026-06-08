#include "can.h"

/*********************************************************************
 * @fn      CAN_Mode_Init
 *
 * @brief   Initializes CAN communication test mode.
 *          Bps =Fpclk1/((tpb1+1+tbs2+1+1)*brp)
 *
 * @param   tsjw - CAN synchronisation jump width.
 *          tbs2 - CAN time quantum in bit segment 1.
 *          tbs1 - CAN time quantum in bit segment 2.
 *          brp - Specifies the length of a time quantum.
 *          mode - Test mode.
 *            CAN_Mode_Normal.
 *            CAN_Mode_LoopBack.
 *            CAN_Mode_Silent.
 *            CAN_Mode_Silent_LoopBack.
 *
 * @return  none
 */
void CAN_Mode_Init(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, uint8_t mode)
{
    GPIO_InitTypeDef      GPIO_InitStructure = {0};
    CAN_InitTypeDef       CAN_InitStructure = {0};
    CAN_FilterInitTypeDef CAN_FilterInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    //GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE); //PB8,PB9

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = ENABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = mode;
    CAN_InitStructure.CAN_SJW = tsjw;
    CAN_InitStructure.CAN_BS1 = tbs1;
    CAN_InitStructure.CAN_BS2 = tbs2;
    CAN_InitStructure.CAN_Prescaler = brp;
    CAN_Init(CAN1, &CAN_InitStructure);

    CAN_FilterInitStructure.CAN_FilterNumber = 0;

    #if(frame_format == STANDARD_FRAME)
    /* identifier/mask mode, One 32-bit filter, StdId: 0x317 */
    /*CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x62E0;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFE0;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0006;*/

    /* identifier/mask mode, Two 16-bit filters, StdId: 0x317,0x316 */
    //	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    //	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
    //	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x62E0;
    //	CAN_FilterInitStructure.CAN_FilterIdLow = 0xFFF8;
    //	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x62C0;
    //	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFF8;

    /* identifier list mode, One 32-bit filter, StdId: 0xC5C0000,0xC580000 */
    //	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
    //	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    //	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x62E0;
    //	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
    //	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x62C0;
    //	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;

    /* identifier list mode, Two 16-bit filters, StdId: 0x317,0x316,0x315,0x314 */
    //	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
    //	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
    //	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x62E0;
    //	CAN_FilterInitStructure.CAN_FilterIdLow = 0x62C0;
    //	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x62A0;
    //	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x6280;

    #elif(frame_format == EXTENDED_FRAME)
    /* identifier/mask mode, One 32-bit filter, ExtId: 0x12124567 */
    /*CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x9092;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x2B3C;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFE;*/

    #endif

    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    GPIO_InitStructure.GPIO_Pin = STBY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(STBY_PORT, &GPIO_InitStructure);
    GPIO_WriteBit(STBY_PORT, STBY_PIN, STBY_STATE_HIGH_SPEED); // Set STBY pin high to enable CAN transceiver
}


void CAN_init_freq(uint16_t kbps) //Frame mode does not restrict operation
{
    switch(kbps)
    {
        case 1000:
            CAN_Mode_Init(CAN_SJW_ALL, CAN_BRP_1000, CAN_BS2_1000, CAN_BS1_1000, STANDARD_FRAME);
        break;
        case 500:

        break;
        case 250:

        break;
        case 100:

        break;
    }
}

/*********************************************************************
 * @fn      CAN_Send_Msg
 *
 * @brief   CAN Transmit function.
 *
 * @param   
            id - Message ID.
            msg - Transmit data buffer.
 *          len - Data length.
 *          frame_format - Frame format (STANDARD_FRAME or EXTENDED_FRAME).
 *
 * @return  0 - Send successful.
 *          1 - Send failed.
 */
uint8_t CAN_Send_Msg(uint32_t id, uint8_t *msg, uint8_t len, uint8_t frame_format)
{
    uint8_t mbox;
    uint16_t i = 0;

    CanTxMsg CanTxStructure;

    #if(frame_format == STANDARD_FRAME)
        CanTxStructure.StdId = id;
        CanTxStructure.IDE = CAN_Id_Standard;

    #elif(frame_format == EXTENDED_FRAME)
        CanTxStructure.ExtId = id;
        CanTxStructure.IDE = CAN_Id_Extended;

    #endif

    CanTxStructure.RTR = CAN_RTR_Data;
    CanTxStructure.DLC = len;

    for(i = 0; i < len; i++) 
    {
        CanTxStructure.Data[i] = msg[i];
    }

    mbox = CAN_Transmit(CAN1, &CanTxStructure);
    i = 0;

    while((CAN_TransmitStatus(CAN1, mbox) != CAN_TxStatus_Ok) && (i < 0xFFF))
    {
        i++;
    }

    if(i == 0xFFF)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* CAN Rx message structure definition for reference
typedef struct
{
    uint32_t StdId; /* Specifies the standard identifier.
                       This parameter can be a value between 0 to 0x7FF. 

    uint32_t ExtId; /* Specifies the extended identifier.
                       This parameter can be a value between 0 to 0x1FFFFFFF. 

    uint8_t IDE; /* Specifies the type of identifier for the message that
                    will be received. This parameter can be a value of
                    @ref CAN_identifier_type 

    uint8_t RTR; /* Specifies the type of frame for the received message.
                    This parameter can be a value of
                    @ref CAN_remote_transmission_request 

    uint8_t DLC; /* Specifies the length of the frame that will be received.
                    This parameter can be a value between 0 to 8 

    uint8_t Data[8]; /* Contains the data to be received. It ranges from 0 to
                        0xFF. 

    uint8_t FMI; /* Specifies the index of the filter the message stored in
                    the mailbox passes through. This parameter can be a
                    value between 0 to 0xFF 
} CanRxMsg;
*/
CanRxMsg CAN_Receive_Msg(void)
{
    CanRxMsg CanRxStructure = {0};

    if(CAN_MessagePending(CAN1, CAN_FIFO0) == 0)
    {
        return CanRxStructure;
    }

    CAN_Receive(CAN1, CAN_FIFO0, &CanRxStructure);

    return CanRxStructure;
}
