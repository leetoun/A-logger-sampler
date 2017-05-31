#include <string.h>
#include "osObjects.h"
#include "Driver_SPI.h"
#include "rtt_log.h"
#include "stm32f4xx_hal.h"
#include "BMM150.h"
#include "BMM150_reg.h"

#define SEND_DONE     (1<<0)

#define READ          (0x80)
#define WRITE         (0x00)

#define BMM_INT_PORT       GPIOC
#define BMM_INT_PIN        GPIO_PIN_1

#define BMM_CS_PORT        GPIOB
#define BMM_CS_PIN         GPIO_PIN_9


#define BMM_CS_L()                                                       \
do {                                                                     \
	BUS_ACQUIRE(spi2);                                                   \
	tid_spi2x = osThreadGetId();                                         \
	BMM_CS_PORT->BSRR = (uint32_t)BMM_CS_PIN << 16;                      \
} while(0)
				
#define BMM_CS_H()                                                       \
do {                                                                     \
	BMM_CS_PORT->BSRR = BMM_CS_PIN;                                      \
	BUS_RELEASE(spi2);                                                   \
} while(0)
				
#define BUS_ACQUIRE(x)  osMutexWait(mutex_##x, osWaitForever)
#define BUS_RELEASE(x)  osMutexRelease(mutex_##x)

#define ENABLE_PORT_CLK(x) __HAL_RCC_##x##_CLK_ENABLE()

static ARM_DRIVER_SPI* SPIdrv = NULL;

int BMM150_read(uint8_t reg)	{
	uint8_t data_in[2] = {0};
	reg |= READ;
	
	BMM_CS_L();
	SPIdrv->Transfer(&reg, data_in, 2);
	osSignalWait(SEND_DONE, osWaitForever);
	BMM_CS_H();
	
	return data_in[1];
}

void BMM150_write(uint8_t reg, uint8_t value)	{
	uint8_t data_out[2] = {reg, value};
	uint8_t data_in[2];
	
	BMM_CS_L();
	SPIdrv->Transfer(data_out, data_in, 2);
	osSignalWait(SEND_DONE, osWaitForever);
	BMM_CS_H();
}

void BMM150_multi_read (uint8_t reg, uint8_t num, uint8_t* pdata)	{
	uint8_t data_in[64];
	reg |=READ;
	
	if ( num > 63 )	{
		Log("BMM150 Multi-Read overflow.\n");
		return;
	}
		
	BMM_CS_L();
	SPIdrv->Transfer(&reg, data_in, num + 1);
	osSignalWait(SEND_DONE, osWaitForever);
	BMM_CS_H();
	
	memcpy(pdata, data_in + 1, num);
}

void BMM150_multi_write(uint8_t reg, uint8_t num, uint8_t* pdata) {
	
}

int BMM150_Init(ARM_DRIVER_SPI* SPIx) {
	
	GPIO_InitTypeDef InitStruct;

	if ( SPIx == NULL ) {
		Log("Error : No SPI used by BMM150.\n ");
		return -1;
	} else {
		SPIdrv = SPIx;
	}
	
	/* Init CS pin     */
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	InitStruct.Pin   = BMM_CS_PIN;
	InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;		//litao	2017-05-16
	InitStruct.Pull  = GPIO_NOPULL;			//litao	2017-05-16
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BMM_CS_PORT, &InitStruct);
	HAL_GPIO_WritePin(BMM_CS_PORT, BMM_CS_PIN, GPIO_PIN_SET);
	
	/* Init INT pin     */
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	InitStruct.Pin   = BMM_INT_PIN;
	InitStruct.Mode  = GPIO_MODE_IT_RISING;
	InitStruct.Pull  = GPIO_PULLDOWN;
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BMM_INT_PORT, &InitStruct);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 4, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	
	BMM150_write(BMM_POWER_CTRL, SOFT_RESET|1);
	osDelay(100);
	
	BMM150_write(BMM_POWER_CTRL, 1);
	osDelay(5);
		
	if ( BMM150_read(BMM_CHIP_ID) != 0x32) {
		Log(RTT_TEXT_RED"No BMM 150 found.\n");
		osThreadTerminate(osThreadGetId());
	}
	
//	Log("BMM150 ID 0x%02X\n", BMM150_read(BMM_CHIP_ID));
//	Log("BMM150 REG 0x4C = 0x%02X\n", BMM150_read(BMM_CTRL));
//	Log("BMM150 REG 0x4E = 0x%02X\n", BMM150_read(BMM_SENS_CTRL));
	
	Log("\nInit BMM150... \n");
	BMM150_write(BMM_CTRL, SLEEP_MODE | ODR_30);
	BMM150_write(BMM_SENS_CTRL, DR_EN | DR_ACTIVE_H);
//	BMM150_write(BMM_REP_XY, 16);
//	BMM150_write(BMM_REP_Z,  16);
	
	Log("BMM150 REG 0x4C = 0x%02X\t0x%02X\n", BMM150_read(BMM_CTRL), SLEEP_MODE | ODR_30);
	Log("BMM150 REG 0x4E = 0x%02X\t0x%02X\n", BMM150_read(BMM_SENS_CTRL), DR_EN | DR_ACTIVE_H);
	
	return 0;
}
