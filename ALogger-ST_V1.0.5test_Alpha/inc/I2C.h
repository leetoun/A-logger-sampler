#ifndef __I2C_H_
#define __I2C_H_

#include "Driver_I2C.h"

extern ARM_DRIVER_I2C Driver_I2C1;

void I2C_Init(ARM_DRIVER_I2C* drv,
                    uint32_t control,
                    uint32_t   speed,
              ARM_I2C_SignalEvent_t I2C_EvtHandler);
void I2C_ReleaseBusy(void);

#endif
