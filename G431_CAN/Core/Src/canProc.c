#include "system.h"

FDCAN_TxHeaderTypeDef   g_TxHeader;
uint8_t g_TxData[8];
uint32_t g_TxMailbox;


void FDCAN_Config()
{
    FDCAN_FilterTypeDef sFilterConfig;

    /* Configure Rx filter */
    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex = 0;
    sFilterConfig.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig.FilterID1 = 0x321;
    sFilterConfig.FilterID2 = 0x7FF;
    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
    {
      Error_Handler();
    }

    /* Configure global filter:
       Filter all remote frames with STD and EXT ID
       Reject non matching frames with STD ID and EXT ID */
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
    {
      Error_Handler();
    }

    /* Start the FDCAN module */
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
    {
      Error_Handler();
    }
}


void firstMessageSend()
{
    g_TxHeader.Identifier = 0x321;
    g_TxHeader.IdType = FDCAN_STANDARD_ID;
    g_TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    g_TxHeader.DataLength = FDCAN_DLC_BYTES_2;
    g_TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    g_TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    g_TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    g_TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    g_TxHeader.MessageMarker = 0;

    g_TxData[0] = 50;  
    g_TxData[1] = 0xAA;

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &g_TxHeader, g_TxData) != HAL_OK)
    {
        /* Transmission request Error */
        Error_Handler();
    }
}