#include "osObjects.h"
#include "Driver_I2C.h"
#include "rtt_log.h"
#include "stm32f4xx_hal.h"

#define SEND_DONE     (1<<0)

#define READ          (0x80)
#define WRITE         (0x00)
#define MULTI         (0x40)

osThreadId tid_i2c1x;

static void I2C1_EvtHandler(uint32_t event);

void I2C_Init(ARM_DRIVER_I2C*    drv,
                    uint32_t control,
                    uint32_t   speed,
              ARM_I2C_SignalEvent_t I2C_EvtHandler)	
{
	volatile uint32_t error;
	Log("Init I2C @ 0x%08X ...\t", drv);
	error = drv->Initialize(I2C1_EvtHandler);	//gpio
	error = drv->PowerControl(ARM_POWER_FULL);	//power
	error = drv->Control(control, speed);		//speed
	error = drv->Control(ARM_I2C_BUS_CLEAR, 0);	//
	//添加了解锁I2C1 busy死锁问题的代码：
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef InitStruct;
	InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	InitStruct.Mode = GPIO_MODE_AF_OD;
	InitStruct.Pull = GPIO_PULLUP;
	InitStruct.Speed = GPIO_SPEED_FAST;
	InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &InitStruct);
	APP_ERROR_TEST(error, 0);
}
/*
void I2C_ReleaseBusy(void)
{
	if(I2C1->SR2 & 0x02)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitTypeDef InitStruct;
		InitStruct.Pin   = GPIO_PIN_6|GPIO_PIN_7;
		InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
		InitStruct.Pull  = GPIO_NOPULL;
		InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB,&InitStruct);

		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, GPIO_PIN_SET);

		//I2C1->CR1 |= I2C_CR1_SWRST;		//复位I2C控制器
		//I2C1->CR1 &= ~I2C_CR1_SWRST;		//解除复位（不会自动清除）
		__HAL_RCC_I2C2_FORCE_RESET();
		__NOP(); __NOP(); __NOP(); __NOP(); 
		__HAL_RCC_I2C2_RELEASE_RESET();
		

	
		InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
		InitStruct.Mode = GPIO_MODE_AF_OD;
		InitStruct.Pull = GPIO_PULLUP;
		InitStruct.Speed = GPIO_SPEED_FAST;
		InitStruct.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &InitStruct);
	}
}
*/

void I2C1_EvtHandler(uint32_t event)
{
	switch ( event )	{
		case ARM_I2C_EVENT_TRANSFER_DONE:
			if (!tid_i2c1x) Log(RTT_BG_RED"[I2C1]: No Device mount on.\n");
			osSignalSet(tid_i2c1x, SEND_DONE);
			tid_i2c1x = NULL;
    break;
		
		default:
			Log(RTT_TEXT_RED "I2C Event : %d \n", event);
	}
			
	event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE ?
	Log(" Master/Slave Transmit/Receive incomplete transfer\n"): 0;
	
	event & ARM_I2C_EVENT_SLAVE_TRANSMIT ?
	Log(" Slave Transmit operation requested\n"): 0;
	
	event & ARM_I2C_EVENT_SLAVE_RECEIVE ?
	Log(" Slave Receive operation requested\n"): 0;
	
	event & ARM_I2C_EVENT_ADDRESS_NACK ?
	Log(RTT_TEXT_YELLOW" WTF?? Address not acknowledged from Slave\n"): 0;
	
	event & ARM_I2C_EVENT_GENERAL_CALL ?
	Log(" General Call indication\n"): 0;
	
	event & ARM_I2C_EVENT_ARBITRATION_LOST ?
	Log(" Master lost arbitration\n"): 0;
	
	event & ARM_I2C_EVENT_BUS_ERROR ?
	Log(RTT_BG_YELLOW" Bus error detected (START/STOP at illegal position)\n"): 0;
	
	event & ARM_I2C_EVENT_BUS_CLEAR ?
	Log(" Bus clear finished\n"):	0;
}

