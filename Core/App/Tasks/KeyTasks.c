//
// Created by huangjungang on 2025/12/30.
//

#include <sys/types.h>

#include "cmsis_os2.h"
#include "stdlib.h"
#include "Types/LEDTypes.h"
#include "main.h"
#include "FreeRTOS.h"

#define IS_KEY_PRESSED() (HAL_GPIO_ReadPin(Key_GPIO_Port, Key_Pin))

uint8_t isKeyClicked(void) {
    static uint8_t counter = 0;
    static uint8_t pressed = 0;
    if (IS_KEY_PRESSED() && !pressed) {
        counter++;
        if (counter >= 3) {
            pressed = 1;
            return 1;
        }
    }
    if (!IS_KEY_PRESSED()) {
        pressed = 0;
        counter = 0;
    }
    return 0;
}

void StartKeyTask(void *argument) {
    LEDState state = LEDState_off;
    for (;;) {
        if (isKeyClicked()) {
            state = !state;
            LEDMesg *msg = pvPortMalloc(sizeof(LEDMesg));
            msg->color = LEDColor_Blue;
            msg->state = state;
            osMessageQueuePut(LEDQueueHandle,&msg,0,osWaitForever);
        }

    }
}