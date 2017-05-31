/*------------------------------------------------------------------------------
 * AM 5 Logger 
 * Copyright (c) 2004-2014 Andon All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    main.c
 * Purpose: USB Device Human Interface Device example program
 * Version: 0.1
 *----------------------------------------------------------------------------*/
 
#include <stdio.h>                      /* Standard I/O .h-file               */
#include <string.h>
#include <time.h>
#include "rl_fs.h"                      /* FileSystem definitions             */
#include "rl_usb.h"                     /* RL-USB function prototypes         */
#include "rtt_log.h"					            /* JLink RT-Terminal functions		  */
#include "EventRecorder.h"              // Keil::Compiler:Event Messaging

#include "stm32f4xx_hal.h"

#include "USBD_MSC_0.h"                     // Media ownership control for USB Device
#include "RTC.h"
#include "SPI.h"
#include "I2C.h"

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions

#define CONNECT           (1<<0)
#define DISCONNECT        (1<<1)

extern uint32_t os_time;
extern uint32_t sample_id;

extern FILE *fp_mag;
extern FILE *fp_acc;
extern FILE *fp_gyr;
extern FILE *fp_hrv;

extern char file_name[20];

uint32_t HAL_GetTick(void) {
  return os_time;
}

/**
  * @brief  System Clock Configuration
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 64,000,000
  *            HCLK(Hz)                       = 64,000,000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 16,000,000
  *            PLL_M                          = 16
  *            PLL_N                          = 384
  *            PLL_P                          = 6
  *            PLL_Q                          = 8
  *            PLL_R                          = 6
  *            VDD(V)                         = 3.0
  *            Main regulator output voltage  = Scale2 mode
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void) {
  int error_code;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = HSE_VALUE/1000000;
  RCC_OscInitStruct.PLL.PLLN = 384;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV6;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  error_code = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  APP_ERROR_CHECK(error_code);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  
  error_code = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
  APP_ERROR_CHECK(error_code);
}


int main (void) {
	uint8_t status;
	osEvent evt;
	char system_time[20] = {0};
	char system_date[20] = {0};

	HAL_Init();
	SystemClock_Config();

	EventRecorderInitialize (EventRecordAll, 1);

	Log("\n"RTT_CLEAR);
	Log("Compiled  %s %s \n", __DATE__, __TIME__);
	Log("Core\t HCLK\t PCLK2\t PCLK1\n%dMh\t %dMh\t %dMh\t %dMh \n",
		HAL_RCC_GetSysClockFreq() / 1000000,
		HAL_RCC_GetHCLKFreq()	  / 1000000,
		HAL_RCC_GetPCLK2Freq()	  / 1000000,
		HAL_RCC_GetPCLK1Freq()	  / 1000000);

	RTC_Init();
	RTC_CalendarConfig();
	RTC_CalendarShow(system_time, system_date);
	Log("RTC time: %s %s\n",system_date, system_time);

	/*     Init GPIO A            */ 
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef InitStruct;
	InitStruct.Pin   = GPIO_PIN_8;
	InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	InitStruct.Pull  = GPIO_NOPULL;
	InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA,&InitStruct);		             /* Init SD VCC Enable pin  */
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, GPIO_PIN_SET);
  
	InitStruct.Pin   = GPIO_PIN_3;
	InitStruct.Mode  = GPIO_MODE_INPUT;
	InitStruct.Pull  = GPIO_PULLUP;
	InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA,&InitStruct);		             /* Init Charger PPR pin    */
	
	/*      Enable EXTI           */
	HAL_NVIC_SetPriority(EXTI15_10_IRQn,4,0);
	HAL_NVIC_EnableIRQ  (EXTI15_10_IRQn);
	
	HAL_NVIC_SetPriority(EXTI9_5_IRQn,4,0);
	HAL_NVIC_EnableIRQ  (EXTI9_5_IRQn);
	
	HAL_NVIC_SetPriority(EXTI4_IRQn,4,0);
	HAL_NVIC_EnableIRQ  (EXTI4_IRQn);
	
	SPI_Init(&Driver_SPI1, ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_MSB_LSB |
            ARM_SPI_SS_MASTER_UNUSED | ARM_SPI_DATA_BITS(8), 8000000, SPI1_EvtHandler);

	SPI_Init(&Driver_SPI2, ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB |
            ARM_SPI_SS_MASTER_UNUSED | ARM_SPI_DATA_BITS(8), 8000000, SPI2_EvtHandler);
		
	I2C_Init(&Driver_I2C1, ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST, NULL);

	Log("Initialize USB driver...\t");
	status  = USBD_Initialize    (0);
	status += USBD_Connect       (0);
	APP_ERROR_TEST(status, usbOK);

	Log("Initialize File system...\t");
