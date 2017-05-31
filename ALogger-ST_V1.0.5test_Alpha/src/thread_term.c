#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "osObjects.h"
#include "rtt_log.h"

#define _ALEFT  "[D"
#define _ARIGHT "[C"
#define _AUP    "[A"
#define _ADOWN 	"[B"

#define CMD_COUNT   (sizeof (cmd) / sizeof (cmd[0]))
#define SUB_SET_CNT (sizeof(cmd_set_sub) / sizeof(cmd_set_sub[0]))

#define BUSY    1
#define IDLE    0

typedef struct scmd {
	char name[10];
	void (*func)(void *par);
	const char* help;
	const char** subcmd;
} scmd_t;

enum __CMDKEY {
	BACKSPACE = 0x08,
	LF        = 0x0A,   // '\n' opq r
	CLR       = 0x0C,
	CR        = 0x0D,   // '\r'
	CNTLQ     = 0x11,
	CNTLS     = 0x13,
	ESC       = 0x1B,
	DEL       = 0x7F
};

static char __input[80] = {0};


static int getline (char *line, int32_t max_num);
static char *getpara (char *cp, char **pNext);

void cmd_start (void *par);
void cmd_stop (void *par);

static void cmd_set (void *par);
static void cmd_help (void *par);
static void cmd_read (void *par);
static void cmd_write (void *par);

static const scmd_t cmd[] = {
	{"help",    cmd_help,   "帮助 \n"},
	{"set",     cmd_set,    "设置 \n"},
	{"start",   cmd_start,  "Start Acc sample.\n"},
	{"stop",    cmd_stop,   "Stop  Acc sample.\n"},
	{"read",    cmd_read,   "read [Reg] \n"      },
	{"write",   cmd_write,  "write [Reg] [Value] \n"},
	{"",0,""}
};

static const char * cmd_set_sub[] = {
	"pulse",
	"fsample",
	"avg",
	"odr",
	"reg"
};

int16_t cmd_set_value[SUB_SET_CNT] = {0};

void sample_timer_handler (void const *arg)	{
	m_timestamp_per_100ms++;
//	osSignalSet(tid_acc_EvtHandler, FIFO_SIG);
	osSignalSet(tid_gyr_EvtHandler, FIFO_SIG);
	osSignalSet(tid_mag_EvtHandler, FIFO_SIG);
	osSignalSet(tid_hrv_EvtHandler, FIFO_SIG);
}

void cmd_start (void *input)
{
	osEvent evt;
	osStatus status;
	
	m_timestamp_per_100ms = 0;
	uint32_t arg = 0;
	sscanf(input, "%d", &arg);

	osDelay(200);

	status = osMessagePut(msg_fs, THD_MSG(START), 100);
	APP_ERROR_CHECK(status);
	
	evt = osSignalWait(DRDY_SIG, 1000);
	if (evt.status == osEventTimeout) {
		Log("Can't create csv log.\n");
		return;
	}
	osTimerStart(sample_timer_id, 100);
	
//	status = osMessagePut(msg_acc, THD_MSG(START), 0);
//	APP_ERROR_CHECK(status);
	
	status = osMessagePut(msg_gyr, THD_MSG(START), 0);
	APP_ERROR_CHECK(status);
				
	status = osMessagePut(msg_mag, THD_MSG(START), 0);
	APP_ERROR_CHECK(status);

	status = osMessagePut(msg_hrv, THD_MSG(START), 0);
	APP_ERROR_CHECK(status);

	sensor_busy = true;

	if ( arg != 0) {
		status = osTimerStart (term_timer_id, arg * 1000);            
		if (status != osOK) {
			Log(RTT_TEXT_RED   "[Term]: Countdown Timer could not be started.\n");
		} else {
			Log(RTT_TEXT_GREEN "[Term]: Countdown %d second.\n", arg);
		}
		Log("\n");
	}
}

