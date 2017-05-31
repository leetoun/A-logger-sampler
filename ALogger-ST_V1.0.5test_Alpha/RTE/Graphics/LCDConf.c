/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************

** emWin V5.32 - Graphical user interface for embedded applications **
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "GUIDRV_SPage.h"
#include "OLED.h"

#define XSIZE_PHYS 96
#define YSIZE_PHYS 64

#define COLOR_CONVERSION GUICC_1
#define DISPLAY_DRIVER GUIDRV_SPAGE_OS_1C1


/*********************************************************************
*
*       LCD_X_Config
*
* Function description
*   Called during the initialization process in order to set up the
*   display driver configuration.
*/
void LCD_X_Config(void) {
	GUI_DEVICE * pDevice;
	GUI_PORT_API PortAPI = {0}; 
	CONFIG_SPAGE Config = {0};

	// Set display driver and color conversion for 1st layer
	pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
	
	LCD_SetMaxNumColors(1);
	
	// Display driver configuration
	if ((DISPLAY_DRIVER == GUIDRV_SPAGE_OSY_1C1)||
		(DISPLAY_DRIVER == GUIDRV_SPAGE_OSX_1C1)||
		(DISPLAY_DRIVER == GUIDRV_SPAGE_OS_1C1)) 
	{
		LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
//		LCD_SetVSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
	} else {
		LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
//		LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
	}

	Config.FirstSEG = 18;
	
	GUIDRV_SPage_Config (pDevice, &Config);

	// Configure hardware routines
	PortAPI.pfWrite8_A0  = _Write8_A0;
	PortAPI.pfWrite8_A1  = _Write8_A1;
	PortAPI.pfWriteM8_A1 = _WriteM8_A1;
	PortAPI.pfRead8_A1   = 0;

	GUIDRV_SPage_SetBus8(pDevice, &PortAPI);

	// Controller configuration
	GUIDRV_SPage_Set1510(pDevice);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
	int r;

	switch (Cmd) {
		case LCD_X_INITCONTROLLER: {
			init_oled();
			return 0;
		}
		case LCD_X_SETVRAMADDR: {
			//
			// Required for setting the address of the video RAM for drivers
			// with memory mapped video RAM which is passed in the 'pVRAM' element of p
			//
			//    LCD_X_SETVRAMADDR_INFO * p;
			//    p = (LCD_X_SETVRAMADDR_INFO *)pData;
			//...
			return 0;
		}
		case LCD_X_SETORG: {
			//
			// Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
			//
			//    LCD_X_SETORG_INFO * p;
			//    p = (LCD_X_SETORG_INFO *)pData;
			//...
			return 0;
		}
		case LCD_X_SETLUTENTRY: {
			//
			// Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
			//
			//    LCD_X_SETLUTENTRY_INFO * p;
			//    p = (LCD_X_SETLUTENTRY_INFO *)pData;
			//...
			return 0;
		}
		case LCD_X_ON: {
			Set_Display_Power(1);
			return 0;
		}
		case LCD_X_OFF: {
			Set_Display_Power(0);
			return 0;
		}
		default:
		r = -1;
	}
	return r;
}

/*************************** End of file ****************************/
