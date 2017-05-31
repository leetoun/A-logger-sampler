#ifndef __BMI__H
#define __BMI__H

#include <stdint.h>

int BMI160_read(uint8_t reg);
void BMI160_write(uint8_t reg, uint8_t value);
void BMI160_read_fifo (uint8_t num, fifo_t * pdata);
//void BMI_multi_read (uint8_t reg, uint8_t num, uint8_t* pdata);
//void BMI_multi_write(uint8_t reg, uint8_t num, uint8_t* pdata);
int16_t BMI160_get_fifo_len(void);
int BMI160_Init(ARM_DRIVER_SPI* SPIx);

#endif
