// SPDX-License-Identifier: Apache-2.0

/******************************************************************************
 * @file periodic_timer.c
 *
 * @brief Periodic timer middleware implementation.
 *
 * @author Suffiyan
 *
 * @version 1.0.0
 *
 ******************************************************************************/

#include "periodic_timer.h"


static periodic_timer_instance_t s_periodic_timer[PERIODIC_TIMER_INSTANCE_COUNT];

static void periodic_timer0_isr(void);
static void periodic_timer1_isr(void);
static void periodic_timer2_isr(void);


/**
 * @brief Check whether a periodic timer instance number is valid.
 *
 * @param timer_id Periodic timer instance number.
 *
 * @return true if timer_id refers to a valid periodic timer instance.
 */
static bool periodic_timer_is_valid(uint8_t timer_id)
{
    return (timer_id < PERIODIC_TIMER_INSTANCE_COUNT);
}

/**
 * @brief Convert a period in milliseconds to a HAL clock count.
 *
 * @param period_ms Period in milliseconds.
 *
 * @return Equivalent clock count, or 0 if period_ms is too small
 *         to represent as a non-zero clock count.
 */
static uint32_t periodic_timer_ms_to_clocks(uint32_t period_ms)
{
    uint64_t clocks;

    clocks = ((uint64_t)period_ms * (uint64_t)PERIODIC_TIMER_CLOCK_HZ)
             / (uint64_t)PERIODIC_TIMER_MS_PER_SEC;

    return (uint32_t)clocks;
}


/**
 * @brief Common interrupt dispatch for a periodic timer instance.
 *
 * Re-arms the HAL timer when the instance is configured for
 * auto-reload, then invokes the application callback, if any.
 *
 * @param timer_id Periodic timer instance number.
 */
static void periodic_timer_dispatch(uint8_t timer_id)
{
    if (s_periodic_timer[timer_id].auto_reload)
    {
        (void)timer_start(timer_id, s_periodic_timer[timer_id].reload_clocks);
    }
    else
    {
        s_periodic_timer[timer_id].is_running = false;
    }

    if (s_periodic_timer[timer_id].callback != NULL)
    {
        s_periodic_timer[timer_id].callback();
    }
}


static void periodic_timer0_isr(void)
{
    periodic_timer_dispatch(0U);
}


static void periodic_timer1_isr(void)
{
    periodic_timer_dispatch(1U);
}


static void periodic_timer2_isr(void)
{
    periodic_timer_dispatch(2U);
}


/**
 * @brief Initialize the periodic timer middleware.
 *
 * Initializes the underlying Timer HAL and clears all periodic
 * timer instance state. Shall be called once, before any other
 * periodic_timer_*() API.
 *
 * @return PERIODIC_TIMER_SUCCESS on success.
 */
periodic_timer_status_t periodic_timer_init(void)
{
    uint8_t timer_id;

    (void)timer_init();

    for (timer_id = 0U; timer_id < PERIODIC_TIMER_INSTANCE_COUNT; timer_id++)
    {
        s_periodic_timer[timer_id].callback = NULL;
        s_periodic_timer[timer_id].reload_clocks = 0U;
        s_periodic_timer[timer_id].auto_reload = false;
        s_periodic_timer[timer_id].is_created = false;
        s_periodic_timer[timer_id].is_running = false;
    }

    return PERIODIC_TIMER_SUCCESS;
}


/**
 * @brief Create a periodic (or one-shot) timer instance.
 *
 * Converts period_ms into a HAL clock count using
 * PERIODIC_TIMER_CLOCK_HZ and stores the configuration for the
 * given instance. Does not start the timer; call
 * periodic_timer_start() to arm it.
 *
 * @param timer_id Periodic timer instance number (0..PERIODIC_TIMER_INSTANCE_COUNT-1).
 * @param period_ms Period in milliseconds between callback invocations.
 * @param auto_reload true for a repeating (periodic) timer, false for one-shot.
 * @param callback Function invoked from interrupt context every time the period elapses.
 *
 * @return PERIODIC_TIMER_SUCCESS on success.
 * @return PERIODIC_TIMER_INVALID_PARAM for an invalid timer_id, a
 *         period_ms of 0, a period_ms too small to represent as a
 *         non-zero clock count, or a NULL callback.
 */
