#ifndef ___BSP_SPI_H__
#define ___BSP_SPI_H__

#ifdef __cplusplus
extern "C"
{
#endif

    //#ifdef HAL_SPI_MODULE_ENABLED

#include "../../../common/inc/common/device.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_dma.h"
    OP_RESULT spi_device_create(SpiDevice *device, SPI_HandleTypeDef *instance, uint16_t dmaThershold);

    //#endif // HAL_SPI_MODULE_ENABLED

#ifdef __cplusplus
}
#endif

#endif // ___BSP_SPI_H__