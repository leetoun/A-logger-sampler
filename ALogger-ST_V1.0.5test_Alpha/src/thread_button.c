#include "osObjects.h"
#include "rtt_log.h"

#include "stm32f4xx_hal.h"

#ifdef  ST_DEMO
#define BUTTON_PORT GPIOC
#define BUTTON_PIN  GPIO_PIN_0
#else
#define BUTTON_PORT GPIOA
#define BUTTON_PIN  GPIO_PIN_0
#endif

#define BUTTON_DEBOUNCE   100			//延时去抖动
extern uint8_t usb_connected;
extern uint8_t get_lcd_stat_pwr(void);
extern void set_lcd_stat_pwr(uint8_t pwr);
extern uint8_t get_lcd_stat_tick(void);
extern void set_lcd_stat_tick(uint8_t cnt);

void button_timer_handler (void const *arg) {
	static uint8_t button_state = 0;

	set_lcd_stat_tick(0);

	if (get_lcd_stat_pwr() == 1) {
		if ( 0 == HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) && !usb_connected  ) {
			if (button_state ^= 1)
				osMessagePut(msg_term, THD_MSG(START), 0);
			else
				osMessagePut(msg_term, THD_MSG(STOP), 0);
		} else {
			Log("[button] : Fake press.\n");
		}
	} else {
		osMessagePut(msg_gui, LCD_ON, 0);
	}
}

void thread_button (const void * par) {
	osStatus status;
	osEvent  evt;
	
	GPIO_InitTypeDef InitStruct;
	InitStruct.Pin   = BUTTON_PIN;
	InitStruct.Mode  = GPIO_MODE_IT_FALLING;
	InitStruct.Pull  = GPIO_NOPULL;
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BUTTON_PORT,&InitStruct);		       /* Init KEY pin EXTI   */ 
	
	HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ  (EXTI0_IRQn);
	
	button_timer_id = osTimerCreate (osTimer(button_timer), osTimerOnce, NULL);
	if (button_timer_id == NULL) {
		Log("Button timer not created.\n");
	}
	
	osDelay(100);	//为什么要延时100ms？
	
	while (1) {
		evt = osSignalWait(ANY_SIG, osWaitForever);		
		
		switch (evt.value.signals) {
			case DRDY_SIG:
				status = osTimerStart (button_timer_id, BUTTON_DEBOUNCE);	//定时器100ms去抖动
				if (status != osOK) {
					Log(RTT_BG_RED"[button] : Button timer could not be started.\n");
				}
				break;
			
			default:
				Log(RTT_BG_RED"[button] : Unknow Signal !\n");

		}
	}
}
