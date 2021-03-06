#ifndef __BSP_IIC_MEM_H__
#define __BSP_IIC_MEM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "common/device.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_dma.h"

#ifdef HAL_GPIO_MODULE_ENABLED

    typedef struct SoftI2CDevice
    {
        PinDevice *scl;
        PinDevice *sda;
    } SoftI2CDevice;
    OP_RESULT i2c_device_create(I2CDevice *device, SoftI2CDevice *instance);

#endif // HAL_GPIO_MODULE_ENABLED

#ifdef __cplusplus
}
#endif

#endif // __BSP_IIC_MEM_H__