#include "../inc/bsp_uart.h"
#include "../inc/bsp_shared.h"

#ifdef HAL_UART_MODULE_ENABLED

typedef enum UART_INDEX
{
    UART4_INDEX = 0,
    UART5_INDEX,
    UART7_INDEX,
    UART8_INDEX,
    UART_COUNT,
} UART_INDEX;

DEFINE_DEVICE_REGISTER_BEGIN(UART, UART_COUNT)
DEFINE_DEVICE_REGISTER_ITEM(UART4_BASE, UART4_INDEX)
DEFINE_DEVICE_REGISTER_ITEM(UART5_BASE, UART5_INDEX)
DEFINE_DEVICE_REGISTER_ITEM(UART7_BASE, UART7_INDEX)
DEFINE_DEVICE_REGISTER_ITEM(UART8_BASE, UART8_INDEX)
DEFINE_DEVICE_REGISTER_END(UART)

static void Uart_TxCpltCallback__(UART_HandleTypeDef *handle)
{
    UartDevice *device = DEVICE_INSTANCE_FIND(handle->Instance);
    if (device->onTxComplete)
    {
        device->onTxComplete(device);
    }
};

static void Uart_RxEventCpltCallback__(UART_HandleTypeDef *handle, uint16_t pos)
{
    UartDevice *device = DEVICE_INSTANCE_FIND(handle->Instance);
    // if (device->_status.isDmaRx)
    {
        SCB_InvalidateDCache_by_Addr(device->_rxBuffer.data, device->_rxBuffer.size);
    }

    if (device->onRxComplete)
    {
        device->onRxComplete(device, pos);
    }
}

static void Uart_ErrCpltCallback__(UART_HandleTypeDef *handle)
{
    UartDevice *device = DEVICE_INSTANCE_FIND(handle->Instance);
    // if (device->_status.isDmaRx)
    {
        SCB_InvalidateDCache_by_Addr(device->_rxBuffer.data, device->_rxBuffer.size);
    }
    if (device->base.onError)
    {
        device->base.onError((DeviceBase *)device, handle->ErrorCode);
    }
};

OP_RESULT uart_device_create(UartDevice *device, UART_HandleTypeDef *instance, uint16_t dmaThershold)
{
    device_base_create((DeviceBase *)device);
    device->base.instance = instance;
    device->_rxBuffer.data = 0;
    device->_rxBuffer.size = 0;
    device->dmaThershold = dmaThershold;
    device->onTxComplete = NULL;
    device->onRxComplete = NULL;
    HAL_UART_RegisterCallback(instance, HAL_UART_TX_COMPLETE_CB_ID, &Uart_TxCpltCallback__);
    HAL_UART_RegisterRxEventCallback(instance, &Uart_RxEventCpltCallback__);
    HAL_UART_RegisterCallback(instance, HAL_UART_ERROR_CB_ID, &Uart_ErrCpltCallback__);
    DEVICE_INSTANCE_REGISTER(device, instance->Instance);
    return OP_RESULT_OK;
};

OP_RESULT uart_device_init(UartDevice *device) { return OP_RESULT_OK; };
OP_RESULT uart_device_deinit(UartDevice *device) { return OP_RESULT_OK; };

OP_RESULT uart_device_tx(UartDevice *device, uint8_t *data, uint32_t size)
{
    UART_HandleTypeDef *handle = device->base.instance;
    if ((HAL_UART_GetState(handle) & HAL_UART_STATE_BUSY_TX) == HAL_UART_STATE_BUSY_TX)
    {
        return OP_RESULT_BUSY;
    }
    if (size > device->dmaThershold)
    {
        device->_status.isDmaTx = 1;
        SCB_CleanDCache_by_Addr((uint32_t *)data, size);
        return HAL_UART_Transmit_DMA(handle, data, size);
    }
    else
    {
        device->_status.isDmaTx = 0;
        return HAL_UART_Transmit_IT(handle, data, size);
    }
};

OP_RESULT uart_device_circular_rx_start(UartDevice *device, uint8_t *data, uint32_t size)
{
    device->_rxBuffer.data = data;
    device->_rxBuffer.size = size;
    UART_HandleTypeDef *handle = device->base.instance;
    if (handle->hdmarx->Init.Mode != DMA_CIRCULAR)
    {
        return OP_RESULT_NOT_SUPPORT;
    }
    if ((HAL_UART_GetState(handle) & HAL_UART_STATE_BUSY_RX) == HAL_UART_STATE_BUSY_RX)
    {
        return OP_RESULT_BUSY;
    }

    return HAL_UARTEx_ReceiveToIdle_DMA(handle, data, size);
};
OP_RESULT uart_device_circular_rx_stop(UartDevice *device)
{
    UART_HandleTypeDef *handle = device->base.instance;

    if ((HAL_UART_GetState(handle) & HAL_UART_STATE_BUSY_RX) != HAL_UART_STATE_BUSY_RX)
    {
        return OP_RESULT_OK;
    }
    return HAL_UART_DMAStop(handle);
};

void uart_send_byte(const char *data, uint16_t len)
{
    for (uint16_t todo = 0; todo < len; todo++)
    {

        /* ???????????????????????????????????? */
        while (LL_USART_IsActiveFlag_TC(USART1) == 0)
            ;

        /* ??????????????????????????????????????? */
        LL_USART_TransmitData8(USART1, (uint8_t)*data++);
    }
}

#endif // HAL_UART_MODULE_ENABLED