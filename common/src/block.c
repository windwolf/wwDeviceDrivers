#include "../inc/common/block.h"
#include <stdlib.h>
#include <stdint.h>
#include "string.h"

static DEVICE_STATUS _block_write_directly(Block *block, void *data, uint32_t address, uint32_t size);

DEVICE_STATUS block_create(Block *block, void *instance,
                           uint32_t readBlockSize,
                           uint32_t writeBlockSize,
                           uint32_t eraseBlockSize,
                           bool_t needEraseBeforeWrite,
                           BLOCK_MODE readMode,
                           BLOCK_MODE writeMode,
                           BLOCK_MODE eraseMode,
                           Buffer buffer,
                           DEVICE_STATUS (*read)(void *instance, void *data, uint32_t address, uint32_t size),
                           DEVICE_STATUS (*write)(void *instance, void *data, uint32_t address, uint32_t size),
                           DEVICE_STATUS (*erase)(void *instance, uint32_t address, uint32_t size))
{

    if (writeBlockSize < eraseBlockSize)
    {
        LOG("block 4")
        return DEVICE_STATUS_PARAMETER_ERROR;
    }
    if (buffer.size < max(readBlockSize, eraseBlockSize))
    {
        return DEVICE_STATUS_PARAMETER_ERROR;
    }
    block->instance = instance;
    block->needEraseBeforeWrite = needEraseBeforeWrite;
    block->readMode = readMode;
    block->writeMode = writeMode;
    block->eraseMode = eraseMode;
    block->readBlockSize = readBlockSize;
    block->writeBlockSize = writeBlockSize;
    block->eraseBlockSize = eraseBlockSize;
    block->buffer = buffer;
    block->read = read;
    block->write = write;
    block->erase = erase;

    uint32_t maxBlockSize = (readBlockSize > writeBlockSize) ? readBlockSize : writeBlockSize;
    maxBlockSize = (maxBlockSize > eraseBlockSize) ? maxBlockSize : eraseBlockSize;
    block->_maxBlockSize = maxBlockSize;

    block->_readBlockSizeBits = fast_log2(readBlockSize);
    block->_writeBlockSizeBits = fast_log2(writeBlockSize);
    block->_eraseBlockSizeBits = fast_log2(eraseBlockSize);
    block->_readBlockSizeMask = readBlockSize - 1;
    block->_writeBlockSizeMask = writeBlockSize - 1;
    block->_eraseBlockSizeMask = eraseBlockSize - 1;

    return DEVICE_STATUS_OK;
};
DEVICE_STATUS block_read(Block *block, void *data, uint32_t address, uint32_t size)
{
    DEVICE_STATUS rst;
    if (block->readMode == BLOCK_MODE_RANDOM)
    {
        return block->read(block->instance, data, address, size);
    }
    else if (block->readMode == BLOCK_MODE_BLOCK)
    {
        uint32_t blkAddress = address & ~(block->_readBlockSizeMask);
        uint32_t blkSize = size & ~(block->_readBlockSizeMask);
        if (address != blkAddress || size != blkSize)
        {
            // not aligned
            return DEVICE_STATUS_PARAMETER_ERROR;
        }
        return block->read(block->instance, data,
                           address >> block->_readBlockSizeBits,
                           size >> block->_readBlockSizeBits);
    }
    else if (block->readMode == BLOCK_MODE_WRAP)
    {
        uint32_t sizeInBlock, remainSize;
        remainSize = size;

        uint8_t *curDataPtr = data;
        do
        {
            sizeInBlock = block->readBlockSize - address & block->_readBlockSizeMask;
            if (sizeInBlock > remainSize)
            {
                sizeInBlock = remainSize;
            }
            rst = block->read(block->instance, curDataPtr, address, sizeInBlock);

            if (rst != DEVICE_STATUS_OK)
            {
                return rst;
            }

            remainSize -= sizeInBlock;
            curDataPtr += sizeInBlock;
        } while (remainSize > 0);

        return DEVICE_STATUS_OK;
    }
    else
    {
        return DEVICE_STATUS_NOT_SUPPORT;
    }
};

