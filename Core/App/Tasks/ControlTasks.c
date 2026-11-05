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
        if (commandIndex= 0){
            if (receive == OxAA){
            command[commandIndex++]=receive;//命令索引加1，代表已经找到包头
            } else if (commandIndex=1) {
                //如果命令索引为1，则receive为命令长度
                if (receive <4 || receive >sizeof(command)) {
                    commandIndex=0;//重置命令索引
                    continue; //命令长度不合法，重置命令索引，继续寻找包头
                }
                commandLength =receive;
                command[commandIndex++]=receive;//命令索引加1，代表已经找到命令长度
            }
        }
    }
}