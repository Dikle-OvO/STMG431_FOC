//
// Created by huangjungang on 2025/11/10.
//

#ifndef FOC_TEST_FOC_H
#define FOC_TEST_FOC_H

#define PI 3.1415926f

float _normalizeAngle(float angle);
float CalElectricalAngle();
void setPhaseVoltage(float Uq,float Ud, float angle_el);
void FOC_test(void);

#endif //FOC_TEST_FOC_H
