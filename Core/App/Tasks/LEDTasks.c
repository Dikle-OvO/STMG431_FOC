#include "cmsis_os2.h"
#include "main.h"
#include "FreeRTOS.h"
#include "Types/LEDTypes.h"
//
// Created by huangjungang on 2025/12/30.
//

void StartLEDTask(void *argument) {
    // while (1) {
    //     for (int i=0;i<=8499;i++) {
    //       __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,i);
    //       osDelay(1);
    //     }
    //
    //     for (int i=8499;i>=0;i--) {
    //       __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,i);
    //       osDelay(1);
    //     }
    //     osDelay(10);
    // }

    for (;;) {
        LEDMesg *mesg;
        osMessageQueueGet(LEDQueueHandle,&mesg,0,osWaitForever);
        if (mesg == NULL) {
            return;
        }

        switch (mesg->color) {
            case LEDColor_Red:
                // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, mesg->state?GPIO_PIN_SET:GPIO_PIN_RESET);
                break;
            case LEDColor_Green:
                // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, mesg->state?GPIO_PIN_SET:GPIO_PIN_RESET);
                break;
            case LEDColor_Blue:
                // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, mesg->state?GPIO_PIN_RESET:GPIO_PIN_SET);
                break;
        }
        vPortFree(mesg);
    }
}