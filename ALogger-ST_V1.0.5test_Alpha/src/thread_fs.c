#include <stdio.h>                      /* Standard I/O .h-file               */
#include <string.h>
#include "osObjects.h"
#include "rl_fs.h"
#include "rtt_log.h"

#include "stm32f4xx_hal.h"
#include "RTC.h"

#include "driver_hrv.h"

#define CONNECT 	      (1<<0)
#define DISCONNECT	      (1<<1)

int16_t mag_x,mag_y,mag_z;
int16_t acc_x,acc_y,acc_z;
int16_t gyr_x,gyr_y,gyr_z;
int16_t m_x,m_y,m_z;
uint16_t hrv;

uint32_t sample_id = 0;
uint16_t hrv_value;

FILE *fp_mag;
FILE *fp_acc;
FILE *fp_gyr;
FILE *fp_hrv;

extern int16_t cmd_set_value[5];

char file_name[20] = "001-log.csv";

char acc_buffer[BUFSIZ] = {0};
char gyr_buffer[BUFSIZ] = {0};
char mag_buffer[BUFSIZ] = {0};
char hrv_buffer[BUFSIZ] = {0};

static FILE* open_file(char* name, char* buffer, int buffer_size)
{
	FILE* fp = fopen(name, "w");
	if (fp != NULL) {
		Log("Touch %s.\n", name);
		setvbuf(fp, buffer, _IOFBF, buffer_size);
		return fp;
	} else {
		Log("Fail to create %s\n", name);
		return NULL;
	}
}

static void close_file(FILE* fp)
{
	fp = fp != NULL ? (FILE *)fclose(fp) : NULL;
	APP_ERROR_TEST(fp_hrv, NULL);
}

