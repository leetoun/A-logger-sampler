/*----------------------------------------------------------------------------
 * osObjects.h: CMSIS-RTOS global object definitions for an application
 *----------------------------------------------------------------------------
 *
 * This header file defines global RTOS objects used throughout a project
 *
 * #define osObjectsPublic indicates that objects are defined; without that
 * definition the objects are defined as external symbols.
 *
 *--------------------------------------------------------------------------*/

#ifndef __osObjects
#define __osObjects

// global Signal defination
#define ANY_SIG         (0)
#define DRDY_SIG        (1<<8)
#define FIFO_SIG        (1<<9)

// global Message defination
#define THD_MSG(x)      ((uint32_t)x << 24)
#define MSG_TYPE(x)     ((thd_msg_t)(x >> 24))

typedef enum {
	// IO CTRL
	STOP                = 0x01,
	START               = 0x02,
	
	// INT 
	DATA_RDY            = 0x10,
	FIFO_FULL           = 0x11,
	FIFO_RD             = 0x12,
	
	// Reciver
	MEMBLCK             = 0x20,
	
	// Configure
	CONFIG              = 0x30
} thd_msg_t;

#if (!defined (osObjectsPublic))
	#define osObjectsExternal           // define RTOS objects with extern attribute
#endif

#include "cmsis_os.h"               // CMSIS RTOS header file

#ifdef osObjectsExternal
	extern uint32_t m_timestamp_per_100ms;
	extern uint8_t sensor_busy;
	extern uint8_t usb_connected;

#else
	uint32_t m_timestamp_per_100ms;
	uint8_t sensor_busy;
	uint8_t usb_connected;
#endif

// global 'timer' functions ----------------------------------------------------

#ifdef osObjectsExternal
	extern osTimerId button_timer_id;
	extern osTimerId sample_timer_id;
	extern osTimerId term_timer_id;
	extern osTimerId lcd_timer_id;
#else
	osTimerId button_timer_id;
	osTimerId sample_timer_id;
	osTimerId term_timer_id;
	osTimerId lcd_timer_id;
#endif

extern void term_timer_handler (void const *arg);
extern void button_timer_handler (void const *arg);
extern void sample_timer_handler (void const *arg);
extern void lcd_timer_handler (void const *arg);

osTimerDef (term_timer,     term_timer_handler);
osTimerDef (button_timer,   button_timer_handler);
osTimerDef (sample_timer,   sample_timer_handler);
osTimerDef (lcd_timer,      lcd_timer_handler);

// global 'thread' functions ---------------------------------------------------

extern void thread_fs(void const *argument);
extern void thread_term(void const *argument);
extern void thread_button(void const *argument);
extern void thread_gui (void const *argument);

extern void driver_acc(void const *argument);
extern void driver_gyr(void const *argument);
extern void driver_mag(void const *argument);
extern void driver_hrv(void const *argument);

extern void acc_EvtHandler(void const *argument);
extern void gyr_EvtHandler(void const *argument);
extern void mag_EvtHandler(void const *argument);
extern void hrv_EvtHandler(void const *argument);

extern osThreadId tid_spi1x;
extern osThreadId tid_spi2x;
extern osThreadId tid_i2c1x;

#ifdef osObjectsExternal
	extern osThreadId tid_main;
	
	extern osThreadId tid_acc;
	extern osThreadId tid_acc_EvtHandler;
	extern osThreadId tid_gyr;
	extern osThreadId tid_gyr_EvtHandler;
	extern osThreadId tid_mag;
	extern osThreadId tid_mag_EvtHandler;
	extern osThreadId tid_hrv;
	extern osThreadId tid_hrv_EvtHandler;

	extern osThreadId tid_fs;
	extern osThreadId tid_term;
	extern osThreadId tid_button;
	extern osThreadId tid_gui;

#else
	osThreadId tid_main;

	osThreadId tid_acc;
	osThreadId tid_acc_EvtHandler;
	osThreadId tid_gyr;
	osThreadId tid_gyr_EvtHandler;
	osThreadId tid_mag;
	osThreadId tid_mag_EvtHandler;
	osThreadId tid_hrv;
	osThreadId tid_hrv_EvtHandler;

	osThreadId tid_fs;
	osThreadId tid_term;
	osThreadId tid_button;
	osThreadId tid_gui;
#endif

osThreadDef (thread_gui,     osPriorityIdle,        1,  2048);
osThreadDef (thread_button,  osPriorityLow,         1,  512);
osThreadDef (thread_fs  ,    osPriorityBelowNormal, 1,  1024);
osThreadDef (thread_term,    osPriorityAboveNormal, 1,  512);

