#ifndef __OLED_H
#define __OLED_H

#include <stdint.h>

#define DELAY_MS    100     /**< Timer Delay in milli-seconds. */

//inside dcdc voltage
#define DCDC_Volt_64                    0x00
#define DCDC_Volt_74                    0x01
#define DCDC_Volt_80                    0x02
#define DCDC_Volt_90                    0x03

//Segment_Remap
#define SEGMENT_REMAP_RIGHT             0x00
#define SEGMENT_REMAP_LEFT              0x01

//Set_Common_Remap
#define COMMON_REMAP_DOWN               0x00
#define COMMON_REMAP_UP                 0x08

//Set_Entire_Display
#define NORMAL_DISPLAY                  0x00
#define ENTIRE_DISPLAY                  0x01

//Set_Inverse_Display
#define REVERSE_DISPLAY                 0x01

//Set_Charge_Pump in or outside
#define INSIDE_CHARGER                  0x01
#define OUTSIDE_CHARGER                 0x00

//Set_Display_On_Off
#define DISPLAY_ON                      0x01
#define DISPLAY_OFF                     0x00

void _Write8_A0(uint8_t command);
void _Write8_A1(uint8_t dat);
void _WriteM8_A1(uint8_t * pdata, int num);
uint8_t _Read8_A1(uint8_t dat);

void Set_Start_Column_Address(uint8_t addr);
void Set_DCDC_Volt(uint8_t param);
void Set_Start_Line_Address(uint8_t line_num);
void Set_Contrast_Control(uint8_t step);
void Set_Segment_Remap(uint8_t param);
void Set_Entire_Display(uint8_t param);
void Set_Inverse_Display(uint8_t param);
void Set_Multiplex_Ratio(uint8_t ratio);
void Set_Charge_Pump(uint8_t param);
void Set_Display_Power(uint8_t param);
void Set_Page_Address(uint8_t addr);
void Set_Common_Remap(uint8_t param);
void Set_Display_Offset(uint8_t offset);
void Set_Display_Clock(uint8_t ratio);
void Set_Precharge_Period(uint8_t param);
void Set_Hardware_Config(uint8_t param);
void Set_VCOM_Deslt(uint8_t level);
void Set_RdMdWr_Start(void);
void Set_RdMdWr_End(void);
void Set_NOP(void);

void ShutDown_Oled(void);

void init_oled(void);


#endif /* HAL_OLED_H */

