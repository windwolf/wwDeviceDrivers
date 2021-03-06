#include "st7735.h"
#include "common/device.h"
#include "stdio.h"

#define ST7735_EVENT_BUSY (0x01)

#define ST7735_INTERNAL_BUFFER_SIZE (16)

OP_RESULT st7735_create(ST77XX *instance, Command *command)
{
    st77xx_create(instance, command);

    instance->pvGamma[0] = 0x02U;
    instance->pvGamma[1] = 0x1CU;
    instance->pvGamma[2] = 0x07U;
    instance->pvGamma[3] = 0x12U;
    instance->pvGamma[4] = 0x37U;
    instance->pvGamma[5] = 0x32U;
    instance->pvGamma[6] = 0x29U;
    instance->pvGamma[7] = 0x2DU;
    instance->pvGamma[8] = 0x29U;
    instance->pvGamma[9] = 0x25U;
    instance->pvGamma[10] = 0x2BU;
    instance->pvGamma[11] = 0x39U;
    instance->pvGamma[12] = 0x00U;
    instance->pvGamma[13] = 0x01U;
    instance->pvGamma[14] = 0x03U;
    instance->pvGamma[15] = 0x10U;

    instance->nvGamma[0] = 0x03U;
    instance->nvGamma[1] = 0x1DU;
    instance->nvGamma[2] = 0x07U;
    instance->nvGamma[3] = 0x06U;
    instance->nvGamma[4] = 0x2EU;
    instance->nvGamma[5] = 0x2CU;
    instance->nvGamma[6] = 0x29U;
    instance->nvGamma[7] = 0x2DU;
    instance->nvGamma[8] = 0x2EU;
    instance->nvGamma[9] = 0x2EU;
    instance->nvGamma[10] = 0x37U;
    instance->nvGamma[11] = 0x3FU;
    instance->nvGamma[12] = 0x00U;
    instance->nvGamma[13] = 0x00U;
    instance->nvGamma[14] = 0x02U;
    instance->nvGamma[15] = 0x10U;

    instance->command.addressMode = COMMAND_FRAME_MODE_SKIP;
    instance->command.dummyCycles = 0;
    instance->command.altDataMode = COMMAND_FRAME_MODE_SKIP;

    return OP_RESULT_OK;
};

