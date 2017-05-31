#ifndef __ADPD__H
#define __ADPD__H

#include <stdint.h>
#include "Driver_I2C.h"

#define ADPD174_ADDRESS     0x64
#define CAIL                0x21
#define FSAMPLE             800
#define AVG_A               4
#define AVG_B               4
#define PULSE_A             32
#define PULSE_B             32

#define HRV_FIFO_NUM        64


uint16_t ADPD174_read (uint8_t reg );
void ADPD174_write(uint8_t reg, uint16_t value);
void ADPD174_multi_read (uint8_t reg, uint8_t num, uint16_t* pdata);
void ADPD174_multi_write(uint8_t reg, uint8_t num, uint16_t* pdata);
void ADPD174_read_fifo(uint8_t num, uint16_t* pdata);
void ADPD174_clear_fifo(void);
int ADPD174_Init(ARM_DRIVER_I2C* I2Cx);

#endif
