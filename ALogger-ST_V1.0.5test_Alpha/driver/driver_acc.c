#include "osObjects.h"
#include "SPI.h"
#include "rtt_log.h"
#include "stm32f4xx_hal.h"

#include "driver_acc.h"


void driver_acc(void const *arg)
{
	osEvent evt;
	
	uint8_t fifo_num;
	uint32_t msg;
	thd_msg_t type;
	raw_3axis_block_t *rawdata_p;
	
	msg_acc = osMessageCreate(osMessageQ(msg_acc), NULL);
	pool_acc = osPoolCreate(osPool(pool_acc));
	
	tid_acc_EvtHandler = osThreadCreate(osThread(acc_EvtHandler), NULL);
	if (tid_acc_EvtHandler == NULL)	Log("couldn't spawn acc handler.\n");

	ADXL362_Init(&Driver_SPI1);
  
	while (1)	{
		evt = osMessageGet(msg_acc, osWaitForever);
		msg = evt.value.v;
		Log(RTT_BG_YELLOW RTT_TEXT_BLACK"[acc] "RTT_RESET": thd msg 0x%08X\n", msg);
		
		type = MSG_TYPE(msg);
		
		switch ( type ) {

			case STOP:
				ADXL362_write(0x2D, 0x20);        // 进入待机模式
				ADXL362_write(0x28, 0x00);        // 关FIFO
				break;

			case START:
				ADXL362_write(0x28, 0x02);        // 开FIFO
				ADXL362_write(0x2D, 0x22);        // 进入工作模式
				break;

			case DATA_RDY:
				
				break;
			
			case FIFO_RD:
				rawdata_p = (raw_3axis_block_t*) osPoolCAlloc(pool_acc);
				fifo_num = ADXL362_read(ADXL_FIFO_ENTRIES_L) / 3;
			
				if (rawdata_p != NULL) {
					rawdata_p->timestamp = m_timestamp_per_100ms;
					rawdata_p->type = Acc;
					rawdata_p->num  = fifo_num;
					ADXL362_read_fifo(fifo_num, rawdata_p);
					osMessagePut(msg_fs, (uint32_t)rawdata_p, osWaitForever);
				} else {
					Log(RTT_BG_MAGENTA RTT_TEXT_BLACK"No avaliable ACC memblock.\n" RTT_RESET);
					ADXL362_write(0x28, 0x00);        // 关FIFO
					ADXL362_write(0x28, 0x02);        // 开FIFO
				}
				
				break;
			
			default:
				Log("[acc] : Unkown type thd msg 0x%08X\n", msg);
		}
	}
}

void acc_EvtHandler(void const *arg)	{
	osEvent evt;
	
	while(1) {
		evt = osSignalWait(ANY_SIG, osWaitForever);
		switch ( evt.value.signals ) {
			case DRDY_SIG:
				osMessagePut(msg_acc, THD_MSG(DATA_RDY), osWaitForever);
				break;
				
			case FIFO_SIG:
				osMessagePut(msg_acc, THD_MSG(FIFO_RD), osWaitForever);
				break;
			
			default:
				
				break;
		}
	}
}
