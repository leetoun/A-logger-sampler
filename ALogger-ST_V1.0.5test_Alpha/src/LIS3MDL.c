#include <string.h>
#include "osObjects.h"
#include "Driver_SPI.h"
#include "rtt_log.h"
#include "stm32f4xx_hal.h"
#include "LIS3MDL.h"
#include "LIS3MDL_reg.h"

#define SEND_DONE     (1<<0)

#define READ          (0x80)
#define WRITE         (0x00)
#define MULTI         (0x40)

#define LIS_INT_PORT       GPIOC
#define LIS_INT_PIN        GPIO_PIN_6

#define LIS_DRDY_PORT      GPIOC
#define LIS_DRDY_PIN       GPIO_PIN_7

#define LIS_CS_PORT        GPIOB
#define LIS_CS_PIN         GPIO_PIN_9

#define BUS_A()            BUS_ACQUIRE(spi2)
#define BUS_R()            BUS_RELEASE(spi2)

#define BUS_ACQUIRE(x)     osMutexWait(mutex_##x, osWaitForever)
#define BUS_RELEASE(x)     osMutexRelease(mutex_##x)

#define LIS_CS_L()                                                       \
do {                                                                     \
	BUS_A();                                                             \
	tid_spi2x = osThreadGetId();                                         \
	LIS_CS_PORT->BSRR = (uint32_t)LIS_CS_PIN << 16;                      \
} while(0)
				
#define LIS_CS_H()                                                       \
do {                                                                     \
	LIS_CS_PORT->BSRR = LIS_CS_PIN;                                      \
	BUS_R();                                                             \
} while(0)


#define _ENABLE_PORT_CLK(x) __HAL_RCC_##x##_CLK_ENABLE()

static ARM_DRIVER_SPI* SPIdrv = NULL;

int LIS3MDL_read(uint8_t reg)	{
	uint8_t data_in[2] = {0};
	reg |= READ;
	
	LIS_CS_L();
	SPIdrv->Transfer(&reg, data_in, 2);
	osSignalWait(SEND_DONE, osWaitForever);
	LIS_CS_H();
	
	return data_in[1];
}

void LIS3MDL_multi_read (uint8_t reg, uint8_t num, uint8_t* pdata)	{
	uint8_t data_in[64];
	reg |= MULTI|READ;
	
	if ( num > 63 )	{
		Log("SPI Multi-Read overflow.\n");
		return;
	}
		
	LIS_CS_L();
	SPIdrv->Transfer(&reg, data_in, num + 1);
	osSignalWait(SEND_DONE, osWaitForever);
	LIS_CS_H();
	
	memcpy(pdata, data_in + 1, num);
}

void LIS3MDL_write(uint8_t reg, uint8_t value)	{
	uint8_t data_out[2] = {reg, value};
	uint8_t data_in[2];
	
	LIS_CS_L();
	SPIdrv->Transfer(data_out, data_in, 2);
	osSignalWait(SEND_DONE, osWaitForever);
	LIS_CS_H();
}

int LIS3MDL_Init(ARM_DRIVER_SPI* SPIx) {
	
	GPIO_InitTypeDef InitStruct;

	if ( SPIx == NULL ) {
		Log("Error : No SPI used by LIS3MDL.\n ");
		return -1;
	} else {
		SPIdrv = SPIx;
	}
	
	/* Init CS pin     */
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	InitStruct.Pin   = LIS_CS_PIN;
	InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;	//2017-05-16
	InitStruct.Pull  = GPIO_NOPULL;		//2017-05-16
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LIS_CS_PORT, &InitStruct);
	HAL_GPIO_WritePin(LIS_CS_PORT, LIS_CS_PIN, GPIO_PIN_SET);
	
	/* Init INT pin     */
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	InitStruct.Pin   = LIS_DRDY_PIN;
	InitStruct.Mode  = GPIO_MODE_IT_RISING;
	InitStruct.Pull  = GPIO_PULLDOWN;
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LIS_DRDY_PORT, &InitStruct);

	
	if ( LIS3MDL_read(LIS3MDL_WHO_AM_I) != 0x3D) {
		Log(RTT_TEXT_RED"No LIS3MDL found.\n");
		osThreadTerminate(osThreadGetId());
	}
	
	LIS3MDL_write(LIS3MDL_CTRL_REG2, SOFT_RST);
	
	osDelay(100);
	
	LIS3MDL_write(LIS3MDL_CTRL_REG1, ODR_40);
	LIS3MDL_write(LIS3MDL_CTRL_REG2, FS_12G);
	
	osDelay(20);
	
	LIS3MDL_write(LIS3MDL_CTRL_REG3, STOP_MODE);
//	LIS3MDL_write(LIS3MDL_CTRL_REG4, 0);
//	LIS3MDL_write(LIS3MDL_CTRL_REG5, 0);
	
	for ( int i = 0x20; i <= 0x24; i++)
		Log("LIS3MDL REG %02X = 0x%02X\n", i, LIS3MDL_read(i));
	
	for ( int i = 0x27; i <= 0x2D; i++)
		Log("LIS3MDL REG %02X = 0x%02X\n", i, LIS3MDL_read(i));
	
	return 0;
}
