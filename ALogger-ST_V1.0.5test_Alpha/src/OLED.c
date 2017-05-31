#include "osObjects.h"
#include "stm32f4xx_hal.h"
#include "Driver_SPI.h"
#include "OLED.h"
#include "rtt_log.h"

#define SEND_DONE     (1<<0)

#ifdef  ST_DEMO
#define OLED_EN_PORT		  GPIOA
#define OLED_EN_PIN		    GPIO_PIN_11

#define OLED_DC_PORT  		GPIOB
#define OLED_DC_PIN  		  GPIO_PIN_4

#define OLED_CS_PORT  		GPIOB
#define OLED_CS_PIN  		  GPIO_PIN_8

#define OLED_RST_PORT		  GPIOA
#define OLED_RST_PIN		  GPIO_PIN_9

#else

#define OLED_EN_PORT		  GPIOB
#define OLED_EN_PIN		    GPIO_PIN_12

#define OLED_DC_PORT  		GPIOB
#define OLED_DC_PIN  		  GPIO_PIN_11

#define OLED_CS_PORT  		GPIOB
#define OLED_CS_PIN  		  GPIO_PIN_1

#define OLED_RST_PORT		  GPIOB
#define OLED_RST_PIN		  GPIO_PIN_2

#endif  /* end ST_DEMO */

#define BUS_ACQUIRE(x)  osMutexWait(mutex_##x, osWaitForever)
#define BUS_RELEASE(x)  osMutexRelease(mutex_##x)

#define BUS_A()  BUS_ACQUIRE(spi2)
#define BUS_R()  BUS_RELEASE(spi2)

#define OLED_CS_L()                                                            \
				do {                                                                   \
					BUS_A();                                                             \
				  tid_spi2x = osThreadGetId();                                         \
					OLED_CS_PORT->BSRR = (uint32_t)OLED_CS_PIN << 16;                    \
				} while(0)
				
#define OLED_CS_H()                                                            \
				do {                                                                   \
					OLED_CS_PORT->BSRR = OLED_CS_PIN;                                    \
					BUS_R();                                                             \
				} while(0)
				
#define OLED_A0_L()                                                            \
				OLED_DC_PORT->BSRR = (uint32_t)OLED_DC_PIN << 16
				
#define OLED_A0_H()                                                            \
				OLED_DC_PORT->BSRR = OLED_DC_PIN
				

extern ARM_DRIVER_SPI Driver_SPI2;
static ARM_DRIVER_SPI* SPIdrv = &Driver_SPI2;

static uint8_t page_column_need_update;
static uint8_t page_column_cmd_buffer[3];

void _Write8_A0(uint8_t command)
{
	if ( command >> 4 == 0xB ) {                          // Set page 
		page_column_cmd_buffer[0] = command;
		page_column_need_update = 1;
	}
	else if ( command >> 4 == 1 ) {                       // Set column H
		page_column_cmd_buffer[1] = command;
		page_column_need_update = 2;
	}
	else if ( command >> 4 == 0 ) {                       // Set column L
		page_column_cmd_buffer[2] = command;
		page_column_need_update = 3;
	}
	else {                                                // other cmds
		OLED_CS_L();
		OLED_A0_L();
		SPIdrv->Send(&command,1);
		osSignalWait(SEND_DONE, osWaitForever);
		OLED_CS_H();
	}
}

void _Write8_A1(uint8_t dat)
{
	OLED_CS_L();
	OLED_A0_H();
	SPIdrv->Send(&dat, 1);
	osSignalWait(SEND_DONE, osWaitForever);
	OLED_CS_H();
}

void _WriteM8_A0(uint8_t * pdata, int num)
{
	OLED_CS_L();
	OLED_A0_L();
	SPIdrv->Send(pdata, num);
	osSignalWait(SEND_DONE, osWaitForever);
	OLED_CS_H();
}