static DEVICE_STATUS _block_write_directly(Block *block, void *data, uint32_t address, uint32_t size)
{
    DEVICE_STATUS rst;
    if (block->writeMode == BLOCK_MODE_RANDOM)
    {
        return block->write(block->instance, data, address, size);
    }
    else if (block->writeMode == BLOCK_MODE_BLOCK)
    {
        return block->write(block->instance, data,
                            address >> block->_writeBlockSizeBits,
                            size >> block->_writeBlockSizeBits);
    }
    else if (block->writeMode == BLOCK_MODE_WRAP)
    {

        uint32_t sizeInBlock, remainSize;
        remainSize = size;

        uint8_t *curDataPtr = data;
        do
        {
            sizeInBlock = block->writeBlockSize - address & block->_writeBlockSizeMask;
            if (sizeInBlock > remainSize)
            {
                sizeInBlock = remainSize;
            }
            rst = block->write(block->instance, curDataPtr, address, sizeInBlock);
            if (rst != DEVICE_STATUS_OK)
            {
                return rst;
            }

            remainSize -= sizeInBlock;
            curDataPtr += sizeInBlock;
        } while (remainSize > 0);

        return DEVICE_STATUS_OK;
    }
    else
    {
        return DEVICE_STATUS_NOT_SUPPORT;
    }
}

DEVICE_STATUS block_write(Block *block, void *data, uint32_t address, uint32_t size)
{
    DEVICE_STATUS rst;
    if (block->writeMode == BLOCK_MODE_BLOCK)
    {
        uint32_t blkAddress = address & ~(block->_writeBlockSizeMask);
        uint32_t blkSize = size & ~(block->_writeBlockSizeMask);
        if (address != blkAddress || size != blkSize)
        {
            // not aligned
            return DEVICE_STATUS_PARAMETER_ERROR;
        }
    }
    if (block->needEraseBeforeWrite)
    {
        uint8_t *buffer = (uint8_t *)block->buffer.data;

        uint32_t wRemainSize = size;
        uint32_t wAddr = address;

        uint8_t *wData = (uint8_t *)data;
        uint32_t erBlkSize = max(block->eraseBlockSize, block->readBlockSize);

        do
        {
            uint32_t erBlkAddr = wAddr ^ ~erBlkSize;
            uint32_t wPosInBlk = wAddr ^ erBlkSize;
            uint32_t wSizeInBlk = min(erBlkSize, erBlkSize - wPosInBlk);

            if (erBlkAddr != wAddr || wRemainSize < erBlkSize)
            {
                // address not aligned to erBlock or tail fragment. read->memcpy->erase->write.
                rst = block_read(block, buffer, erBlkAddr, erBlkSize); // read entire block
                if (rst != DEVICE_STATUS_OK)
                {
                    return rst;
                }
                memcpy((void *)(buffer + wPosInBlk), (const void *)wData, wSizeInBlk);
                rst = block_erase(block, erBlkAddr, erBlkSize);
                if (rst != DEVICE_STATUS_OK)
                {
                    return rst;
                }
                rst = block_write(block, buffer, erBlkAddr, erBlkSize); //write entire block
                if (rst != DEVICE_STATUS_OK)
                {
                    return rst;
                }
            }
            else
            {
                // address aligned to erBlock. middle entire blocks. directly erase->write.
                uint32_t blkCount = wRemainSize / erBlkSize;
                rst = block_erase(block, erBlkAddr, erBlkSize * blkCount);
                if (rst != DEVICE_STATUS_OK)
                {
                    return rst;
                }
                rst = block_write(block, wData, erBlkAddr, erBlkSize * blkCount);
                if (rst != DEVICE_STATUS_OK)
                {
                    return rst;
                }
            }

            wAddr += wSizeInBlk;
            wData += wSizeInBlk;
            wRemainSize -= wSizeInBlk;
        } while (wRemainSize > 0);
        return DEVICE_STATUS_OK;
    }
    else
    {
        return _block_write_directly(block, data, address, size);
    }
};

DEVICE_STATUS block_erase(Block *block, uint32_t address, uint32_t size)
{
    DEVICE_STATUS rst;
    if (block->eraseMode == BLOCK_MODE_RANDOM)
    {
        return block->erase(block->instance, address, size);
    }
    else if (block->eraseMode == BLOCK_MODE_BLOCK)
    {
        uint32_t blkAddress = address & ~(block->_eraseBlockSizeMask);
        uint32_t blkSize = size & ~(block->_eraseBlockSizeMask);
        if (address != blkAddress || size != blkSize)
        {
            // not aligned
            return DEVICE_STATUS_PARAMETER_ERROR;
        }
        return block->erase(block->instance,
                            address >> block->_eraseBlockSizeBits,
                            size >> block->_eraseBlockSizeBits);
    }
    else if (block->eraseMode == BLOCK_MODE_WRAP)
    {
        return DEVICE_STATUS_OK;
    }
    else
    {
        return DEVICE_STATUS_NOT_SUPPORT;
    }
};