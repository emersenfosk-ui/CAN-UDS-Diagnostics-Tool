#include "uds.h"

/* 发送 UDS 响应到 0x7E8，经典 CAN 2.0，波特率/采样点保持你现有配置 */
static void UDS_SendResponse(uint8_t *data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader = {0};
    TxHeader.Identifier = 0x7E8;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
		
		/* ↓↓↓ 改成用 FDCAN_DLC_BYTES_x 宏 ↓↓↓ */
    switch(len)
    {
        case 1: TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2: TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3: TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4: TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5: TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6: TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7: TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8: TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
    }
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;   // 经典 CAN 2.0
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, data);
}

void UDS_HandleRequest(uint8_t *data, uint8_t len)
{
    uint8_t sid = data[1];   // data[0] 是 ISO-TP 单帧长度字节

    switch (sid)
    {
        case 0x10:   // 会话控制
        {
            uint8_t resp[6] = {0x05, 0x50, 0x10, data[2], 0x00, 0x32};
            UDS_SendResponse(resp, 6);
            break;
        }

        case 0x22:   // 读数据标识符
        {
            uint16_t did = (data[2] << 8) | data[3];
            if (did == 0xF190)  // VIN
            {
                uint8_t resp[8] = {0x07, 0x62, 0xF1, 0x90, 'L', 'X', 'B', '7'};
                UDS_SendResponse(resp, 8);
            }
            else
            {
                uint8_t resp[3] = {0x02, 0x7F, 0x22};
                UDS_SendResponse(resp, 3);
            }
            break;
        }

        case 0x19:   // 读 DTC
        {
            uint8_t resp[3] = {0x02, 0x59, 0x02};
            UDS_SendResponse(resp, 3);
            break;
        }

        case 0x14:   // 清除 DTC
        {
            uint8_t resp[2] = {0x01, 0x54};  // ✅ 0x01 = 后面1字节有效数据
            UDS_SendResponse(resp, 2);
            break;
        }

        case 0x3E:   // Tester Present
        {
            uint8_t resp[3] = {0x02, 0x7E, 0x00};
            UDS_SendResponse(resp, 2);
            break;
        }

        default:     // 不支持的服务
        {
            uint8_t resp[3] = {0x02, 0x7F, sid};
            UDS_SendResponse(resp, 3);
            break;
        }
    }
}
