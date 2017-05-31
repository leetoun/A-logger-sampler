#ifndef __ADXL__H
#define __ADXL__H

#include <stdint.h>

#define ADXL_DEVID_AD			    0x00
#define ADXL_DEVID_MST			  0x01
#define ADXL_PARTID			      0x02
#define ADXL_REVID			  	  0x03
#define ADXL_XDATA				    0x08
#define ADXL_YDATA				    0x09
#define ADXL_ZDATA				    0x0A
#define ADXL_STATUS			      0x0B
#define ADXL_FIFO_ENTRIES_L  	0x0C
#define ADXL_FIFO_ENTRIES_H	  0x0D
#define ADXL_XDATA_L			    0x0E
#define ADXL_XDATA_H			    0x0F
#define ADXL_YDATA_L			    0x10
#define ADXL_YDATA_H			    0x11
#define ADXL_ZDATA_L			    0x12
#define ADXL_ZDATA_H			    0x13
#define ADXL_TEMP_L			      0x14
#define ADXL_TEMP_H			      0x15
#define ADXL_SOFT_RESET		    0x1F
#define ADXL_THRESH_ACT_L	  	0x20
#define ADXL_THRESH_ACT_H			0x21
#define ADXL_TIME_ACT			  	0x22
#define ADXL_THRESH_INACT_L		0x23
#define ADXL_THRESH_INACT_H		0x24
#define ADXL_TIME_INACT_L			0x25
#define ADXL_TIME_INACT_H			0x26
#define ADXL_ACT_INACT_CTL		0x27
#define ADXL_FIFO_CONTROL			0x28
#define ADXL_FIFO_SAMPLES			0x29
#define ADXL_INTMAP1			    0x2A
#define ADXL_INTMAP2			    0x2B
#define ADXL_FILTER_CTL		  	0x2C
#define ADXL_POWER_CTL			  0x2D
#define ADXL_SELF_TEST			  0x2E

uint8_t ADXL362_read(uint8_t reg);
void ADXL362_write(uint8_t reg, uint8_t value);
void ADXL362_multi_read (uint8_t reg, uint8_t num, uint8_t* pdata);
void ADXL362_multi_write(uint8_t reg, uint8_t num, uint8_t* pdata);

void ADXL362_read_fifo (uint8_t num, raw_3axis_block_t * pdata);
int ADXL362_Init(ARM_DRIVER_SPI* SPIx);

#endif