void _WriteM8_A1(uint8_t * pdata, int num)
{
	if ( page_column_need_update != false ) {
		_WriteM8_A0(page_column_cmd_buffer, page_column_need_update);
		page_column_need_update = false;
	}
	OLED_CS_L();
	OLED_A0_H();
	SPIdrv->Send(pdata, num);
	osSignalWait(SEND_DONE, osWaitForever);
	OLED_CS_H();
}

uint8_t _Read8_A1(uint8_t dat)
{
	OLED_CS_L();
	OLED_A0_H();
	//TBD
	//SPIdrv->Send(&dat,1);
	//osSignalWait(SEND_DONE, osWaitForever);
	OLED_CS_H();
	return 1;
}



/*********************************************************************
* @fn      Set_Start_Column_Address
*
* @brief   set start address of column
*
* @param   uint8_t addr
*
* @return  void
*/
void Set_Start_Column_Address(uint8_t addr)//??????????,???????????       2?
{
	
	// set lower column start address for Page addressing mode, Default = 0x00
	_Write8_A0(0x00 + addr % 16);

	// set higher column start address for page addressing mode, Default = 0x10
	_Write8_A0(0x10 + addr / 16);
}

/*********************************************************************
* @fn      Set_DCDC_Volt
*
* @brief   Set DC-DC voltage output value
*
* @param   uint8_t param 00:6.4V;01:7.4V;10:8V;11:9V
*
* @return  void
*/
void Set_DCDC_Volt(uint8_t param)//??DC-DC??,?????????????????           2?
{
	_Write8_A0(0x30 | param);
}

/*********************************************************************
* @fn      Set_Start_Line_Address
*
* @brief   set start address of line
*
* @param   uint8_t line_num
*
* @return  void
*/
void Set_Start_Line_Address(uint8_t line_num)//??????????                            2?
{
	_Write8_A0(0x40 | line_num);
}

/*********************************************************************
* @fn      Set_Contrast_Control
*
* @brief   set contrast
*
* @param   uint8_t step
*
* @return  void
*/
void Set_Contrast_Control(uint8_t step)//????????????????                   2?
{
	uint8_t tx[2] = {0x81, step};
	
	_WriteM8_A0(tx, 2);
}

/*********************************************************************
* @fn      Set_Segment_Remap
*
* @brief   Remap segment
*
* @param   uint8_t param  0x01:left 0x00:right
*
* @return  void
*/
void Set_Segment_Remap(uint8_t param)//???????????????                     2?
{
	// 0xa0: column 0 mapped to SEG18;  0xa1:  column 0 mapped to SEG113
	_Write8_A0(0xA0 | param);
}

/*********************************************************************
* @fn      Set_Entire_Display
*
* @brief   set entire display 0x00 normal display 0x01 entire display
*
* @param   uint8_t param
*
* @return  void
*/
void Set_Entire_Display(uint8_t param)//??????,????????????????????? 2?
{
	// 0xA4: Normal display;  0xA5: entire display on
	_Write8_A0(0xA4 | param);
}

/*********************************************************************
* @fn      Set_Inverse_Display
*
* @brief   set inverse display  0x00: normal display;  0x01: inverse display
*
* @param   uint8_t param
*
* @return  void
*/
void Set_Inverse_Display(uint8_t param)//?????????????????????????      2?
{
	// 0xA6: normal display;  0xA7: inverse display
	_Write8_A0(0xA6 | param);
}

/*********************************************************************
* @fn      Set_Multiplex_Ratio
*
* @brief   Set multiplex ratio
*
* @param   uint8_t ratio
*
* @return  void
*/
void Set_Multiplex_Ratio(uint8_t ratio)
{
	uint8_t tx[2] = {0xA8, ratio};
	// default = 0x3f(1/64 duty)
	_WriteM8_A0(tx, 2);
}

