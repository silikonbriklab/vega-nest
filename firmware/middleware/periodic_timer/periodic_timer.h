// SPDX-License-Identifier: Apache-2.0

/******************************************************************************
 * @file periodic_timer.h
 *
 * @brief Periodic timer middleware interface.
 *
 * @details Provides millisecond-based periodic (and one-shot) software
 *          timers built on top of the Timer HAL's 3 general purpose
 *          timer (GPT) instances. Each periodic timer instance maps
 *          1:1 onto a HAL GPT instance (TIMER_0/TIMER_1/TIMER_2).
 *
 * @author Suffiyan
 *
 * @version 1.0.0
 *
 ******************************************************************************/

#ifndef MIDDLEWARE_PERIODIC_TIMER_H
#define MIDDLEWARE_PERIODIC_TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "hal_timer.h"

/** Number of periodic timer instances (one per HAL GPT). */
#define PERIODIC_TIMER_INSTANCE_COUNT TIMER_INSTANCE_COUNT

/**
 * GPT input clock frequency in Hz, used to convert a period in
 * milliseconds to a HAL clock count. This is board/SoC specific and
 * shall be updated to match the actual timer input clock.
 */
#define PERIODIC_TIMER_CLOCK_HZ 100000000UL //100 Mhz

/** Milliseconds per second, used in period-to-clock-count conversion. */
#define PERIODIC_TIMER_MS_PER_SEC 1000UL

typedef enum
{
    PERIODIC_TIMER_SUCCESS = 0,
    PERIODIC_TIMER_ERROR,
    PERIODIC_TIMER_INVALID_PARAM
} periodic_timer_status_t;

typedef void (*periodic_timer_callback_t)(void);

typedef struct
{
    periodic_timer_callback_t callback;
    uint32_t reload_clocks;
    bool auto_reload;
    bool is_created;
    bool is_running;
} periodic_timer_instance_t;


/**
 * @brief Initialize the periodic timer middleware.
 *
 * Initializes the underlying Timer HAL and clears all periodic
 * timer instance state. Shall be called once, before any other
 * periodic_timer_*() API.
 *
 * @return PERIODIC_TIMER_SUCCESS on success.
 */
periodic_timer_status_t periodic_timer_init(void);

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
periodic_timer_status_t periodic_timer_create(uint8_t timer_id,uint32_t period_ms,bool auto_reload,periodic_timer_callback_t callback);

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
periodic_timer_status_t periodic_timer_start(uint8_t timer_id);

/**
 * @brief Stop a running periodic timer instance.
 *
 * @param timer_id Periodic timer instance number.
 *
 * @return PERIODIC_TIMER_SUCCESS on success.
 * @return PERIODIC_TIMER_INVALID_PARAM for an invalid timer_id.
 */
periodic_timer_status_t periodic_timer_stop(uint8_t timer_id);

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
periodic_timer_status_t periodic_timer_delete(uint8_t timer_id);

#endif /* MIDDLEWARE_PERIODIC_TIMER_H */