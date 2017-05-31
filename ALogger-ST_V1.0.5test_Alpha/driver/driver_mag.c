#include "osObjects.h"
#include "Driver_SPI.h"
#include "rtt_log.h"
#include "stm32f4xx_hal.h"


#ifdef SELECT_BMM150
#include "BMM150.h"
#include "BMM150_reg.h"
#else
#include "LIS3MDL.h"
#include "LIS3MDL_reg.h"
#endif

extern ARM_DRIVER_SPI Driver_SPI2;

void driver_mag(void const* arg)
{
	osEvent evt;
	osStatus status;
	uint8_t  idx = 0;
	uint32_t msg;
	
	raw_3axis_block_t* rawdata_p;
	
	msg_mag = osMessageCreate(osMessageQ(msg_mag), NULL);
	pool_mag = osPoolCreate(osPool(pool_mag));
	
	tid_mag_EvtHandler = osThreadCreate(osThread(mag_EvtHandler), NULL);
	if (tid_mag_EvtHandler == NULL) Log("[mag] : No Evthandler.\n");
	
	rawdata_p = osPoolCAlloc(pool_mag);
	rawdata_p->type = Mag;
				
#ifdef SELECT_BMM150
	BMM150_Init(&Driver_SPI2);
#else
	LIS3MDL_Init(&Driver_SPI2);
#endif

  while (1)	{
		evt = osMessageGet(msg_mag, osWaitForever);
		msg = evt.value.v;
		Log(RTT_BG_MAGENTA RTT_TEXT_BLACK"[mag] "RTT_RESET": thd msg 0x%08X\n", msg);
		
#ifdef SELECT_BMM150
		switch (MSG_TYPE(msg)) {
			case START:
				BMM150_write(BMM_CTRL, NORMAL_MODE | ODR_30);
				idx = 0;
				break;
			
			case STOP:
				BMM150_write(BMM_CTRL, SLEEP_MODE | ODR_30);
				BMM150_read(BMM_INT_STAT_REG);
				
				break;
			
			case DATA_RDY:
/*     TBD      */
//				mag_rawdata_p = osPoolAlloc(pool_mag);
//				if ( mag_rawdata_p == NULL ) {
//					Log("Error Mag Pool Alloc\n");
//					BMM150_read(BMM_INT_STAT_REG);
//					
//				} else {
//					mag_rawdata_p->type = Mag;
//					
//					BMM150_multi_read(0x42, 6, (rawdata_p->axis_array + idx));
//					osMessagePut(msg_fs, (uint32_t)mag_rawdata_p, 10);
//				}
				
				break;
				
			case FIFO_RD:
				osMessagePut(msg_fs, (uint32_t)rawdata_p, osWaitForever);
				rawdata_p = osPoolCAlloc(pool_acc);
				rawdata_p->type = Mag;
				idx = 0;
				break;
			
			default:
				;
		}
#else		
		switch (MSG_TYPE(msg)) {
			case START:
				LIS3MDL_write(LIS3MDL_CTRL_REG3, CONTINUOUS_MODE);
				break;
			
			case STOP:
				LIS3MDL_write(LIS3MDL_CTRL_REG3, STOP_MODE);
				idx = 0;
				break;
			
			case DATA_RDY:				
				LIS3MDL_multi_read(LIS3MDL_OUT_X_L, 6, (uint8_t*)(rawdata_p->axis_array + idx));
				idx++;
				idx = idx % 20;
				rawdata_p->num = idx;
				break;
				
			case FIFO_RD:
				// Send this frame data
				rawdata_p->timestamp = m_timestamp_per_100ms;
				osMessagePut(msg_fs, (uint32_t)rawdata_p, osWaitForever);
				
				// Allocate a newone to hold new samples
				rawdata_p = osPoolCAlloc(pool_mag);
				if ( rawdata_p != NULL) {
					rawdata_p->type = Mag;
					idx = 0;
				} else {
					Log(RTT_BG_MAGENTA RTT_TEXT_BLACK"[mag] "RTT_RESET": No avaliable pool space.\n", msg);
				}
				break;
			
			default:
				;
		}
#endif
  }
}

void mag_EvtHandler(void const* arg) {
	osEvent evt;
	
	while(1) {
		evt = osSignalWait(ANY_SIG, osWaitForever);
		
		switch(evt.value.signals) {
			case DRDY_SIG:
				osMessagePut(msg_mag, THD_MSG(DATA_RDY), 0);
				break;
			
			case FIFO_SIG:
				osMessagePut(msg_mag, THD_MSG(FIFO_RD), 0);
				break;
			
			default:
				break;
		}
			
	}
}