osThreadDef (driver_acc,     osPriorityNormal,      1,  512);
osThreadDef (driver_gyr,     osPriorityNormal,      1,  512);
osThreadDef (driver_mag,     osPriorityNormal,      1,  512);
osThreadDef (driver_hrv,     osPriorityNormal,      1,  512);

osThreadDef (acc_EvtHandler,    osPriorityHigh,        1,  256);
osThreadDef (gyr_EvtHandler,    osPriorityHigh,        1,  256);
osThreadDef (mag_EvtHandler,    osPriorityHigh,        1,  256);
osThreadDef (hrv_EvtHandler,    osPriorityHigh,        1,  256);

// global 'mutex' ---------------------------------------------------------------

#ifdef osObjectsExternal
	extern osMutexId mutex_sdcard;
	extern osMutexId mutex_spi1;
	extern osMutexId mutex_spi2;
	extern osMutexId mutex_i2c1;
#else
	osMutexId mutex_sdcard;
	osMutexId mutex_spi1;
	osMutexId mutex_spi2;
	osMutexId mutex_i2c1;
#endif

osMutexDef (mutex_sdcard);
osMutexDef (mutex_spi1);
osMutexDef (mutex_spi2);
osMutexDef (mutex_i2c1);

// global 'semaphores' ----------------------------------------------------------
/* 
Example:
osSemaphoreId sid_sample_name;                          // semaphore id
osSemaphoreDef (sample_name);                           // semaphore object
*/

/*******************************************************************************/
typedef enum {
	LCD_REFRESH,
	LCD_ON,
	LCD_OFF
} lcd_cmd_t;


typedef enum {
  Acc = 1,
  Gyr = 2,
  HRV = 3,
  Mag = 4,
} sensor_t;

typedef struct {
	sensor_t type;
	uint32_t timestamp;
	uint8_t  num;                                            // num of sample (uint16_t)
	uint16_t fifo[10];
} hrv_t;

typedef struct {
	int16_t x; 
	int16_t y; 
	int16_t z;
} axis_t;

typedef struct {
	sensor_t type;
	axis_t   axis;
} xyz_format_t;

typedef struct {
	sensor_t type;
	uint32_t timestamp;
	uint8_t  num;											// num of sample (axis_t)
	axis_t   axis_array[25];
} raw_3axis_block_t;

typedef struct {
	uint8_t  num;
	uint16_t pad;
	axis_t   buffer[50];
} fifo_t;

// global 'memory pools' --------------------------------------------------------
#ifdef osObjectsExternal
	extern osPoolId pool_acc;
	extern osPoolId pool_gyr;
	extern osPoolId pool_mag;
	extern osPoolId pool_hrv; 
#else
	osPoolId pool_acc;
	osPoolId pool_gyr;
	osPoolId pool_mag;
	osPoolId pool_hrv;
#endif

osPoolDef (pool_acc,  6, raw_3axis_block_t);
osPoolDef (pool_gyr,  6, raw_3axis_block_t);
osPoolDef (pool_mag,  6, raw_3axis_block_t);
osPoolDef (pool_hrv,  5, hrv_t);


// global 'message queues' -------------------------------------------------------
#ifdef osObjectsExternal
	extern osMessageQId  msg_term;
	extern osMessageQId  msg_gui;
	extern osMessageQId  msg_fs;
	extern osMessageQId  msg_hrv;
	extern osMessageQId  msg_acc;
	extern osMessageQId  msg_gyr;
	extern osMessageQId  msg_mag;
#else
	osMessageQId  msg_term;
	osMessageQId  msg_gui;
	osMessageQId  msg_fs;
	osMessageQId  msg_hrv;
	osMessageQId  msg_acc;
	osMessageQId  msg_gyr;
	osMessageQId  msg_mag;
#endif

osMessageQDef (msg_term,   4, int);
osMessageQDef (msg_fs,    16, int);
osMessageQDef (msg_gui,    4, int);
osMessageQDef (msg_hrv,    4, int);
osMessageQDef (msg_acc,    4, int);
osMessageQDef (msg_gyr,    4, int);
osMessageQDef (msg_mag,    4, int);


// global 'mail queues' ----------------------------------------------------------


#ifdef osObjectsExternal
  extern osMailQId  fs_mail;
#else
  osMailQId  fs_mail;
#endif

osMailQDef(fs_mail, 4, xyz_format_t);											//define the mailbox 

#endif  // __osObjects
