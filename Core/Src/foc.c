//
// Created by huangjungang on 2025/11/10.
//

#include "../Inc/foc.h"

#include "AS5600.h"
#include "main.h"
#include "math.h"
#include "FreeRTOS.h" // 用于 FreeRTOS API
#include "task.h"     // 用于 xTaskGetTickCount
#include "stdio.h"
#include "usart.h"

extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
extern float AngleDegrees;

//初始变量及函数定义
#define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

float voltage_power_supply=12.6f;
float shaft_angle=0, Ualpha=0, Ubeta=0, Ua=0, Ub=0, Uc=0, dc_a=0, dc_b=0, dc_c=0;
float zero_electric_angle=0;
// 使用 FreeRTOS 的 Tick 计数器
// TickType_t 是 FreeRTOS 中标准的时间类型 (通常是 uint32_t)
TickType_t open_loop_timestamp = 0;

// 电角度求解(开环)
float _electricalAngle(float shaft_angle, int pole_pairs) {
  return (shaft_angle * pole_pairs);
}

// 归一化角度到 [0,2PI]
float _normalizeAngle(float angle){
  float a = fmod(angle, 2*PI);   //取余运算可以用于归一化，列出特殊值例子算便知
  return a >= 0 ? a : (a + 2*PI);
}

int PP=7,DIR=1;
float CalElectricalAngle() {
  // 1. 将 0-360 度转换为 0-2PI 弧度
  float shaft_angle_rad = (DIR * AngleDegrees) * (PI / 180.0f);

  // 2. 计算电角度弧度：机械弧度 * 极对数 - 偏移弧度
  float el_angle = shaft_angle_rad * (float)PP - zero_electric_angle;

  return _normalizeAngle(el_angle);
}

// 设置PWM到控制器输出
void setPwm(float Ua, float Ub, float Uc) {
  // 1. 计算占空比 (0.0 ~ 1.0)
  dc_a = _constrain(Ua / voltage_power_supply, 0.0f , 1.0f );
  dc_b = _constrain(Ub / voltage_power_supply, 0.0f , 1.0f );
  dc_c = _constrain(Uc / voltage_power_supply, 0.0f , 1.0f );

  // 2. 映射到 ARR 值 (8500)
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (int)(dc_a * 100));
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (int)(dc_b * 100));
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, (int)(dc_c * 100));
}

void setPhaseVoltage(float Uq, float Ud, float angle_el_rad) {
  // 强制使用 sinf/cosf (float版本) 提高在 G431 上的运算速度
  Ualpha = -Uq * sinf(angle_el_rad);
  Ubeta  =  Uq * cosf(angle_el_rad);

  Ua = Ualpha + voltage_power_supply/2.0f;
  Ub = (sqrtf(3.0f)*Ubeta - Ualpha)/2.0f + voltage_power_supply/2.0f;
  Uc = (-Ualpha - sqrtf(3.0f)*Ubeta)/2.0f + voltage_power_supply/2.0f;

  setPwm(Ua, Ub, Uc);
}


//开环速度函数test
float velocityOpenloop(float target_velocity){
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



void FOC_test(void) {
  // 开环速度
  velocityOpenloop(10);

  // 闭环位置
  // static float Kp = 0.133;
  //
  // float target_angle = 70.0f;
  // float current_angle = DIR * AngleDegrees;
  // float motor_err = target_angle - current_angle;
  // float Uq = Kp * motor_err;
  //
  // Uq = _constrain(Uq, -6.0f, 6.0f);
  // setPhaseVoltage(Uq, 0, CalElectricalAngle());
}