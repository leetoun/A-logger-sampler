#include <string.h>
#include "osObjects.h"
#include "Driver_SPI.h"
#include "rtt_log.h"
#include "stm32f4xx_hal.h"
#include "BMI160.h"
#include "BMI160_reg.h"
#include "GUI.h"

#define SEND_DONE     (1<<0)

#define READ          (0x80)
#define WRITE         (0x00)

#ifdef  ST_DEMO

#define BMI_INT_PORT       GPIOC
#define BMI_INT_PIN        GPIO_PIN_4

#define BMI_CS_PORT        GPIOA
#define BMI_CS_PIN         GPIO_PIN_4

#else

#define BMI_INT1_PORT      GPIOA
#define BMI_INT1_PIN       GPIO_PIN_11

#define BMI_INT2_PORT      GPIOA
#define BMI_INT2_PIN       GPIO_PIN_12

#define BMI_CS_PORT        GPIOA
#define BMI_CS_PIN         GPIO_PIN_4
#endif

#define BUS_A()  BUS_ACQUIRE(spi1)
#define BUS_R()  BUS_RELEASE(spi1)
				
#define BUS_ACQUIRE(x)  osMutexWait(mutex_##x, osWaitForever)
#define BUS_RELEASE(x)  osMutexRelease(mutex_##x)

#define BMI_CS_L()                                                             \
do {                                                                           \
	BUS_A();                                                                   \
	tid_spi1x = osThreadGetId();                                               \
	BMI_CS_PORT->BSRR = (uint32_t)BMI_CS_PIN << 16;                            \
} while(0)
				
#define BMI_CS_H()                                                             \
do {                                                                           \
	BMI_CS_PORT->BSRR = BMI_CS_PIN;                                            \
	BUS_R();                                                                   \
} while(0)

static ARM_DRIVER_SPI* SPIdrv = NULL;

int BMI160_read(uint8_t reg)	{
	uint8_t data_in[2] = {0};
	reg |= READ;

	BMI_CS_L();
	SPIdrv->Transfer(&reg, data_in, 2);
	osSignalWait(SEND_DONE, osWaitForever);
	BMI_CS_H();
	
	return data_in[1];
}

void BMI160_write(uint8_t reg, uint8_t value)	{
	uint8_t data_out[2] = {reg, value};
	uint8_t data_in[2];
	
	BMI_CS_L();
	SPIdrv->Transfer(data_out, data_in, 2);
	osSignalWait(SEND_DONE, osWaitForever);
	BMI_CS_H();
}

void BMI160_multi_read (uint8_t reg, uint8_t num, uint8_t* pdata)	{
	uint8_t data_in[num+1];
	reg |= READ;
	
	BMI_CS_L();
	SPIdrv->Transfer(&reg, data_in, num + 1);
	osSignalWait(SEND_DONE, osWaitForever);
	BMI_CS_H();
	
	memcpy(pdata, data_in + 1, num);
}

void BMI160_read_fifo (uint8_t num, fifo_t * pdata)	{
	uint8_t data_out = READ|REG_FIFO_DATA;
		
	if ( num  > 50 ) {
		Log(RTT_BG_YELLOW RTT_TEXT_BLACK "BMI160 read fifo overflow: %d\n",num);
		num = 50;
	}
	
	BMI_CS_L();
	SPIdrv->Transfer(&data_out, (uint8_t*)pdata->buffer - sizeof(data_out), sizeof(data_out) + num * sizeof(axis_t));
	osSignalWait(SEND_DONE, osWaitForever);
	BMI_CS_H();
	
	pdata->num = num;
}

int16_t BMI160_get_fifo_len()
{
	int16_t len;
	BMI160_multi_read(REG_FIFO_LEN_L, 2, (uint8_t*)&len);
	len &= 0x07FF;
	return len;
}

int BMI160_Init(ARM_DRIVER_SPI* SPIx) {
	
	GPIO_InitTypeDef InitStruct;

	if ( SPIx == NULL ) {
		Log("Error : No SPI used by BMI160.\n ");
		return -1;
	} else {
		SPIdrv = SPIx;
	}
	
	/* Init CS pin     */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	InitStruct.Pin   = BMI_CS_PIN;
	InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;	//litao	2017-05-16
	InitStruct.Pull  = GPIO_NOPULL;		//litao	2017-05-16
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BMI_CS_PORT, &InitStruct);
	HAL_GPIO_WritePin(BMI_CS_PORT, BMI_CS_PIN, GPIO_PIN_SET);
	
	/* Init INT pin     */
//	__HAL_RCC_GPIOC_CLK_ENABLE();
//	InitStruct.Pin   = BMI_INT1_PIN;
//	InitStruct.Mode  = GPIO_MODE_IT_RISING;
//	InitStruct.Pull  = GPIO_PULLDOWN;
//	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(BMI_INT1_PORT, &InitStruct);

//	HAL_NVIC_SetPriority(EXTI1_IRQn, 4, 0);
//	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	if ( BMI160_read(REG_CHIPID) != 0xD1) {
		Log(RTT_TEXT_RED"No BMI 160 found.\n");
		osThreadTerminate(osThreadGetId());
	}
	
	BMI160_write(REG_CMD, CMD_SOFTRESET);
	osDelay(100);
	
	Log("BMI160 ERR = %08X\n", BMI160_read(REG_ERROR));
	Log("BMI160 PMU = %08X\n", BMI160_read(REG_PMU_STATUS));
	Log("BMI160 STA = %08X\n", BMI160_read(REG_STATUS));
	
	BMI160_write(REG_ACC_CONF,   0x29);
	osDelay(1);
	BMI160_write(REG_ACC_RANGE,  0x0C);	//量程+/-16
	osDelay(1);
	BMI160_write(REG_GYR_CONF,   0x29);
	osDelay(1);
	BMI160_write(REG_GYR_RANGE,  0x01);
	osDelay(1);
	
	BMI160_write(REG_FIFO_WATERMARK, 0xFF);
	osDelay(1);
	BMI160_write(REG_FIFO_CONF, 1<<7|1<<6);
	osDelay(1);
	
	Log("FIFO WATERMARK = %08X\n", BMI160_read(REG_FIFO_WATERMARK));
	Log("FIFO CONFIG    = %08X\n", BMI160_read(REG_FIFO_CONF));
	Log("GYO CONFIG     = %08X\n", BMI160_read(REG_GYR_CONF));
	Log("GYO RANGE      = %08X\n", BMI160_read(REG_GYR_RANGE));
	Log("ACC CONFIG     = %08X\n", BMI160_read(REG_ACC_CONF));
	Log("ACC RANGE      = %08X\n", BMI160_read(REG_ACC_RANGE));
	
	return 0;
}