periodic_timer_status_t periodic_timer_create(uint8_t timer_id,uint32_t period_ms,bool auto_reload,periodic_timer_callback_t callback)
{
    uint32_t reload_clocks;
    static const periodic_timer_callback_t isr_table[PERIODIC_TIMER_INSTANCE_COUNT] =
    {
        periodic_timer0_isr,
        periodic_timer1_isr,
        periodic_timer2_isr
    };

    if (!periodic_timer_is_valid(timer_id))
    {
        return PERIODIC_TIMER_INVALID_PARAM;
    }

    if ((period_ms == 0U) || (callback == NULL))
    {
        return PERIODIC_TIMER_INVALID_PARAM;
    }

    reload_clocks = periodic_timer_ms_to_clocks(period_ms);

    if (reload_clocks == 0U)
    {
        return PERIODIC_TIMER_INVALID_PARAM;
    }

    s_periodic_timer[timer_id].callback = callback;
    s_periodic_timer[timer_id].reload_clocks = reload_clocks;
    s_periodic_timer[timer_id].auto_reload = auto_reload;
    s_periodic_timer[timer_id].is_created = true;
    s_periodic_timer[timer_id].is_running = false;

    if (timer_attach_isr(timer_id, isr_table[timer_id]) != TIMER_SUCCESS)
    {
        return PERIODIC_TIMER_ERROR;
    }

    return PERIODIC_TIMER_SUCCESS;
}


/**
 * @brief Start (arm) a previously created periodic timer instance.
 *
 * @param timer_id Periodic timer instance number.
 *
 * @return PERIODIC_TIMER_SUCCESS on success.
 * @return PERIODIC_TIMER_ERROR if the instance was not created,
 *         or if the underlying HAL call fails.
 * @return PERIODIC_TIMER_INVALID_PARAM for an invalid timer_id.
 */
periodic_timer_status_t periodic_timer_start(uint8_t timer_id)
{
    if (!periodic_timer_is_valid(timer_id))
    {
        return PERIODIC_TIMER_INVALID_PARAM;
    }

    if (!s_periodic_timer[timer_id].is_created)
    {
        return PERIODIC_TIMER_ERROR;
    }

    if (timer_start(timer_id, s_periodic_timer[timer_id].reload_clocks) != TIMER_SUCCESS)
    {
        return PERIODIC_TIMER_ERROR;
    }

    s_periodic_timer[timer_id].is_running = true;

    return PERIODIC_TIMER_SUCCESS;
}


/**
 * @brief Stop a running periodic timer instance.
 *
 * @param timer_id Periodic timer instance number.
 *
 * @return PERIODIC_TIMER_SUCCESS on success.
 * @return PERIODIC_TIMER_INVALID_PARAM for an invalid timer_id.
 */
periodic_timer_status_t periodic_timer_stop(uint8_t timer_id)
{
    if (!periodic_timer_is_valid(timer_id))
    {
        return PERIODIC_TIMER_INVALID_PARAM;
    }

    (void)timer_stop(timer_id);

    s_periodic_timer[timer_id].is_running = false;

    return PERIODIC_TIMER_SUCCESS;
}


/**
 * @brief Delete a periodic timer instance.
 *
 * Stops the timer if running and clears its configuration and
 * callback. The instance must be re-created before it can be
 * started again.
 *
 * @param timer_id Periodic timer instance number.
 *
 * @return PERIODIC_TIMER_SUCCESS on success.
 * @return PERIODIC_TIMER_INVALID_PARAM for an invalid timer_id.
 */
periodic_timer_status_t periodic_timer_delete(uint8_t timer_id)
{
    if (!periodic_timer_is_valid(timer_id))
    {
        return PERIODIC_TIMER_INVALID_PARAM;
    }

    (void)timer_stop(timer_id);

    s_periodic_timer[timer_id].callback = NULL;
    s_periodic_timer[timer_id].reload_clocks = 0U;
    s_periodic_timer[timer_id].auto_reload = false;
    s_periodic_timer[timer_id].is_created = false;
    s_periodic_timer[timer_id].is_running = false;

    return PERIODIC_TIMER_SUCCESS;
}