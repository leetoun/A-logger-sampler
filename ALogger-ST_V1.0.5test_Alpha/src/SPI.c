#include "osObjects.h"
#include "Driver_SPI.h"
#include "rtt_log.h"
#include "stm32f4xx_hal.h"

#define SEND_DONE     (1<<0)

osThreadId tid_spi1x;
osThreadId tid_spi2x;

void SPI_Init(ARM_DRIVER_SPI* SPIdrv,
                    uint32_t control,
                    uint32_t   speed,
              ARM_SPI_SignalEvent_t SPI_EvtHandler)
{
	uint32_t error;
	Log("Init SPI @ 0x%08X ...\t", SPIdrv);
	
	error  = SPIdrv->Initialize(SPI_EvtHandler);
	error += SPIdrv->PowerControl(ARM_POWER_FULL);
	error += SPIdrv->Control(control, speed);
	APP_ERROR_TEST(error, 0);
}

void SPI1_EvtHandler(uint32_t event)
{
	switch (event)
	{
		case ARM_SPI_EVENT_TRANSFER_COMPLETE:
			if (!tid_spi1x) Log(RTT_BG_RED"[SPI1]: No Driver mount.\n");
			osSignalSet(tid_spi1x, SEND_DONE);
			break;
		
		case ARM_SPI_EVENT_DATA_LOST:
				/*  Occurs in slave mode when data is requested/sent by master
						but send/receive/transfer operation has not been started
						and indicates that data is lost. */
				Log(RTT_TEXT_RED"[SPI1]: Error DATA_LOST\n");
				break;
		
		case ARM_SPI_EVENT_MODE_FAULT:
				/*  Occurs in master mode when Slave Select is deactivated and
						indicates Master Mode Fault. */
				Log(RTT_TEXT_RED"[SPI1]: Error MODE_FAULT\n");
				break;
	}
	
	tid_spi1x = NULL;
	
}

void SPI2_EvtHandler(uint32_t event)
{
	switch (event)	{
		case ARM_SPI_EVENT_TRANSFER_COMPLETE:

			if (!tid_spi2x) Log(RTT_BG_RED"[SPI2]: No Driver mount.\n");
			osSignalSet(tid_spi2x, SEND_DONE);
			break;
		
		case ARM_SPI_EVENT_DATA_LOST:
			/*  Occurs in slave mode when data is requested/sent by master
				but send/receive/transfer operation has not been started
				and indicates that data is lost. */
			Log(RTT_TEXT_RED"[SPI2]: Error DATA_LOST\n");
			break;
		
		case ARM_SPI_EVENT_MODE_FAULT:
			/*  Occurs in master mode when Slave Select is deactivated and
				indicates Master Mode Fault. */
			Log(RTT_TEXT_RED"[SPI2]: Error MODE_FAULT\n");
			break;
	}
	
	tid_spi2x = NULL;

}
