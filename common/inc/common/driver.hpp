//
// Created by windwolf on 2021/4/15.
//
#pragma once
#ifndef STM32_THREADX_STARTUP_DRIVER_H
#define STM32_THREADX_STARTUP_DRIVER_H

#include "common/device.hpp"
#include "common/ring_buffer.hpp"

namespace Windwolf::Drivers {
    using namespace Windwolf::Common;

    class StreamDriver {
        enum class Status {
            initial,
            opened,
            closed,
        };

    private:
        IoDevice &_device;
        //RingBuffer<uint8_t> _txBuffer;
        //RingBuffer<uint8_t> _rxBuffer;
        Status _status;

    public:
        StreamDriver(IoDevice &device)
                : _device(device), _status(Status::initial) {

        };

        /**
         * @brief Start the device to get ready for transmition or receiving.
         *
         * For device support DMA or IT, it register a callback to device to monitor the tx buffer empty or rx buffer full event.
         * For device not support polling, it start loop routine, in that hardware fills the rx ringbuffer.
         *
         * @return DEVICE_STATUS
         */
//        DEVICE_STATUS Open(const Buffer<uint8_t> &buffer) {
//            _device.RegisterTxCompleteNotification(_rxCallback);
//            return _device->StartRxLoop(buffer);
//        };
//
//        DEVICE_STATUS Close() {
//            _device.RegisterTxCompleteNotification(nullptr);
//            return _device->StopRxLoop(buffer);
//        }

        virtual DEVICE_STATUS Write(Buffer<uint8_t> data, WaitHandle waitHandle);

        virtual DEVICE_STATUS Read(Buffer<uint8_t> data, WaitHandle waitHandle);

        /**
         * @brief Notify driver that rxbuffer is not empty, when new data received.
         *
         * Called by ISR, etc. USART_IDLE, DMA_HALF_COMPLETE, DMA_COMPLATE.
         *
         * @return DEVICE_STATUS
         */
        virtual DEVICE_STATUS NotifyRxReady();
    };

} // namespace Windwolf::Driver

#endif //STM32_THREADX_STARTUP_DRIVER_H