/*********************************************************************
* @fn      Set_Charge_Pump
*
* @brief   Set charge pump inside or outside 0x00 DC-DC OFF 0x01 ON
*
* @param   uint8_t param
*
* @return  void
*/
void Set_Charge_Pump(uint8_t param)
{
	uint8_t tx[2] = {0xAD, 0x8A|param};
	_WriteM8_A0(tx, 2);
}

/*********************************************************************
* @fn      Set_Display_Power
*
* @brief   Display NO or OFF 0x00: display off;   0x01: display on
*
* @param   uint8_t param
*
* @return  void
*/
void Set_Display_Power(uint8_t param)
{
	HAL_GPIO_WritePin(OLED_DC_PORT, OLED_DC_PIN, GPIO_PIN_SET);
	// 0xAE: display off;   0xAF: display on
	_Write8_A0(0xAE | param);
}

/*********************************************************************
* @fn      Set_Page_Address
*
* @brief   specifies page address
*
* @param   uint8_t addr
*
* @return  void
*/
void Set_Page_Address(uint8_t addr)//?????                                      2?
{
	// set page start address for page addressing mode, Default = 0xb0
	_Write8_A0(0xB0 | addr);
}

/*********************************************************************
* @fn      Set_Common_Remap
*
* @brief   set common output scan  0xC0: scan from COM0 to 63;  0xC8: scan from COM63 to 0
*
* @param   uint8_t param
*
* @return  void
*/
void Set_Common_Remap(uint8_t param)//??????????????????            2?
{
	// set COM output scan direction
	// 0xC0: scan from COM0 to 63;  0xC8: scan from COM63 to 0
	_Write8_A0(0xC0 | param);
}

/*********************************************************************
* @fn      Set_Display_Offset
*
* @brief   set display offset   offset COM0 - COM63
*
* @param   offset
*
* @return  void
*/
void Set_Display_Offset(uint8_t offset)//??????/??                             2?
{
	uint8_t tx_data[2] = {0xD3, offset};
	_WriteM8_A0(tx_data, 2);
}

/*********************************************************************
* @fn      Set_Display_Clock
*
* @brief   Set display divide ratio oscillator frequency
*
* @param   uint8_t ratio
*
* @return  void
*/
void Set_Display_Clock(uint8_t ratio)
{
	uint8_t tx_data[2] = {0xD5, ratio};
	_WriteM8_A0(tx_data, 2);
	// set display clock divide ratio / oscillator frequency
	// Default = 0x50
	// D[3:0] = Display Clock Divider
	// D[7:4] = Oscillator Frequency
}

/*********************************************************************
* @fn      Set_Precharge_Period
*
* @brief   set discharge or precharge period
*
* @param   uint8_t param
*
* @return  void
*/
void Set_Precharge_Period(uint8_t param)//???????                                2?
{
	// set Pre-Charge Period
	// Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
	// D[3:0] = Phase 1 Period in 1~15 Display Clocks
	// D[7:4] = Phase 2 Period in 1~15 Display Clocks
	uint8_t tx_data[2] = {0xD9, param};
	_WriteM8_A0(tx_data, 2);
}

/*********************************************************************
* @fn      Set_Hardware_Config
*
* @brief   set hardware configure
*
* @param   uint8_t param
*
* @return  void
*/
void Set_Hardware_Config(uint8_t param)//??????                                  2?
{

	uint8_t tx_data[2] = {0xDA, 0x02 | param};
	_WriteM8_A0(tx_data, 2);

}

/*********************************************************************
* @fn      Set_VCOM_Deslt
*
* @brief   set VCOM deselect level
*
* @param   uint8_t level
*
* @return  void
*/
void Set_VCOM_Deslt(uint8_t level)//??????????????????                2?
{
	uint8_t tx_data[2] = {0xDB, level};
	_WriteM8_A0(tx_data, 2);
}

