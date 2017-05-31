#ifndef __BMM__H
#define __BMM__H

#include <stdint.h>

/*     READ only        */
#define BMM_CHIP_ID                      (0x40)
#define BMM_DATA_X_LSB                   (0x42)
#define BMM_DATA_X_MSB                   (0x43)
#define BMM_DATA_Y_LSB                   (0x44)
#define BMM_DATA_Y_MSB                   (0x45)
#define BMM_DATA_Z_LSB                   (0x46)
#define BMM_DATA_Z_MSB                   (0x47)
#define BMM_DATA_R_LSB                   (0x48)
#define BMM_DATA_R_MSB                   (0x49)
#define BMM_INT_STAT_REG                 (0x4A)

/*   Control Registers  */
#define BMM_POWER_CTRL                   (0x4B)
#define BMM_CTRL                         (0x4C)
#define BMM_INT_CTRL                     (0x4D)
#define BMM_SENS_CTRL                    (0x4E)
#define BMM_LOW_THRES                    (0x4F)
#define BMM_HIGH_THRES                   (0x50)
#define BMM_REP_XY                       (0x51)
#define BMM_REP_Z                        (0x52)

int BMM150_Init(ARM_DRIVER_SPI* SPIx);
int BMM150_read(uint8_t reg);
void BMM150_write(uint8_t reg, uint8_t value);
void BMM150_multi_read (uint8_t reg, uint8_t num, uint8_t* pdata);
void BMM150_multi_write(uint8_t reg, uint8_t num, uint8_t* pdata);

#endif
