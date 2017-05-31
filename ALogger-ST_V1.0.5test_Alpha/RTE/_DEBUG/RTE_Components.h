
/*
 * Auto generated Run-Time-Environment Component Configuration File
 *      *** Do not modify ! ***
 *
 * Project: 'AM5_Logger' 
 * Target:  'DEBUG' 
 */

#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H


/*
 * Define the Device Header File: 
 */
#define CMSIS_device_header "stm32f4xx.h"

#define RTE_CMSIS_RTOS                  /* CMSIS-RTOS */
        #define RTE_CMSIS_RTOS_RTX              /* CMSIS-RTOS Keil RTX */
#define RTE_Compiler_EventRecorder
          #define RTE_Compiler_EventRecorder_DAP
#define RTE_Compiler_IO_File            /* Compiler I/O: File */
          #define RTE_Compiler_IO_File_FS         /* Compiler I/O: File (File System) */
#define RTE_DEVICE_FRAMEWORK_CLASSIC
#define RTE_DEVICE_HAL_COMMON
#define RTE_DEVICE_HAL_CORTEX
#define RTE_DEVICE_HAL_DMA
#define RTE_DEVICE_HAL_GPIO
#define RTE_DEVICE_HAL_I2C
#define RTE_DEVICE_HAL_PWR
#define RTE_DEVICE_HAL_RCC
#define RTE_DEVICE_HAL_RTC
#define RTE_DEVICE_HAL_SPI
#define RTE_DEVICE_STARTUP_STM32F4XX    /* Device Startup for STM32F4 */
#define RTE_Drivers_I2C1                /* Driver I2C1 */
        #define RTE_Drivers_I2C2                /* Driver I2C2 */
        #define RTE_Drivers_I2C3                /* Driver I2C3 */
#define RTE_Drivers_MCI0                /* Driver MCI0 */
#define RTE_Drivers_SPI1                /* Driver SPI1 */
        #define RTE_Drivers_SPI2                /* Driver SPI2 */
        #define RTE_Drivers_SPI3                /* Driver SPI3 */
        #define RTE_Drivers_SPI4                /* Driver SPI4 */
        #define RTE_Drivers_SPI5                /* Driver SPI5 */
        #define RTE_Drivers_SPI6                /* Driver SPI6 */
#define RTE_Drivers_USBD1               /* Driver USBD1 */
#define RTE_FileSystem_Core             /* File System Core */
          #define RTE_FileSystem_LFN              /* File System with Long Filename support */
          #define RTE_FileSystem_Debug            /* File System Debug Version */
#define RTE_FileSystem_Drive_MC_0       /* File System Memory Card Drive 0 */
#define RTE_Graphics_Core               /* Graphics Core */
#define RTE_USB_Core                    /* USB Core */
          #define RTE_USB_Core_Debug              /* USB Core Debug Version */
#define RTE_USB_Device_0                /* USB Device 0 */
#define RTE_USB_Device_MSC_0            /* USB Device MSC instance 0 */

#endif /* RTE_COMPONENTS_H */
