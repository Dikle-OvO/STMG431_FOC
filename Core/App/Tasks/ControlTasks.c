//
// Created by huangjungang on 2025/12/30.
//
#include <string.h>

#include "main.h"
#include "cmsis_os2.h"
#include "usart.h"

void CommandStartTask(void *argument) {
    UART1_Receive_Start();
    uint8_t receive;
    uint8_t command[30]={0};
    uint8_t commandIndex=0;
    uint8_t commandLength=0;
    for (;;) {
        osMessageQueueGet(CommandQueueHandle,&receive,0,osWaitForever);
        if (receive== 'a'){
        }
    }
}