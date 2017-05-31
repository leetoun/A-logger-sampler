#ifndef __RTC_H
#define __RTC_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

extern RTC_HandleTypeDef RTCHandle;

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void RTC_Init(void);
void RTC_CalendarConfig(void);
void RTC_CalendarShow(char* showtime, char* showdate);

#endif
