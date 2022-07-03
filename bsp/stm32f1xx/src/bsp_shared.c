#include "../inc/bsp_shared.h"

void bsp_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void bsp_delay_us(uint32_t us)
{
    volatile uint32_t remain = us << 1;
    if (us < 1000)
    {
        while (remain--)
        {
            __NOP();
        }
    }
    else
    {
        bsp_delay_ms(us / 1000);
        remain = (us % 1000) << 1;
        while (remain--)
        {
            __NOP();
        }
    }
}