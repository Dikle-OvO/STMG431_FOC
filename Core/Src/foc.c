//
// Created by huangjungang on 2025/11/10.
//

#include "../Inc/foc.h"

#include "main.h"
#include "math.h"
#include "FreeRTOS.h" // 用于 FreeRTOS API
#include "task.h"     // 用于 xTaskGetTickCount
#include "stdio.h"
#include "usart.h"
#include "AS5600.h"


//初始变量及函数定义
#define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

float voltage_power_supply=12.6f;
float shaft_angle=0, Ualpha=0, Ubeta=0, Ua=0, Ub=0, Uc=0, dc_a=0, dc_b=0, dc_c=0;
float zero_electric_angle=0;
// 使用 FreeRTOS 的 Tick 计数器
// TickType_t 是 FreeRTOS 中标准的时间类型 (通常是 uint32_t)
TickType_t open_loop_timestamp = 0;

// 电角度求解
float _electricalAngle(float shaft_angle, int pole_pairs) {
  return (shaft_angle * pole_pairs);
}

// 归一化角度到 [0,2PI]
float _normalizeAngle(float angle){
  float a = fmod(angle, 2*PI);   //取余运算可以用于归一化，列出特殊值例子算便知
  return a >= 0 ? a : (a + 2*PI);
}


// 设置PWM到控制器输出
void setPwm(float Ua, float Ub, float Uc) {

  // 计算占空比
  // 限制占空比从0到1
  dc_a = _constrain(Ua / voltage_power_supply, 0.0f , 1.0f );
  dc_b = _constrain(Ub / voltage_power_supply, 0.0f , 1.0f );
  dc_c = _constrain(Uc / voltage_power_supply, 0.0f , 1.0f );

  //写入PWM到PWM 0 1 2 通道
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (int)(dc_a*8500));
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (int)(dc_b*8500));
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, (int)(dc_c*8500));
  static char tx_buffer[64];
  // int foc_strlen = sprintf(tx_buffer, "%0.2f,%0.2f,%0.2f\r\n", dc_a, dc_b, dc_c);
  int foc_strlen = sprintf(tx_buffer, "%d,%d,%d\r\n", (int)(dc_a*8500), (int)(dc_b*8500), (int)(dc_c*8500));

  HAL_UART_Transmit_DMA(&huart1, tx_buffer, foc_strlen);
  // HAL_UART_Transmit_DMA(&huart1,"Hi DMA\r\n",sizeof("Hi DMA\r\n"));
}

void setPhaseVoltage(float Uq,float Ud, float angle_el) {
  angle_el = _normalizeAngle(angle_el + zero_electric_angle);
  // 帕克逆变换
  Ualpha =  -Uq*sin(angle_el);
  Ubeta =   Uq*cos(angle_el);

  // 克拉克逆变换
  Ua = Ualpha + voltage_power_supply/2;
  Ub = (sqrt(3)*Ubeta-Ualpha)/2 + voltage_power_supply/2;
  Uc = (-Ualpha-sqrt(3)*Ubeta)/2 + voltage_power_supply/2;

  setPwm(Ua,Ub,Uc);
}


//开环速度函数test
float velocityOpenloop(float target_velocity){
  // *** MODIFIED ***
  // 获取从 FreeRTOS 调度器启动以来的毫秒数 (Tick 数)
  TickType_t now_ms = xTaskGetTickCount();

  // 计算当前每个Loop的运行时间间隔
  // (now_ms - open_loop_timestamp) 是 Tick 差值
  // portTICK_PERIOD_MS 是 1 个 Tick 代表的毫秒数 (通常是 1ms)
  // 结果 Ts 单位是秒 (s)
  float Ts = (float)(now_ms - open_loop_timestamp) * (float)portTICK_PERIOD_MS * 1e-3f;

  // 修正时间间隔，防止启动或溢出时异常
  // (原逻辑: 如果 Ts <= 0 或 > 0.5s, 则默认为 1ms)
  if(Ts <= 0.0f || Ts > 0.5f) Ts = 1e-3f; // 1ms

  // 计算机械角度
  shaft_angle = _normalizeAngle(shaft_angle + target_velocity*Ts);

  // 设置 Uq, Ud=0
  float Uq = voltage_power_supply / 3.0f;

  // 假设极对数是 7
  setPhaseVoltage(Uq,0, _electricalAngle(shaft_angle, 7));

  open_loop_timestamp = now_ms;// 更新时间戳
  return Uq;
}

extern float AngleDegrees;
extern float AngleRadians;
int PP=7,DIR=-1;
// float CalElectricalAngle() {
//   float el_angle = (float)DIR * AngleRadians * (float)PP - zero_electric_angle;
//
//   return _normalizeAngle(el_angle);
// }

void speed_open(void) {
  while (1) {
    AS5600_Read(); // 获取当前 AngleRadians

    // 1. 强制给一个恒定的 2V 力矩 (Uq = 2.0, Ud = 0)
    // 2. 使用包含 DIR 的电角度公式
    float el_angle = (float)(DIR * 7) * AngleRadians - zero_electric_angle;

    setPhaseVoltage(3.0f, 0, _normalizeAngle(el_angle));
  }
}

void FOC_test(void) {
    // velocityOpenloop(10);
    speed_open();

  // 闭环位置
  // static float Kp = 0.033;
  // AS5600_Read();
  // float target_angle = 70.0f;
  // float current_angle =AngleDegrees;
  // float motor_err = target_angle - current_angle;
  // float Uq = Kp * motor_err;
  //
  // Uq = _constrain(Uq, -6.0f, 6.0f);
  // setPhaseVoltage(Uq, 0, CalElectricalAngle());

  // static char tx_buffer[24];
  // int as5600_strlen = sprintf(tx_buffer, "%d,%d\r\n", (int)(Uq), (int)(motor_err));
  // HAL_UART_Transmit_DMA(&huart1, tx_buffer, as5600_strlen);
}