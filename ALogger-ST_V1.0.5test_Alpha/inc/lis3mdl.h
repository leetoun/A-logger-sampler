#ifndef __LIS3MDL_H_
#define __LIS3MDL_H_


#define  LIS3MDL_WHO_AM_I                    ((uint8_t)0x0F)            /*!< CONTROL REGISTER 1 */
#define  LIS3MDL_CTRL_REG1                   ((uint8_t)0x20)            /*!< CONTROL REGISTER 1 */
#define  LIS3MDL_CTRL_REG2                   ((uint8_t)0x21)            /*!< CONTROL REGISTER 2 */
#define  LIS3MDL_CTRL_REG3                   ((uint8_t)0x22)            /*!< CONTROL REGISTER 3 */
#define  LIS3MDL_CTRL_REG4                   ((uint8_t)0x23)            /*!< CONTROL REGISTER 4 */
#define  LIS3MDL_CTRL_REG5                   ((uint8_t)0x24)            /*!< CONTROL REGISTER 5 */
#define  LIS3MDL_STATUS_REG                  ((uint8_t)0x27)            /*!< STATUS REGISTER */
#define  LIS3MDL_OUT_X_L                     ((uint8_t)0x28)            /*!< OUTPUT X-AXIS LOW */
#define  LIS3MDL_OUT_X_H                     ((uint8_t)0x29)            /*!< OUTPUT X-AXIS HIGH */
#define  LIS3MDL_OUT_Y_L                     ((uint8_t)0x2A)            /*!< OUTPUT Y-AXIS LOW */
#define  LIS3MDL_OUT_Y_H                     ((uint8_t)0x2B)            /*!< OUTPUT Y-AXIS HIGH */
#define  LIS3MDL_OUT_Z_L                     ((uint8_t)0x2C)            /*!< OUTPUT Z-AXIS LOW */
#define  LIS3MDL_OUT_Z_H                     ((uint8_t)0x2D)            /*!< OUTPUT Z-AXIS HIGH */
#define  LIS3MDL_INT_CFG                     ((uint8_t)0x30)            /*!< INTERRUPT1 CONFIG */
#define  LIS3MDL_INT_SRC                     ((uint8_t)0x31)            /*!< INTERRUPT1 SOURCE */
#define  LIS3MDL_INT_THS_L                   ((uint8_t)0x32)            /*!< INTERRUPT1 THRESHOLD */
#define  LIS3MDL_INT_THS_H                   ((uint8_t)0x33)            /*!< INTERRUPT1 DURATION */

int LIS3MDL_read(uint8_t reg);
void LIS3MDL_multi_read (uint8_t reg, uint8_t num, uint8_t* pdata);
void LIS3MDL_write(uint8_t reg, uint8_t value);
int LIS3MDL_Init(ARM_DRIVER_SPI* SPIx);

#endif /* _LIS3MDL_H_ */