OP_RESULT st7735_reset(ST77XX *instance)
{
    // LOG("ST7735:SWRST s")
    st77xx_command(instance, ST7735_CMD_SOFTWARE_RESET);
    ww_os_delay(120);
    // LOG("ST7735:SWRST e")

    // LOG("ST7735:SWRST s")
    st77xx_command(instance, ST7735_CMD_SOFTWARE_RESET);
    ww_os_delay(120);
    // LOG("ST7735:SWRST e")

    // LOG("ST7735:SLPOUT s")
    st77xx_command(instance, ST7735_CMD_SLEEP_OUT);
    // LOG("ST7735:SLPOUT e")

    // LOG("ST7735:RBGIC s")
    static const uint8_t frctl1[3] = {0x01U, 0x2CU, 0x2DU};
    st77xx_command_write_8(instance, ST7735_CMD_FRAME_RATE_CTRL1, (uint8_t *)frctl1, 3);
    // LOG("ST7735:RBGIC e")

    // LOG("ST7735:PROCH s")
    static const uint8_t frctl2[3] = {0x01U, 0x2CU, 0x2DU};
    st77xx_command_write_8(instance, ST7735_CMD_FRAME_RATE_CTRL2, (uint8_t *)frctl2, 3);
    // LOG("ST7735:PROCH e")

    // LOG("ST7735:FRMRCTL s")
    static const uint8_t frctl3[6] = {0x01U, 0x2CU, 0x2DU, 0x01U, 0x2CU, 0x2DU};
    st77xx_command_write_8(instance, ST7735_CMD_FRAME_RATE_CTRL3, (uint8_t *)frctl3, 6);
    // LOG("ST7735:FRMRCTL e")

    // LOG("ST7735:FRMICTL s")
    static const uint8_t fictl[1] = {0x07U};
    st77xx_command_write_8(instance, ST7735_CMD_FRAME_INVERSION_CTRL, (uint8_t *)fictl, 1);
    // LOG("ST7735:FRMICTL e")

    // LOG("ST7735:PWRCTL1 s")
    static const uint8_t pctl1[3] = {0xA2U, 0x02U, 0x84U};
    st77xx_command_write_8(instance, ST7735_CMD_PWR_CTRL1, (uint8_t *)pctl1, 3);
    // LOG("ST7735:PWRCTL1 e")

    // LOG("ST7735:PWRCTL2 s")
    static const uint8_t pctl2[1] = {0xC5U};
    st77xx_command_write_8(instance, ST7735_CMD_PWR_CTRL2, (uint8_t *)pctl2, 1);
    // LOG("ST7735:PWRCTL2 e")

    // LOG("ST7735:PWRCTL3 s")
    static const uint8_t pctl3[2] = {0x0AU, 0x00U};
    st77xx_command_write_8(instance, ST7735_CMD_PWR_CTRL3, (uint8_t *)pctl3, 2);
    // LOG("ST7735:PWRCTL3 e")

    // LOG("ST7735:PWRCTL4 s")
    static const uint8_t pctl4[2] = {0x8AU, 0x2AU};
    st77xx_command_write_8(instance, ST7735_CMD_PWR_CTRL4, (uint8_t *)pctl4, 2);
    // LOG("ST7735:PWRCTL4 e")

    // LOG("ST7735:PWRCTL5 s")
    static const uint8_t pctl5[2] = {0x8AU, 0xEEU};
    st77xx_command_write_8(instance, ST7735_CMD_PWR_CTRL5, (uint8_t *)pctl5, 2);
    // LOG("ST7735:PWRCTL5 e")

    // LOG("ST7735:PWRCTL s")
    static const uint8_t vcomctl1[1] = {0x0EU};
    st77xx_command_write_8(instance, ST7735_CMD_VCOMH_VCOML_CTRL1, (uint8_t *)vcomctl1, 1);
    // LOG("ST7735:PWRCTL e")

    // LOG("ST7735:DISINV s")
    st77xx_command(instance, ST7735_CMD_DISPLAY_INVERSION_ON);
    // LOG("ST7735:DISINV e")

    // LOG("ST7735:PIXFMT s")
    st77xx_command_write_8(instance, ST7735_CMD_INTERFACE_PIXEL_FORMAT, &instance->colorMode, 1);
    // LOG("ST7735:PIXFMT e")

    // LOG("ST7735:PVGAMMA s")
    st77xx_command_write_8(instance, ST7735_CMD_POSITIVE_VOLTAGE_GAMMA_CONTROL, instance->pvGamma, 16);
    // LOG("ST7735:PVGAMMA e")

    // LOG("ST7735:NVGAMMA s")
    st77xx_command_write_8(instance, ST7735_CMD_NAGATIVE_VALTAGE_GAMMA_CONTROL, instance->nvGamma, 16);
    // LOG("ST7735:NVGAMMA e")

    // LOG("ST7735:DISNOR s")
    st77xx_command(instance, ST7735_CMD_NORMAL_DISPLAY_MODE_ON);
    // LOG("ST7735:DISNOR e")

    // LOG("ST7735:DISP s")
    st77xx_command(instance, ST7735_CMD_DISPLAY_ON);
    // LOG("ST7735:DISP e")

    // LOG("ST7735:MEMDAC s")
    st77xx_command_write_8(instance, ST7735_CMD_MEMORY_DATA_ACCESS_CONTROL, &instance->orientation, 1);
    // LOG("ST7735:MEMDAC e")

    return OP_RESULT_OK;
};