//  finit() has been called by USBD_Initilaize() before we got this line; 
	status  = finit("M0:");
	status += fmount("M0:");
	APP_ERROR_TEST(status, fsOK);
  
//  Read media information of actual media
	fsMediaInfo media_info;
	if (fs_ioc_read_info(0, &media_info) != fsOK)
		Log("No SD Card found.\n");
	else 
		Log("Free capacity:\t%d MB\n", ffree("M:")>>20);

	Log("sizeof enum = %d byte(s)\n", sizeof(thd_msg_t));
	
	osDelay( 1 * 1000 );	//无事件运行时，进入低功耗。

	mutex_spi1   = osMutexCreate(osMutex(mutex_spi1));
	mutex_spi2   = osMutexCreate(osMutex(mutex_spi2));
	mutex_i2c1   = osMutexCreate(osMutex(mutex_i2c1));
	mutex_sdcard = osMutexCreate(osMutex(mutex_sdcard));
	
	tid_main = osThreadGetId();
	
	tid_fs = osThreadCreate (osThread (thread_fs), NULL);
	if (!tid_fs )  Log("couldn't spawn thread_fs.\n");
	
	osDelay(200);

	//tid_acc = osThreadCreate (osThread (driver_acc), NULL);
	//if (!tid_acc )  Log("couldn't spawn acc driver thd.\n");

	tid_mag = osThreadCreate (osThread (driver_mag), NULL);
	if (!tid_mag )  Log("couldn't spawn mag driver thd.\n");

	tid_gyr = osThreadCreate (osThread (driver_gyr), NULL);
	if (!tid_gyr )  Log("couldn't spawn gyr driver thd.\n");

	tid_hrv = osThreadCreate (osThread (driver_hrv), NULL);
	if (!tid_hrv )  Log("couldn't spawn hrv driver thd.\n");

	tid_term = osThreadCreate (osThread (thread_term), NULL);
	if (!tid_term )		Log("couldn't spawn term thd.\n");

	tid_gui = osThreadCreate (osThread(thread_gui), NULL);
	if (!tid_gui) Log("couldn't spawn GUI thd.\n");

	tid_button = osThreadCreate (osThread (thread_button), NULL);
	if (!tid_button ) 	Log("couldn't spawn button thd.\n");

	
	while(1)
		osSignalWait(0xFF, osWaitForever);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
		case GPIO_PIN_12:
			break;
		
		case GPIO_PIN_11:
//      	osSignalSet(tid_gyr_EvtHandler, DRDY_SIG);
			break;
		
		case GPIO_PIN_7:                              /* Mag DRDY  pin = PC7  */
		case GPIO_PIN_1:
			osSignalSet(tid_mag_EvtHandler, DRDY_SIG);
			break;
    
		case GPIO_PIN_5:                              /* ADPD INT  pin = PB5  */
			osSignalSet(tid_hrv_EvtHandler, DRDY_SIG);
			break;
			
		case GPIO_PIN_4:                              /* Acc INT 1 pin = PC4  */
//			osSignalSet(tid_acc_EvtHandler, DRDY_SIG);
			break;
					
		case GPIO_PIN_0:							  /* Button 0  pin = PA0  */
			osSignalSet(tid_button, DRDY_SIG);
			break;
			
		default:
			Log("Unkown EXTI !\n");
	}
}

void assert_failed(uint8_t* file, uint32_t line) {
	Log(RTT_CLEAR);
	Log(" Error @ %s : %d \n", file, line);
}
