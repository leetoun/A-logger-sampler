#include <string.h>
#include "osObjects.h"

#include "I2C.h"
#include "rtt_log.h"
#include "stm32f405xx.h"

#include "ADPD174.h"

#define SEND_DONE   (1<<0)

#define HRV_ RTT_BG_MAGENTA RTT_TEXT_BLACK"[hrv] :"RTT_RESET
uint16_t dummy[HRV_FIFO_NUM] = {0};
uint16_t payload2;


void driver_hrv (void const *argument) {
	osEvent evt;
	osStatus status;
	hrv_t *hrv_p;
	uint8_t fifo_num;
	uint8_t regadd;
	uint16_t payload;
	uint16_t mode = 0;
	uint32_t msg;
	thd_msg_t type;

	msg_hrv = osMessageCreate(osMessageQ(msg_hrv),  NULL);
	pool_hrv = osPoolCreate(osPool(pool_hrv));
	
	tid_hrv_EvtHandler = osThreadCreate(osThread(hrv_EvtHandler), NULL);
	if ( tid_hrv_EvtHandler == NULL )
		Log(HRV_"Error no EvtHandler\n");
	
	ADPD174_Init(&Driver_I2C1);
	
  while (1) {
		evt = osMessageGet(msg_hrv, osWaitForever);
		msg = evt.value.v;
		Log(HRV_" thd msg 0x%08X\n", msg);
		type = MSG_TYPE(msg);
		
		switch ( type ) {
			case START:
				ADPD174_write(0x10, 1);
				ADPD174_write(0x4B,0x2680|CAIL); 
				ADPD174_write(0x10, 2);      // 进入Normal 模式.
				mode = 2;
				break;
			
			case STOP:
				ADPD174_write(0x10, 1);
				ADPD174_write(0x4B,0x2600);
				ADPD174_write(0x5F, 1);
				ADPD174_write(0x5F, 1);      //写两次0x01 FIFO_ACCESS_EN (打开 32MHz 时钟)
				ADPD174_write(0x00,0x80FF);  // 清中断 清FIFO
				ADPD174_write(0x5F, 0);
				ADPD174_write(0x10, 0);      // 进入Standby 模式
				mode = 0;
				break;
			
			case DATA_RDY:

				break;
						
			case FIFO_RD:
				hrv_p = (hrv_t*)osPoolAlloc(pool_hrv);
				fifo_num = ADPD174_read(0x00) >> 9;
				Log(HRV_" FIFO contains %d \n",fifo_num);

				if( fifo_num > 10 ) 
					fifo_num = 10;
				
				if ( hrv_p != NULL ) {
					hrv_p->timestamp = m_timestamp_per_100ms;
					hrv_p->type = HRV;
					hrv_p->num  = fifo_num;
					ADPD174_read_fifo(fifo_num , hrv_p->fifo);

					status = osMessagePut(msg_fs, (uint32_t)hrv_p, 10);
					if ( status != osOK )
						Log(HRV_"FS queue is full!\n");
				} else {
					Log(RTT_BG_BLUE RTT_TEXT_BLACK"No avaliable HRV memblock.\n"RTT_RESET);
					ADPD174_write(0x10, 1);
					ADPD174_clear_fifo();
					ADPD174_write(0x10, 2);
				}
				break;
				
			case CONFIG:
				ADPD174_write(0x10, 1);            // 进入Program 模式
				payload = (uint16_t) msg;
				regadd  = (uint8_t) (msg >> 16);
				ADPD174_write(regadd, payload);
				ADPD174_write(0x5F, 1);
				ADPD174_write(0x5F, 1);         //写两次0x01 FIFO_ACCESS_EN (打开 32MHz 时钟)
				ADPD174_write(0x00,0x80FF);  // 清中断 清FIFO
				ADPD174_write(0x5F, 0);
				ADPD174_write(0x10, mode);
				break;
			
			default:
				Log("[hrv] : Unknow thd msg !\n");
				break;
		}
	}
}

void hrv_EvtHandler(void const* arg){
	osEvent evt;
	
	while(1) {
		evt = osSignalWait(ANY_SIG, osWaitForever);
		switch ( evt.value.signals ) {
			case DRDY_SIG:
				osMessagePut(msg_hrv, THD_MSG(DATA_RDY), 0);
				break;
			case FIFO_SIG:
				osMessagePut(msg_hrv, THD_MSG(FIFO_RD), 0);
				break;
			default:
				APP_ERROR_CHECK(0xDEADBEEF);
		}
	}
}
