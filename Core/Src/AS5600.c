//
// Created by huangjungang on 2025/11/11.
//

#include "../Inc/AS5600.h"
#include "main.h"
#include "stdio.h"
#include "math.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;

float AngleDegrees = 0.0f;
float AngleRadians = 0.0f;

float AS5600_Read(void)
{
        uint8_t i2c_rx_buffer[2];
        uint16_t raw_angle = 0;

        HAL_StatusTypeDef read_status;

        // 1. 调用 HAL_I2C_Mem_Read
        // 这个函数会自动：
        // (1) 发送 Start + 设备地址(写)
        // (2) 发送要读取的寄存器地址 (0x0E)
        // (3) 发送 Restart + 设备地址(读)
        // (4) 连续读取2个字节 (0x0E 和 0x0F)
        // (5) 发送 Stop

        read_status = HAL_I2C_Mem_Read(
            &hi2c1,                      // I2C 句柄
            (AS5600_DEVICE_ADDRESS << 1), // 设备地址 (左移一位)
            AS5600_ANGLE_REGISTER,       // 要读取的寄存器起始地址
            I2C_MEMADD_SIZE_8BIT,        // 寄存器地址是8位的
            i2c_rx_buffer,               // 存放读取数据的缓冲区
            2,                           // 要读取的数据长度 (2字节)
            100                          // 超时时间 (ms)
        );

        // 2. 检查通信状态
        if (read_status == HAL_OK)
        {
            // 3. 组合数据
            // i2c_rx_buffer[0] 存放的是 0x0E 寄存器 (高位)
            // i2c_rx_buffer[1] 存放的是 0x0F 寄存器 (低位)
            raw_angle = ((uint16_t)i2c_rx_buffer[0] << 8) | i2c_rx_buffer[1];

            // AS5600 的输出是 12-bit 的 (0 - 4095)
            raw_angle &= 0x0FFF; // 确保是12位 (虽然读出来就是12位，做个掩码更安全)

            // 4. 转换为角度 弧度
            AngleDegrees = (float)raw_angle * (360.0f / 4096.0f);
            AngleRadians = AngleDegrees * (M_PI / 180.0f);

            static char tx_buffer[24];
            // int as5600_strlen = sprintf(tx_buffer, "%d,%d\r\n", (int)(raw_angle), (int)(AngleDegrees));
            // HAL_UART_Transmit_DMA(&huart1, tx_buffer, as5600_strlen);

            return AngleDegrees;
        }
        else
        {
            // I2C 读取失败
            // printf("I2C Read Error!\n");
            return 0.0f;
        }

}