OP_RESULT st7735_inversion(ST77XX *instance, uint8_t on)
{

    st77xx_command(instance, (on ? ST7735_CMD_DISPLAY_INVERSION_ON : ST7735_CMD_DISPLAY_INVERSION_OFF));

    return OP_RESULT_OK;
}

OP_RESULT st7535_sleep(ST77XX *instance, uint8_t on)
{

    st77xx_command(instance, (on ? ST7735_CMD_SLEEP_IN : ST7735_CMD_SLEEP_OUT));

    return OP_RESULT_OK;
}

OP_RESULT st7735_display(ST77XX *instance, uint8_t on)
{

    st77xx_command(instance, (on ? ST7735_CMD_DISPLAY_ON : ST7735_CMD_DISPLAY_OFF));

    return OP_RESULT_OK;
}

OP_RESULT st7735_display_window_set(ST77XX *instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    x1 += instance->xOffset;
    x2 += instance->xOffset;
    y1 += instance->yOffset;
    y2 += instance->yOffset;

    uint16_t x[2] = {x1, x2};
    st77xx_command_write_16(instance, ST7735_CMD_COLUMN_ADDRESS_SET, x, 2);

    uint16_t y[2] = {y1, y2};
    st77xx_command_write_16(instance, ST7735_CMD_ROW_ADDRESS_SET, y, 2);
    return OP_RESULT_OK;
}

OP_RESULT st7735_cursor_set(ST77XX *instance, uint16_t x, uint16_t y)
{
    x += instance->xOffset;
    st77xx_command_write_16(instance, ST7735_CMD_COLUMN_ADDRESS_SET, &x, 1);
    y += instance->yOffset;
    st77xx_command_write_16(instance, ST7735_CMD_ROW_ADDRESS_SET, &y, 1);

    return OP_RESULT_OK;
}

OP_RESULT st7735_pixel_draw(ST77XX *instance, uint16_t x, uint16_t y, uint16_t color)
{
    OP_RESULT ret = OP_RESULT_OK;

    if ((x >= instance->width) || (y >= instance->height))
    {
        return DEVICE_STATUS_POSITION_OUT_OF_RANGE;
    }
    ret = st7735_cursor_set(instance, x, y);

    if (ret != OP_RESULT_OK)
    {
        return ret;
    }

    st77xx_command_write_16(instance, ST7735_CMD_MEMORY_WRITE, &color, 2);

    return OP_RESULT_OK;
}

OP_RESULT st7735_hline_draw(ST77XX *instance, uint16_t x1, uint16_t y, uint16_t x2, uint16_t *data)
{

    OP_RESULT ret = OP_RESULT_OK;

    if ((x1 > instance->width) || (x2 > instance->width))
    {
        return OP_RESULT_GENERAL_ERROR;
    }

    ret = st7735_cursor_set(instance, x1, y);

    if (ret != OP_RESULT_OK)
    {
        return ret;
    }

    st77xx_command_write_16(instance, ST7735_CMD_MEMORY_WRITE, data, x2 - x1 + 1);

    return ret;
}

OP_RESULT st7735_vline_draw(ST77XX *instance, uint16_t x, uint16_t y1, uint16_t y2, uint16_t *data)
{
    OP_RESULT ret = OP_RESULT_OK;

    if ((y1 > instance->height) || (y2 > instance->height))
    {
        return OP_RESULT_GENERAL_ERROR;
    }

    ret = st7735_display_window_set(instance, x, y1, x, y2);

    if (ret != OP_RESULT_OK)
    {
        return ret;
    }

    st77xx_command_write_16(instance, ST7735_CMD_MEMORY_WRITE, data, y2 - y1 + 1);

    return ret;
}

