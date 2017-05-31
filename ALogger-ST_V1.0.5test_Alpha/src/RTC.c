#include <string.h>
#include "rtt_log.h"
#include "stm32f4xx_hal.h"

RTC_HandleTypeDef RTCHandle;

static const char * _month[] =  {  
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};
		

uint8_t str2month( char * str )
{
	int i;
	for ( i = 0 ; i < 12; i++)
		if ( strcmp( _month[i], str) == 0 ) break;
	return i+1;
}

uint8_t dayofweek(uint8_t d, uint8_t m, uint16_t y)
{
    uint8_t t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    y -= m < 3;
    return ( y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
	int status;
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	/* To change the source clock of the RTC feature (LSE, LSI), You have to:
	- Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
	- Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
	configure the RTC clock source (to be done once after reset).
	- Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
	__HAL_RCC_BACKUPRESET_RELEASE().
	- Configure the needed RTC clock source */

	/*##-1- Configue LSE as RTC clock soucre ###################################*/ 
	// oscillate the LSE
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	status = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	APP_ERROR_CHECK(status);
	// select LSE as RTC clock
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	status = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
	APP_ERROR_CHECK(status);

	/*##-2- Enable RTC peripheral Clocks #######################################*/ 
	/* Enable RTC Clock */ 
	__HAL_RCC_RTC_ENABLE(); 
}

void RTC_CalendarConfig(void)
{
	int status;
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	char str[10] = {0};

	sscanf(__DATE__, "%s %d %d", str, (int *)&date.Date, (int *)&date.Year);
	date.Year -= 2000;
	date.Month = str2month(str);
	date.WeekDay = dayofweek(date.Date, date.Month, date.Year + 2000);
	status = HAL_RTC_SetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
	APP_ERROR_CHECK(status);

	time.TimeFormat = RTC_HOURFORMAT12_AM;
	time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	time.StoreOperation = RTC_STOREOPERATION_RESET;

	sscanf(__TIME__, "%d:%d:%d", (int *)&time.Hours, (int *)&time.Minutes, (int *)&time.Seconds);
	status = HAL_RTC_SetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
	APP_ERROR_CHECK(status);   
	/*##-3- Writes a data in a RTC Backup data Register0 #######################*/
	HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR0,0x32F2);  
}

void RTC_CalendarShow(char* showtime, char* showdate)
{
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	/* Get the RTC current Time */
	HAL_RTC_GetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
	/* Display time Format : hh:mm:ss */
	sprintf((char*)showtime,"%02d:%02d:%02d",time.Hours, time.Minutes, time.Seconds);
	/* Display date Format : mm-dd-yy */
	sprintf((char*)showdate,"%02d %02d %02d",date.Month, date.Date, 2000 + date.Year);
} 

void RTC_Init(void) {
	char status;
	RTCHandle.Instance = RTC;
	RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	RTCHandle.Init.AsynchPrediv = 0x7F;
	RTCHandle.Init.SynchPrediv = 0x00FF;
	RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
	RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW;
	RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	status = HAL_RTC_Init(&RTCHandle);
	APP_ERROR_CHECK(status);
	return ;
}
