//
// Created by huangjungang on 2025/11/11.
//

#ifndef FOC_TEST_AS5600_H
#define FOC_TEST_AS5600_H

// 定义 AS5600 的 I2C 地址和寄存器地址
#define AS5600_DEVICE_ADDRESS   0x36   // 7-bit I2C address
#define AS5600_ANGLE_REGISTER   0x0E   // 角度寄存器 (高位)

void AS5600_test(void);

#endif //FOC_TEST_AS5600_H