OP_RESULT st7735_rect_draw(ST77XX *instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *data)
{
    OP_RESULT ret = OP_RESULT_OK;

    if (x1 > instance->width || x2 > instance->width)
    {
        return OP_RESULT_GENERAL_ERROR;
    }
    if (y1 > instance->height || y2 > instance->height)
    {
        return OP_RESULT_GENERAL_ERROR;
    }

    ret = st7735_display_window_set(instance, x1, y1, x2, y2);

    if (ret != OP_RESULT_OK)
    {
        return ret;
    }

    uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);
    st77xx_command_write_16(instance, ST7735_CMD_MEMORY_WRITE, data, size);
    // st77xx_command_write_8(instance, ST7735_CMD_MEMORY_WRITE, buf, size * 2);
    return ret;
}

OP_RESULT st7735_rect_fill(ST77XX *instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    OP_RESULT ret = OP_RESULT_OK;

    if (x1 > instance->width || x2 > instance->width)
    {
        return OP_RESULT_GENERAL_ERROR;
    }
    if (y1 > instance->height || y2 > instance->height)
    {
        return OP_RESULT_GENERAL_ERROR;
    }

    ret = st7735_display_window_set(instance, x1, y1, x2, y2);

    if (ret != OP_RESULT_OK)
    {
        return ret;
    }

    uint16_t buf[ST7735_INTERNAL_BUFFER_SIZE];
    for (uint32_t i = 0; i < ST7735_INTERNAL_BUFFER_SIZE; i++)
    {
        buf[i] = color;
    }
    uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);

    for (uint32_t i = 0; i < size / 8; i++)
    {
        st77xx_command_write_16(instance, ST7735_CMD_MEMORY_WRITE, buf, size * 2);
    }

    st77xx_command_write_16(instance, ST7735_CMD_MEMORY_WRITE, buf, (size % 8) / 2);

    return ret;
}

OP_RESULT st7735_id_read(ST77XX *instance, uint32_t *id)
{
    uint32_t id_temp = 0;
    uint8_t rd;
    st77xx_command_read_8(instance, ST7735_CMD_READ_ID1, &rd, 1);
    id_temp += rd;

    st77xx_command_read_8(instance, ST7735_CMD_READ_ID2, &rd, 1);
    id_temp <<= 8;
    id_temp += rd;

    st77xx_command_read_8(instance, ST7735_CMD_READ_ID3, &rd, 1);
    id_temp <<= 8;
    id_temp += rd;

    *id = id_temp;

    return OP_RESULT_OK;
}

OP_RESULT st7735_bitmap_draw(ST77XX *instance, uint32_t x, uint32_t y, uint8_t *pBmp)
{
    uint32_t index, size, width, height, y_pos;
    uint8_t *pbmp;

    /* Get bitmap data address offset */
    index = (uint32_t)pBmp[10] + ((uint32_t)pBmp[11] << 8) + ((uint32_t)pBmp[12] << 16) + ((uint32_t)pBmp[13] << 24);

    /* Read bitmap width */
    width = (uint32_t)pBmp[18] + ((uint32_t)pBmp[19] << 8) + ((uint32_t)pBmp[20] << 16) + ((uint32_t)pBmp[21] << 24);

    /* Read bitmap height */
    height = (uint32_t)pBmp[22] + ((uint32_t)pBmp[23] << 8) + ((uint32_t)pBmp[24] << 16) + ((uint32_t)pBmp[25] << 24);

    /* Read bitmap size */
    size = (uint32_t)pBmp[2] + ((uint32_t)pBmp[3] << 8) + ((uint32_t)pBmp[4] << 16) + ((uint32_t)pBmp[5] << 24);
    size = size - index;

    pbmp = pBmp + index;

    /* Remap Ypos, st7735 works with inverted X in case of bitmap */
    /* X = 0, cursor is on Top corner */
    y_pos = instance->height - y - height;

    st7735_display_window_set(instance, x, y_pos, width, height);

    // uint16_t *buf = (uint16_t *)(instance->buffer);

    st77xx_command_write_16(instance, ST7735_CMD_MEMORY_WRITE, (uint16_t *)pbmp, size / 2);

    return OP_RESULT_OK;
}
