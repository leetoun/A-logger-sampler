#include <string.h>
#include "osObjects.h"
#include "Driver_SPI.h"
#include "rtt_log.h"
#include "stm32f4xx_hal.h"
#include "ADXL362.h"

#define SEND_DONE      (1<<0)

#define WR_REG         (0x0A)
#define RD_REG         (0x0B)
#define RD_FIFO        (0x0D)

#ifdef  ST_DEMO
#define ADXL_INT_PORT       GPIOC
#define ADXL_INT_PIN        GPIO_PIN_4

#define ADXL_CS_PORT        GPIOA
#define ADXL_CS_PIN         GPIO_PIN_4

#else

#define ADXL_INT_PORT       GPIOC
#define ADXL_INT_PIN        GPIO_PIN_4

#define ADXL_CS_PORT        GPIOB
#define ADXL_CS_PIN         GPIO_PIN_0

#endif

#define BUS_A()  BUS_ACQUIRE(spi1)
#define BUS_R()  BUS_RELEASE(spi1)
				
#define BUS_ACQUIRE(x)  osMutexWait(mutex_##x, osWaitForever)
#define BUS_RELEASE(x)  osMutexRelease(mutex_##x)

#define ADXL_CS_L()                                                      \
do {                                                                     \
	BUS_A();                                                             \
	tid_spi1x = osThreadGetId();                                         \
	ADXL_CS_PORT->BSRR = (uint32_t)ADXL_CS_PIN << 16;                    \
} while(0)
				
#define ADXL_CS_H()                                                      \
do {                                                                     \
	ADXL_CS_PORT->BSRR = ADXL_CS_PIN;                                    \
	BUS_R();                                                             \
} while(0)

#define ENABLE_PORT(x) __HAL_RCC_##x##_CLK_ENABLE()

static ARM_DRIVER_SPI* SPIdrv = NULL;

void ADXL362_write(uint8_t reg, uint8_t value)	{
	uint8_t data_out[3] = {WR_REG, reg, value};
	uint8_t dummy[3];
	
	ADXL_CS_L();
	SPIdrv->Transfer(data_out, dummy, 3);
	osSignalWait(SEND_DONE, osWaitForever);
	ADXL_CS_H();
}

uint8_t ADXL362_read(uint8_t reg)	{
	uint8_t data_out[3] = {RD_REG, reg};
	uint8_t data_in[3];
	
	ADXL_CS_L();
	SPIdrv->Transfer(data_out, data_in, 3);
	osSignalWait(SEND_DONE, osWaitForever);
	ADXL_CS_H();
	
	return data_in[2];
}

void ADXL362_multi_write (uint8_t reg, uint8_t num, uint8_t* pdata)	{
	uint8_t data_out[16] = {WR_REG, reg};
	uint8_t dummy[16];
	
	for ( int i = 0; i < num; i++)
		data_out[2 + i] = pdata[i]; 
	
	ADXL_CS_L();
	SPIdrv->Transfer(data_out, dummy, 2 + num);
	osSignalWait(SEND_DONE, osWaitForever);
	ADXL_CS_H();
}

void ADXL362_multi_read (uint8_t reg, uint8_t num, uint8_t * pdata)	{
	uint8_t data_in[64];
	uint8_t data_out[2] = {RD_REG, reg};
	
	if ( num + sizeof(data_out) > 64 ) {
		Log(RTT_TEXT_YELLOW "ADXL multi read overflow: %d\n", num);
		return;
	}
	
	ADXL_CS_L();
	SPIdrv->Transfer(data_out, data_in, sizeof(data_out) + num );
	osSignalWait(SEND_DONE, osWaitForever);
	ADXL_CS_H();
	
	memcpy(pdata, data_in+2, num);
}

void ADXL362_read_fifo (uint8_t num, raw_3axis_block_t * pdata)	{
	uint8_t data_out = RD_FIFO;
	
	if ( num > 30 ) {
		Log(RTT_BG_YELLOW RTT_TEXT_BLACK"[%d] : ADXL read buffer overflow: %d\n", m_timestamp_per_100ms, num);
		num = 20;
	}
	
	ADXL_CS_L();
	SPIdrv->Transfer(&data_out, &pdata->num, sizeof(data_out) + num * sizeof(axis_t));
	osSignalWait(SEND_DONE, osWaitForever);
	ADXL_CS_H();
	
	pdata->num = num;
}

int ADXL362_Init(ARM_DRIVER_SPI* SPIx) {
	
	//                  REG  28    29      2A    2B    2C    2D  
	uint8_t data_out[16] = { 0x00, 50*3-1, 0x04, 0x00, 0x94, 0x20 };
	uint8_t data_in [16] = {0};
	
	GPIO_InitTypeDef InitStruct;

	if ( SPIx == NULL ) {
		return -1;
	} else {
		SPIdrv = SPIx;
	}
	
	/* Init ADXL CS pin     */
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	InitStruct.Pin   = ADXL_CS_PIN;
	InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
	InitStruct.Pull  = GPIO_PULLUP;
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ADXL_CS_PORT,&InitStruct);
	HAL_GPIO_WritePin(ADXL_CS_PORT,ADXL_CS_PIN,GPIO_PIN_SET);
	
	/* Init ADXL INT pin     */
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	InitStruct.Pin   = ADXL_INT_PIN;
	InitStruct.Mode  = GPIO_MODE_IT_RISING;
	InitStruct.Pull  = GPIO_PULLDOWN;
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ADXL_INT_PORT,&InitStruct);

	if ( ADXL362_read(ADXL_DEVID_AD) != 0xAD) {
		Log(RTT_TEXT_RED"No ADXL 362 found.\n");
		osThreadTerminate(osThreadGetId());
	}
	
	ADXL362_write(ADXL_SOFT_RESET, 'R');
	
	osDelay(50);

	ADXL362_multi_write(ADXL_FIFO_CONTROL, 6, data_out);
	
	ADXL362_multi_read (ADXL_FIFO_CONTROL, 6, data_in);
	for(int i = 0; i < 6; i++)
    Log("ADXL REG 0x%02X = 0x%02X\n", 0x28 + i, data_in[i]);
	
	return 0;
}

