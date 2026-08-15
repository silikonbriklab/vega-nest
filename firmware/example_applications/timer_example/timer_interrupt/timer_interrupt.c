#include "hal_timer.h"
#include "stdlib.h"

#define TIMER0_INTERVAL_CLOCKS 200000U
#define TIMER1_INTERVAL_CLOCKS 300000U

static void timer0_tick(void)
{
    printf("\nTimer 0 interrupt:\n");
}

static void timer1_tick(void)
{
    printf("\nTimer 1 interrupt:\n");
}

int main(void)
{
    timer_init();

    timer_attach_isr(GP_TIMER_0, timer0_tick);

    timer_start(GP_TIMER_0, TIMER0_INTERVAL_CLOCKS);

    timer_attach_isr(GP_TIMER_1, timer1_tick);

    timer_start(GP_TIMER_1, TIMER1_INTERVAL_CLOCKS);

    while (1)
    {
    }

    return 0;
}