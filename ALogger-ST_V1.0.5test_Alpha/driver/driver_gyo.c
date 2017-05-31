#include "osObjects.h"
#include "Driver_SPI.h"
#include "rtt_log.h"
#include "stm32f4xx_hal.h"
#include "BMI160.h"
#include "BMI160_reg.h"
#define SEND_DONE     (1<<0)

#define READ          (0x80)
#define WRITE         (0x00)
#define MULTI         (0x40)

#define _GYR_ RTT_BG_BLUE RTT_TEXT_BLACK"[gyr] :"RTT_RESET

extern ARM_DRIVER_SPI Driver_SPI1;

static fifo_t bmi_fifo;

void driver_gyr(void const* arg)
{
	osEvent evt;

	int16_t  fifo_num = 0;
	uint32_t msg;
	
	raw_3axis_block_t* acc_data_p;
	raw_3axis_block_t* gyr_data_p;
	
	msg_gyr = osMessageCreate(osMessageQ(msg_gyr), NULL);
	pool_acc = osPoolCreate(osPool(pool_acc));
	pool_gyr = osPoolCreate(osPool(pool_gyr));

	tid_gyr_EvtHandler = osThreadCreate(osThread(gyr_EvtHandler), NULL);
	if (tid_gyr_EvtHandler == NULL) Log("[gyr] : No Evthandler.\n");
	
	BMI160_Init(&Driver_SPI1);
	
	while (1)	{
		evt = osMessageGet(msg_gyr, osWaitForever);
		msg = evt.value.v;
		Log(_GYR_" thd msg 0x%08X\n", msg);
		
		switch (MSG_TYPE(msg)) {
			case START:
				BMI160_write(REG_CMD, CMD_FIFOFLUSH);    // clear fifo
				osDelay(5);
				BMI160_write(REG_CMD, CMD_ACC_NORMAL);
				osDelay(5);
				BMI160_write(REG_CMD, CMD_GYR_NORMAL);
				osDelay(5);
				break;
			
			case STOP:
				BMI160_write(REG_CMD, CMD_ACC_SUSPEND);
				osDelay(5);
				BMI160_write(REG_CMD, CMD_GYR_SUSPEND);
				osDelay(5);
				break;
			
			case DATA_RDY:

				break;
				
			case FIFO_RD:
				fifo_num = BMI160_get_fifo_len() / sizeof(axis_t);
				BMI160_read_fifo(fifo_num, &bmi_fifo);

				acc_data_p = osPoolCAlloc(pool_acc);
				gyr_data_p = osPoolCAlloc(pool_gyr);
				
				if ( acc_data_p != NULL && gyr_data_p != NULL ) {
					for ( int i = 0; i < fifo_num / 2; i++) {
						gyr_data_p->axis_array[i] = bmi_fifo.buffer[i*2];
						acc_data_p->axis_array[i] = bmi_fifo.buffer[i*2+1];
					}
					acc_data_p->timestamp = m_timestamp_per_100ms;
					acc_data_p->type = Acc;
					acc_data_p->num  = fifo_num / 2;
					osMessagePut(msg_fs, (uint32_t)acc_data_p, osWaitForever);

					gyr_data_p->timestamp = m_timestamp_per_100ms;
					gyr_data_p->type = Gyr;
					gyr_data_p->num  = fifo_num / 2;
					osMessagePut(msg_fs, (uint32_t)gyr_data_p, osWaitForever);

				} else {
					Log(RTT_BG_RED"No avaliable GYR memblock !\n"RTT_RESET);
					BMI160_write(REG_CMD, CMD_FIFOFLUSH);   // cleanup FIFO
				}

				break;
			
			default:
				break;
		}
	}
}

void gyr_EvtHandler(void const* arg) {
	osEvent evt;
	
	while(1) {
		evt = osSignalWait(ANY_SIG, osWaitForever);
		
		switch(evt.value.signals) {
			case DRDY_SIG:
				osMessagePut(msg_gyr, THD_MSG(DATA_RDY), 0);
				break;
			
			case FIFO_SIG:
				osMessagePut(msg_gyr, THD_MSG(FIFO_RD), 0);
				break;
			
			default:
				break;
		}
			
	}
}