void cmd_stop (void *input)
{
	osStatus status;
	
	osTimerStop(term_timer_id);
	osTimerStop(sample_timer_id);
	
	Log("\n"RTT_TEXT_MAGENTA"Time Up ! Stop collection.\n");
	
//	status = osMessagePut(msg_acc, THD_MSG(STOP), 10);
//	APP_ERROR_CHECK(status);

	status = osMessagePut(msg_gyr, THD_MSG(STOP), 10);
	APP_ERROR_CHECK(status);
	
	status = osMessagePut(msg_mag, THD_MSG(STOP), 10);
	APP_ERROR_CHECK(status);
	
	status = osMessagePut(msg_hrv, THD_MSG(STOP), 10);
	APP_ERROR_CHECK(status);
	
	osDelay(200);
	
	status = osMessagePut(msg_fs, THD_MSG(STOP), 500);
	APP_ERROR_CHECK(status);

	sensor_busy = false;
}

static void cmd_set (void *input)
{
	osStatus stat;
	char *next,*argv;
	uint32_t idx = 0;
	uint32_t payload;
	
	argv = getpara(input, &next);
	for (idx = 0; idx < SUB_SET_CNT ; idx++)
		if ( strcmp(argv, cmd_set_sub[idx]) == 0 )
			break;
	
	if( idx >= SUB_SET_CNT )	{
		Log(RTT_TEXT_YELLOW "Unknow set config.\n"RTT_RESET);
	}	
	else {
		sscanf(next, "%d", &payload);
		cmd_set_value[idx] = payload;
		switch (idx){
			case 0:
				payload &= 0x000000FFL; // pulse  0-255
				Log("Pulse = %d.\n", payload);
				payload = payload << 8 | 0x13 ;
				payload |= 0x36 << 16;
				break;
			case 1:
				payload %= 3400;       // fsample 0-3000
				payload = 8000/payload;
				payload |= 0x12 << 16;
				break;
			case 2:
				Log("Avg = %d.\n", 1 << payload);
				payload = (payload & 0x07L) << 8  ;
				payload |= 0x15 << 16 ;
				break;
			case 3:
				payload *= 1 << cmd_set_value[2];
				Log("fsample = %d\n",payload);
				if ( payload > 3400 )
					Log(RTT_TEXT_RED " Error odr value\n"RTT_RESET);
				payload %= 3400; // fsample 0-3000
				payload = 8000/payload;
				payload |= 0x12<<16;
				break;
			case 4:
				Log("Reg %02X",payload);
				break;
		}
		
		stat = osMessagePut(msg_hrv, THD_MSG(CONFIG)|payload, osWaitForever);
		APP_ERROR_CHECK(stat);
	}	
	
  return;
}
      
static void cmd_help (void *par)
{
  char *next,*cmd_name;
	if (par == NULL ) {
		Log(" Please input cmd name.\n eg. help set\n");
		return;
	}
	else{
		cmd_name = getpara(par, &next);
	}
	
	for (int i = 0; i < CMD_COUNT; i++) {
      if (strcmp ((char*)cmd_name, (const char *)&cmd[i].name)) {
        continue;
      }
      Log("%s", cmd[i].help);                     /* execute command function   */
      break;
	}

  return;
}

static void cmd_read (void *input)
{
//	osStatus stat;
//	char *next,*argv;
//	int idx = 0;
//	uint32_t payload;
//	
////	argv = getpara(input, &next);

//	sscanf(argv, "%d", &payload);
//	
//	cmd_set_value[idx] = payload;
//	
//	stat = osMessagePut(msg_hrv, payload, osWaitForever);
//	APP_ERROR_CHECK(stat);
  return;
}
static void cmd_write (void *input)
{
//	osStatus stat;
//	char *next,*argv;
//	int idx = 0;
//	uint32_t payload;
//	
////	argv = getpara(input, &next);

//	sscanf(argv, "%d", &payload);
//	
//	cmd_set_value[idx] = payload;
//	
//	stat = osMessagePut(msg_hrv, payload, osWaitForever);
//	APP_ERROR_CHECK(stat);
  return;
}

