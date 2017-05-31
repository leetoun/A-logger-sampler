#ifndef __SPI_H_
#define __SPI_H_

#include "Driver_SPI.h"

extern ARM_DRIVER_SPI Driver_SPI1;
extern ARM_DRIVER_SPI Driver_SPI2;

void SPI1_EvtHandler(uint32_t event);
void SPI2_EvtHandler(uint32_t event);

void SPI_Init(ARM_DRIVER_SPI* SPIdrv,
                    uint32_t control,
                    uint32_t   speed,
              ARM_SPI_SignalEvent_t SPI_EvtHandler);

#endif