/*********************************************************************
* @fn      Set_RdMdWr_Start
*
* @brief   start set read modify write
*
* @param   void
*
* @return  void
*/
void Set_RdMdWr_Start(void)//??????                                             2?
{
	_Write8_A0(0xE0);
}

/*********************************************************************
* @fn      Set_RdMdWr_End
*
* @brief   finish set read modify write
*
* @param   void
*
* @return  void
*/
void Set_RdMdWr_End(void)//??????                                               2?
{
	_Write8_A0(0xEE);
}

/*********************************************************************
* @fn      Set_NOP
*
* @brief   Nop Operation Command
*
* @param   void
*
* @return  void
*/
void Set_NOP(void)//??????                                                       2?
{
	_Write8_A0(0xE3);
}

/*********************************************************************
* @fn      init_oled
*
* @brief   Init driver of oled
*
* @param   dat
*
* @return  void
*/
void init_oled(void)
{	
	GPIO_InitTypeDef InitStruct;
	InitStruct.Pin   = OLED_EN_PIN;
	InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;		//20170518
	InitStruct.Pull  = GPIO_NOPULL;
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(OLED_EN_PORT, &InitStruct);
	
	InitStruct.Pin   = OLED_CS_PIN;
	InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;		//20170518
	InitStruct.Pull  = GPIO_NOPULL;
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(OLED_CS_PORT, &InitStruct);
	
	InitStruct.Pin   = OLED_DC_PIN;
	InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;		//20170518
	InitStruct.Pull  = GPIO_NOPULL;
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(OLED_DC_PORT, &InitStruct);
	
	InitStruct.Pin   = OLED_RST_PIN;
	InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	InitStruct.Pull  = GPIO_NOPULL;
	InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(OLED_RST_PORT, &InitStruct);
	
	HAL_GPIO_WritePin(OLED_EN_PORT, OLED_EN_PIN, GPIO_PIN_SET);		//DC-DC//P5OUT |= (BIT2);//20130507-20//??
	osDelay(5);
	HAL_GPIO_WritePin(OLED_RST_PORT, OLED_RST_PIN, GPIO_PIN_RESET);		//rst
	osDelay(20);
	HAL_GPIO_WritePin(OLED_RST_PORT, OLED_RST_PIN, GPIO_PIN_SET);
	
	Set_Display_Power(DISPLAY_OFF);  //Set Display Off
	Set_Display_Power(DISPLAY_ON);       //litao
	
	Set_Display_Clock(0xF0);          //display divide ratio/osc. freq. mode
	Set_Multiplex_Ratio(63);        //multiplex ration mode
	
	Set_Display_Offset(0x00);         //Set Display Offset
	Set_Start_Line_Address(0x00);     //Set Display Start Line
	
	Set_Charge_Pump(OUTSIDE_CHARGER);  //DC-DC Mode Set
	
	Set_Segment_Remap(SEGMENT_REMAP_LEFT);          //Segment Remap
	Set_Common_Remap(COMMON_REMAP_DOWN);           //Set COM Output Scan Direction
	
	Set_Hardware_Config(0x10);        //common pads hardware: alternative
	Set_Contrast_Control(0x40);       //contrast control
	Set_Precharge_Period(0x22);	      //set pre-charge period
	Set_VCOM_Deslt(0x40);             //VCOM deselect level mode
	
	Set_Entire_Display(NORMAL_DISPLAY);           //Set Entire Display On/Off
	Set_Inverse_Display(NORMAL_DISPLAY);          //Set Normal Display
	
	Set_Display_Power(DISPLAY_ON);       //Set Display On  20130514-28
	osDelay(20);  
}

void ShutDown_Oled(void)                                                      // 4?  ,?DCDC
{
	//  Clr_Screen();                                                               //3?
	Set_Display_Power(0x00);
	HAL_GPIO_WritePin(OLED_EN_PORT, OLED_EN_PIN, GPIO_PIN_RESET);//DC-DC//P5OUT |= (BIT2);//20130507-20//??
}

