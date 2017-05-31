#include "osObjects.h"
#include "I2C.h"
#include "stm32f4xx_hal.h"

#include "rtt_log.h"
#include "ADPD174.h"

#define SEND_DONE   (1<<0)

#define BUS_A()  BUS_ACQUIRE(i2c1)
#define BUS_R()  BUS_RELEASE(i2c1)

#define BUS_ACQUIRE(x)  osMutexWait(mutex_##x, osWaitForever)
#define BUS_RELEASE(x)  osMutexRelease(mutex_##x)

#define FIFO_ACCESS_EN()                                                       \
        do {ADPD174_write(0x5F, 1);ADPD174_write(0x5F, 1);} while(0)
				
#define FIFO_ACCESS_DIS()   ADPD174_write(0x5F, 0)
				
static ARM_DRIVER_I2C* I2Cdrv = NULL;

uint16_t ADPD174_read (uint8_t reg)
{
	uint8_t  data[2];
	
	BUS_A();
	tid_i2c1x = osThreadGetId();
	I2Cdrv->MasterTransmit (ADPD174_ADDRESS, &reg, 1, true);
	osSignalWait(SEND_DONE, osWaitForever);
	
	tid_i2c1x = osThreadGetId();
	I2Cdrv->MasterReceive (ADPD174_ADDRESS, data, 2, false);
	osSignalWait(SEND_DONE, osWaitForever);
	BUS_R();
	return ( data[0] << 8 | data[1] );
}

void ADPD174_write (uint8_t reg_address, uint16_t value)
{
	uint8_t buffer[3] = {reg_address,0};
	*(uint16_t*)(buffer + 1) = __REV16(value);
	
	BUS_A();
	tid_i2c1x = osThreadGetId();
	I2Cdrv->MasterTransmit (ADPD174_ADDRESS, buffer, 3, false);
	osSignalWait(SEND_DONE, osWaitForever);
	BUS_R();
}

void ADPD174_multi_write(uint8_t reg, uint8_t num, uint16_t *pData)
{
	
}

void ADPD174_multi_read(uint8_t reg, uint8_t num, uint16_t *pData)
{
	BUS_A();
	tid_i2c1x = osThreadGetId();
	I2Cdrv->MasterTransmit (ADPD174_ADDRESS, &reg, 1, true);
	osSignalWait(SEND_DONE, osWaitForever);
	
	tid_i2c1x = osThreadGetId();
	I2Cdrv->MasterReceive (ADPD174_ADDRESS, (uint8_t*) pData, 2 * num, false);
	osSignalWait(SEND_DONE, osWaitForever);
	BUS_R();
	
	for(int i = 0; i < num; i++, pData++)
		*pData = __REV16(*pData);
	
}
void ADPD174_read_fifo(uint8_t num, uint16_t* pdata)
{
	ADPD174_write(0x5F, 1);
	ADPD174_write(0x5F, 1);         //写两次0x01 FIFO_ACCESS_EN (打开 32MHz 时钟)
	ADPD174_multi_read(0x60, num, pdata);
	ADPD174_write(0x5F, 0);         // 读完FIFO 关 32MHz 时钟
}
void ADPD174_clear_fifo(void)
{
	ADPD174_write(0x5F, 1);
	ADPD174_write(0x5F, 1);         //写两次0x01 FIFO_ACCESS_EN (打开 32MHz 时钟)
	ADPD174_write(0x00, 1<<15);
	ADPD174_write(0x5F, 0);         // 读完FIFO 关 32MHz 时钟
}
int ADPD174_Init(ARM_DRIVER_I2C* I2Cx)
{
	GPIO_InitTypeDef InitStruct;
	
	if ( I2Cx == NULL )
		return -1;
	else
		I2Cdrv = I2Cx;

	InitStruct.Pin   = GPIO_PIN_5;
	InitStruct.Mode  = GPIO_MODE_IT_FALLING;
	InitStruct.Pull  = GPIO_PULLUP;
	InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB,&InitStruct);		              /* Init ADPD174 pin EXTI   */ 
	
	if ( ADPD174_read(0x08) != 0x0416 ) {
		Log(RTT_TEXT_RED"No ADPD 174 found.\n");
		osThreadTerminate(osThreadGetId());
	}
			
	ADPD174_write(0x10, 1);
	ADPD174_write(0x4B,0x2680|CAIL);  //32k clk  使能+校准
	
	/***  LED Control Registers  ***/
	
	ADPD174_write(0x14,0x0447);  // LED select 
	ADPD174_write(0x22,0x3030);  // ILED3 coarse
	ADPD174_write(0x23,0x3030);  // ILED1 coarse
	ADPD174_write(0x24,0x3030);  // ILED2 coarse
	ADPD174_write(0x25,0x02CC);  // ILEDx fine
	ADPD174_write(0x34,  1<<8);  // SLOTx enable: Disable SLOTA LED
	ADPD174_write(0x31,0x13|PULSE_A<<8);  // SLOTA pulse
	ADPD174_write(0x36,0x13|PULSE_B<<8);  // SLOTB pulse
	ADPD174_write(0x30,0x0319);  // SLOTx LED mode
	ADPD174_write(0x35,0x0319);  // n/a value

	
	/***  AFE Configuration Registers  ***/

	ADPD174_write(0x39,0x21F0);  // SLOTx AFE mode
	ADPD174_write(0x3B,0x21F0);  // n/a value
	ADPD174_write(0x3C,0x31C6);
	ADPD174_write(0x42,0x1C36);  // A AFE TIA Gain
	ADPD174_write(0x44,0x1C36);  // B AFE TIA Gain
	ADPD174_write(0x43,0xADA5);
	ADPD174_write(0x45,0xADA5);
	ADPD174_write(0x4e,0x0040);  // ADC Timing n/a

	/***  System Registers  ***/

	ADPD174_write(0x01,0x00FF);  // INT MASK
	ADPD174_write(0x02,0x0005);  // INT IO CTL
	ADPD174_write(0x06,(HRV_FIFO_NUM - 1) << 8);    // FIFO THRESH (word: 2-bytes)
	ADPD174_write(0x11,0x1060);  // FIFO Config：slotB enable，slotA disable；
    
	/***  ADC Registers  ***/
	
	ADPD174_write(0x12,8000 / FSAMPLE);  // f (sample) = 8K / Reg 0x12
	ADPD174_write(0x15,AVG_B<<8 | AVG_A);  // num_avg   = 2 power 0..7
	ADPD174_write(0x18,0x1F00);
	ADPD174_write(0x19,0x3FFF);  // slot x Channel 1..3
	ADPD174_write(0x1A,0x3FFF);  // n/a value 
	ADPD174_write(0x1B,0x3FFF);
	ADPD174_write(0x1E,0x1F00);
	ADPD174_write(0x1F,0x3FFF);
	ADPD174_write(0x20,0x3FFF);
	ADPD174_write(0x21,0x3FFF);

	ADPD174_write(0x5F,1);  // DATA ACCESS  
	ADPD174_write(0x5F,1);  // 写两次0x01 FIFO_ACCESS_EN ( power on 32M clock )
	ADPD174_write(0x00,0x80FF);  //写0x80ff到寄存器0清中断
	ADPD174_write(0x5F,0);
	ADPD174_write(0x54,0x0020);
	ADPD174_write(0x58,0x0000);
	
	ADPD174_write(0x10, 0);
	
	return 0;
}