void thread_fs (void const *argument) {
	osEvent evt;
	osStatus status;
	uint32_t cnt;
	uint32_t msg;
	thd_msg_t type;
	
	raw_3axis_block_t *sensor_data_p;
	axis_t       *axis_p;
	hrv_t        *hrv_p;
	
	msg_fs = osMessageCreate(osMessageQ(msg_fs), NULL);
	
	while (1) {
		evt = osMessageGet(msg_fs, osWaitForever);
		msg = evt.value.v ;
		Log(RTT_BG_YELLOW"[fs]  "RTT_RESET": thd msg 0x%08X \n", msg);
		type = MSG_TYPE(msg);	//消息的类型，应该和按键相关联。
		switch ( type ) {
			case START:
				status = osMutexWait(mutex_sdcard, 1000);
				if ( status != osOK )
					break;

				Log("[fs]  :"RTT_TEXT_BLUE"TAKE"RTT_RESET" mutex_sdcard.\n");
				sample_id++;
				
				sprintf(file_name, "%d-acc.csv", sample_id);
				fp_acc = open_file(file_name, acc_buffer, 512);
				
				sprintf(file_name, "%d-gyr.csv", sample_id);
				fp_gyr = open_file(file_name, gyr_buffer, 512);
				
				sprintf(file_name, "%d-mag.csv", sample_id);
				fp_mag = open_file(file_name, mag_buffer, 512);	//litao

//				sprintf(file_name, "%d-hrv-p%d-a%d-o%d.csv", sample_id,
//								cmd_set_value[0], cmd_set_value[2], cmd_set_value[3]);

				sprintf(file_name, "%d-hrv.csv", sample_id);
				fp_hrv = open_file(file_name, hrv_buffer, 512);//litao
				
				osSignalSet(tid_term, DRDY_SIG);
				break;
			
			case STOP:
				Log(" Close acc file...");
				fp_acc = fp_acc != NULL ? (FILE *)fclose(fp_acc) : 0 ;
				APP_ERROR_TEST(fp_acc, NULL);
			
				Log(" Close gyr file...");
				fp_gyr = fp_gyr != NULL ? (FILE *)fclose(fp_gyr) : 0 ;
				APP_ERROR_TEST(fp_gyr, NULL);
			
				Log(" Close mag file...");
				fp_mag = fp_mag != NULL ? (FILE *)fclose(fp_mag) : 0 ;
				APP_ERROR_TEST(fp_mag, NULL);
			
				Log(" Close hrv file...");
				fp_hrv = fp_hrv != NULL ? (FILE *)fclose(fp_hrv) : 0 ;
				APP_ERROR_TEST(fp_hrv, NULL);
				
				osMutexRelease(mutex_sdcard);
				Log("[fs]  "RTT_TEXT_YELLOW"RELEASE"RTT_RESET" mutex_sdcard.\n");				
				break;
			
			case MEMBLCK :
				sensor_data_p = (raw_3axis_block_t *) msg;
				switch(sensor_data_p->type) {
					case Mag:
						Log("write mag...");
						axis_p = sensor_data_p->axis_array;
						fprintf(fp_mag, "%d,%d\r\n", sensor_data_p->timestamp, sensor_data_p->num);
						for ( int n = sensor_data_p->num; n > 0; n--) {
							cnt += fprintf(fp_mag,"%d,%d,%d\r\n", axis_p->x, axis_p->y, axis_p->z);
							axis_p++;
						}
						
						status = osPoolFree(pool_mag, sensor_data_p);
						APP_ERROR_TEST(status, osOK);
						break;
					
					case Acc:
						Log("write acc...");      
						axis_p = sensor_data_p->axis_array;
						fprintf(fp_acc,"%d,%d\r\n", sensor_data_p->timestamp, sensor_data_p->num);
						for ( int n = sensor_data_p->num; n > 0; n--) {
							acc_x = axis_p->x / 2;
							acc_y = axis_p->y / 2;
							acc_z = axis_p->z / 2;
							
							cnt += fprintf(fp_acc,"%d,%d,%d\r\n", acc_x, acc_y, acc_z);
							axis_p++;
						}
						
						status = osPoolFree(pool_acc, sensor_data_p);
						APP_ERROR_TEST(status, osOK);
						break;
					
					case Gyr:
						Log("write gyr...");
						axis_p = sensor_data_p->axis_array;
						fprintf(fp_gyr, "%d,%d\r\n", sensor_data_p->timestamp, sensor_data_p->num);
						for ( int n = sensor_data_p->num; n > 0; n--) {
							cnt += fprintf(fp_gyr,"%d,%d,%d\r\n", axis_p->x, axis_p->y, axis_p->z);
							axis_p++;
						}
						
						status = osPoolFree(pool_gyr, sensor_data_p);
						APP_ERROR_TEST(status, osOK);
						break;
					
					case HRV:
						Log("write hrv...");
						hrv_p = (hrv_t*)sensor_data_p;
						fprintf(fp_hrv,"%d,%d\r\n", sensor_data_p->timestamp, hrv_p->num);
						for ( int num = hrv_p->num, i = 0; i < num; i++)
							cnt += fprintf(fp_hrv,"%d\r\n",hrv_p->fifo[i]);
						
						status = osPoolFree(pool_hrv, hrv_p);
						APP_ERROR_TEST(status, osOK);
						break;
					
					default:
						Log(RTT_BG_RED RTT_TEXT_BLACK"[fs]  : Unkown sensor type %d.\n", sensor_data_p->type);
				}
				Log(" %d bytes.\n", cnt);
				cnt = 0;
				break;
				
			default:
				Log("[fs]  : Unknow msg 0x%08X\n", msg);
		}
	}
}


fsStatus fs_get_time (fsTime *ftime) {
  RTC_DateTypeDef date;
  RTC_TimeTypeDef time;

  HAL_RTC_GetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
  
  ftime->hr   = time.Hours;       // Hours:   0 - 23
  ftime->min  = time.Minutes;     // Minutes: 0 - 59
  ftime->sec  = time.Seconds;     // Seconds: 0 - 59
  ftime->day  = date.Date;        // Day:     1 - 31
  ftime->mon  = date.Month;       // Month:   1 - 12
  ftime->year = date.Year + 2000; // Year:    2000-2099
  
  return fsOK;
}
