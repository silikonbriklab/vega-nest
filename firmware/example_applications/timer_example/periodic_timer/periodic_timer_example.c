// SPDX-License-Identifier: Apache-2.0

#include "periodic_timer.h"
#include "stdlib.h"

#define LED_BLINK_PERIOD_MS 500U
#define SENSOR_POLL_PERIOD_MS 1000U

static volatile bool s_led_toggle_pending = false;
static volatile uint32_t s_sensor_poll_count = 0U;

/*
 * Periodic timer callbacks. These run in interrupt context - the HAL
 * has already cleared the timer interrupt before this is called, so
 * application code only decides what should happen when the period
 * elapses. Keep this work short; heavier processing should be
 * deferred to the main loop via a flag, as shown for the LED toggle.
 */
static void led_blink_callback(void)
{
    s_led_toggle_pending = true;
}

static void sensor_poll_callback(void)
{
    s_sensor_poll_count++;
}

int main(void)
{
    periodic_timer_init();

    /* TIMER_0: repeating, toggles the LED every 500 ms. */
    periodic_timer_create(TIMER_0, LED_BLINK_PERIOD_MS, true, led_blink_callback);
    periodic_timer_start(TIMER_0);

    /* TIMER_1: repeating, polls a sensor every 1000 ms. */
    periodic_timer_create(TIMER_1, SENSOR_POLL_PERIOD_MS, true, sensor_poll_callback);
    periodic_timer_start(TIMER_1);

    while (1)
    {
        if (s_led_toggle_pending)
        {
            s_led_toggle_pending = false;
            printf("\nLED toggle, sensor polls so far: %u\n", s_sensor_poll_count);
        }
    }

    return 0;
}