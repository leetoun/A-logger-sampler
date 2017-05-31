#include <stdio.h>
#include "osObjects.h"
#include "GUI.h"
#include "rtt_log.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif

/*----------------------------------------------------------------------------
 *      thread_gui: GUI Thread for Single-Task Execution Model
 *---------------------------------------------------------------------------*/\
#define TIMEOUT_TICKS  50               // timeout = 5s (100ms per tick)

typedef struct {
	uint8_t pwr;
	int8_t  tick;
} lcd_stat_t;

extern uint32_t sample_id;
static char o_buffer[40];

static lcd_stat_t m_lcd_status;

uint8_t get_lcd_stat_pwr()
{
	return m_lcd_status.pwr;
}

void set_lcd_stat_pwr(uint8_t pwr)
{
	m_lcd_status.pwr = pwr;
}

uint8_t get_lcd_stat_tick()
{
	return m_lcd_status.tick;
}

void set_lcd_stat_tick(uint8_t cnt)
{
	m_lcd_status.tick = cnt;
}

void lcd_timer_handler(void const *arg)
{
	lcd_stat_t* pstat = (lcd_stat_t*)arg;

	if (pstat->tick > TIMEOUT_TICKS)
		osMessagePut(msg_gui, LCD_OFF, 0);
	else
		osMessagePut(msg_gui, LCD_REFRESH, 0);

	pstat->tick++;
}

void thread_gui (void const *argument) {
	osEvent evt;
	osStatus status;

	GUI_Init();                     /* Initialize the Graphics Component */
	
	GUI_Clear();
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_8X15B_ASCII);
	GUI_DispStringAt("A-Logger", 0, 0);

	msg_gui = osMessageCreate(osMessageQ(msg_gui), NULL);
	lcd_timer_id = osTimerCreate(osTimer(lcd_timer), osTimerPeriodic, &m_lcd_status);
	if ( lcd_timer_id != NULL) {
		status = osTimerStart(lcd_timer_id, 100);
		if ( status != osOK )
			Log(RTT_TEXT_RED "[GUI]: LCD Timer can't be started.\n");
	} else {
		Log(RTT_TEXT_RED "[GUI]: LCD Timer can't be created.\n");
	}

	while (1) {
		evt = osMessageGet(msg_gui, osWaitForever);
		switch ( evt.value.v ) {
			case LCD_REFRESH:
				GUI_SetFont(GUI_FONT_D24X32);
				GUI_GotoXY(6, 24);
				GUI_DispDecMin(sample_id);
				
				sprintf(o_buffer, "%2d:%02d:%02d.%d", m_timestamp_per_100ms / 36000,
													  m_timestamp_per_100ms / 10 % 3600 / 60,
													  m_timestamp_per_100ms / 10 % 60,
													  m_timestamp_per_100ms % 10);
				GUI_SetFont(GUI_FONT_6X8_ASCII);
				GUI_DispStringAt(o_buffer, 0, 70);
				break;
			
			case LCD_OFF:
				osTimerStop(lcd_timer_id);
				LCD_X_DisplayDriver(0, LCD_X_OFF, NULL);
				m_lcd_status.pwr = 0;
				break;

			case LCD_ON:
				osTimerStart(lcd_timer_id, 100);
				LCD_X_DisplayDriver(0, LCD_X_ON, NULL);
				m_lcd_status.pwr = 1;
				break;
		}
  }
}

