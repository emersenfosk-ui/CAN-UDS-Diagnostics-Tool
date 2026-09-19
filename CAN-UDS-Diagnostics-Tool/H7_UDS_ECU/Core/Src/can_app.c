#include "can_app.h"
#include <stdio.h>
#include <string.h>

// 软件FIFO
#define CAN_RX_FIFO_SIZE 16
typedef struct {
    uint32_t id;
    uint8_t data[64];
    uint8_t len;
} Can_RxMsg_t;

Can_RxMsg_t can_rx_fifo[CAN_RX_FIFO_SIZE];
volatile uint8_t fifo_head = 0;
volatile uint8_t fifo_tail = 0;

void APP_Can_PushMsg(uint32_t id, uint8_t *data, uint8_t len) {
    uint8_t next = (fifo_head + 1) % CAN_RX_FIFO_SIZE;
    if (next != fifo_tail) {
        can_rx_fifo[fifo_head].id = id;
        memcpy(can_rx_fifo[fifo_head].data, data, 8);
        can_rx_fifo[fifo_head].len = len;
        fifo_head = next;
    }
}

uint8_t APP_Can_PopMsg(Can_RxMsg_t *msg) {
    if (fifo_head == fifo_tail) return 0;
    *msg = can_rx_fifo[fifo_tail];
    fifo_tail = (fifo_tail + 1) % CAN_RX_FIFO_SIZE;
    return 1;
}

void APP_CAN_HandleRx(FDCAN_HandleTypeDef *hfdcan, uint32_t id, uint8_t *data, uint8_t len) {
    APP_Can_PushMsg(id, data, len);
}

void APP_CAN_Init(void) {
    printf("CAN APP Init OK\r\n");
}

// 每10ms调用一次，在main的while里轮询
void APP_CAN_Task_10ms(void) {
	 Can_RxMsg_t rx_msg;

    // 只处理接收打印，不做周期性发送
    if (APP_Can_PopMsg(&rx_msg)) {
        printf("Recv ID:0x%03X, Len:%d\r\n", rx_msg.id, rx_msg.len);
    }
	
//    static uint32_t send_counter = 0;
//    static uint32_t loopback_test_flag = 0;
//    Can_RxMsg_t rx_msg;

//    // 1. 先处理接收：从FIFO里取报文并打印
//    if (APP_Can_PopMsg(&rx_msg)) {
//        // 检查是否收到了自己发的数据
//        if (rx_msg.id == 0x123) {
//            printf("ELoopbackTest: Received!\r\n");
//            printf("    ID: 0x%03X, Data[0]=0x%02X, Data[1]=0x%02X\r\n", 
//                   rx_msg.id, rx_msg.data[0], rx_msg.data[1]);
//            loopback_test_flag = 1;
//        } else {
//            printf("Recv ID:0x%03X, Len:%d\r\n", rx_msg.id, rx_msg.len);
//        }
//    }

//    // 2. 每1秒发一帧（100次 * 10ms = 1秒）
//    send_counter++;
////    if (send_counter >= 100) {
////        send_counter = 0;

////        uint8_t tx_data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
////        FDCAN_TxHeaderTypeDef TxHeader = {0};
////        TxHeader.Identifier = 0x123;
////        TxHeader.IdType = FDCAN_STANDARD_ID;
////        TxHeader.TxFrameType = FDCAN_DATA_FRAME;
////        TxHeader.DataLength = FDCAN_DLC_BYTES_8;
////        TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
////        TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
////        TxHeader.FDFormat = FDCAN_FD_CAN;
////        TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
////        TxHeader.MessageMarker = 0;

////        if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, tx_data) == HAL_OK) {
//if (send_counter >= 100) {
//    send_counter = 0;

//    uint8_t tx_data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
//    FDCAN_TxHeaderTypeDef TxHeader = {0};
//    TxHeader.Identifier = 0x123;
//    TxHeader.IdType = FDCAN_STANDARD_ID;
//    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
//    TxHeader.DataLength = FDCAN_DLC_BYTES_8;
//    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
//    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;       // 经典 CAN 不用 BRS
//    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;         // 经典 CAN 帧格式
//    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
//    TxHeader.MessageMarker = 0;

//    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, tx_data) == HAL_OK) {           
//		printf("Send OK\r\n");
//        } else {
//            printf("Send FAILED\r\n");
//        }
//    }
}