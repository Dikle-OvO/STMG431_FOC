//
// Created by huangjungang on 2025/12/30.
//

#ifndef FOC_TEST_LEDTYPES_H
#define FOC_TEST_LEDTYPES_H

typedef enum {
    LEDColor_Red = 0,
    LEDColor_Blue = 1,
    LEDColor_Green = 2,
  } LEDColor;

typedef enum {
    LEDState_off = 0,
    LEDState_on = 1,
  } LEDState;

typedef struct {
    LEDColor color;
    LEDState state;
} LEDMesg;

#endif //FOC_TEST_LEDTYPES_H