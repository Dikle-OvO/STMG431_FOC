//
// Created by huangjungang on 2025/12/30.
//
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"
#include "foc.h"
#include "AS5600.h"

#define _3PI_2 4.71238898038f
extern float zero_electric_angle;
void FOCStartTask(void *argument) {

    // setPhaseVoltage(3, 0.0f, 4.712389f);
    // vTaskDelay(2000);
    // zero_electric_angle=CalElectricalAngle();
    // setPhaseVoltage(0, 0, 4.712389f);

    while (1) {
        // AS5600_Read();
        FOC_test();
        osDelay(2);
    }
}