/*-----------------------------------------------------------------------------
 *        Process input string for long or short name entry
 *----------------------------------------------------------------------------*/
static char *getpara (char *cp, char **pNext)  {
  char *sp, lfn = 0, sep_ch = ' ';
  
  if (cp == NULL) {                           /* skip NULL pointers           */
    *pNext = cp;
    return (cp);
  }

  for ( ; *cp == ' ' || *cp == '\"'; cp++) {  /* skip blanks and starting  "  */
    if (*cp == '\"') { sep_ch = '\"'; lfn = 1; }
    *cp = 0;
  }
 
  for (sp = cp; *sp != CR && *sp != LF && *sp != 0; sp++) {
    if ( lfn && *sp == '\"') break;
    if (!lfn && *sp == ' ' ) break;
  }

  for ( ; *sp == sep_ch || *sp == CR || *sp == LF; sp++) {
    *sp = 0;
    if ( lfn && *sp == sep_ch) { sp ++; break; }
  }

  *pNext = (*sp) ? sp : NULL;                 /* next entry                   */
  return (cp);
}



/*------------------------------------------------------------------------------
 *      Line Editor
 *----------------------------------------------------------------------------*/
static int getline (char *line, int32_t max_num)  {
  int32_t cnt = 0;
  char keycode;
	
	while(1) {
		if ( SEGGER_RTT_HasKey() == 1 ) {
			keycode = SEGGER_RTT_GetKey();
			
			switch(keycode){
				case CR:
					*line = keycode;
					break;
				
				case LF:
					*line = keycode;
					cnt++;
					break;
				
				case CLR:
					SEGGER_RTT_printf(0,RTT_CLEAR);
					break;
				
				case DEL:
				case BACKSPACE:
					if ( cnt != 0 ) {
						SEGGER_RTT_printf(0," %c",BACKSPACE);
						*(--line) = 0;
						cnt--;
					} else {
						SEGGER_RTT_printf(0," ");
					}
					break;
				
				default:
					*line++ = keycode;
					cnt++;
			}
			
			if ( keycode == LF )
				return cnt;
		}
		else {
			osDelay(100);
		}
	
	}

}

void thread_term (void const *argument) {
	osEvent evt;
	thd_msg_t type;

	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
  
	msg_term = osMessageCreate(osMessageQ(msg_term), NULL);
	term_timer_id = osTimerCreate (osTimer(term_timer), osTimerOnce, NULL);
	if (term_timer_id == NULL) 
		Log("Term timer not created.\n");
		
	sample_timer_id = osTimerCreate (osTimer(sample_timer), osTimerPeriodic, NULL);
	if (sample_timer_id == NULL) 
		Log("Sample timer not created.\n");

	while (1) {
		evt = osMessageGet(msg_term, osWaitForever);
		type = MSG_TYPE(evt.value.v);
		switch( type ) {
			case START:
				cmd_start(NULL);
				break;
			case STOP:
				cmd_stop(NULL);
				break;
			default:
				break;
		}
	/*
		char *sp,*next;
		int i = 0;

		SEGGER_RTT_printf(0, "A-Logger> ");
			
		memset(__input, 0, sizeof(__input));
			
		if (getline(__input, sizeof(__input)) == false) {
		  continue;
		}

		sp = getpara(__input, &next);

		if (*sp == 0) {
		  continue;
		}

		for (i = 0; i < CMD_COUNT; i++) {
		  if (strcmp (sp, (const char *)&cmd[i].name)) {
			continue;
		  }
				
		  cmd[i].func (next);
		  break;
		}

		if (i == CMD_COUNT) 
			SEGGER_RTT_printf(0, RTT_TEXT_RED" Error: command not found.\n"RTT_RESET);
	*/
	}
}

void term_timer_handler (void const *arg)	{
	cmd_stop(NULL);
}
