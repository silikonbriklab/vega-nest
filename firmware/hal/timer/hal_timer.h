// SPDX-License-Identifier: Apache-2.0

/******************************************************************************
 * @file hal_timer.h
 *
 * @brief Timer HAL interface.
 *
 * @author Suffiyan
 *
 * @version 1.0.0
 *
 ******************************************************************************/

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdint.h>
#include <stddef.h>
/*
 * Vega SDK timer driver.
 *
 * This header is supplied by the existing Vega SDK.
 *
 *   void timer_run_in_intr_mode(UC timer_no, UI no_of_clocks);
 *   void timer_unmask_intr(UC timer_no);
 *   void timer_load(UC timer_no, UI count);
 *   void timer_disable(UC timer_no);
 *   void timer_enable(UC timer_no);
 *   void timer_register_isr(UC timer_no, void (*timer_isr)());
 *
 * NOTE: UC/UI are the Vega SDK typedefs for uint8_t/uint32_t.
 *
 * timer_register_isr() is how a handler gets wired into the driver's
 * interrupt dispatch for a given timer instance - it is not restricted
 * to any particular function name.
 *
 * This HAL keeps EOI clearing out of application code: hal_timer.c
 * defines its own (static) timer0_intr_handler()/timer1_intr_handler()/
 * timer2_intr_handler() functions, each of which clears the Timer(n).EOI
 * register and then invokes whatever callback the application attached
 * via timer_attach_isr(). timer_init() registers these three handlers
 * with timer_register_isr() so hardware interrupts are always routed
 * through the HAL first. Application code only ever calls
 * timer_attach_isr() and never touches the EOI register.
 */
#include <include/timer.h>

/** Number of hardware timer instances exposed by the SDK. */
#define TIMER_INSTANCE_COUNT 3U

typedef enum
{
    GP_TIMER_0 = 0,
    GP_TIMER_1 = 1,
    GP_TIMER_2 = 2
} timer_id_t;

typedef enum
{
    TIMER_SUCCESS = 0,
    TIMER_ERROR
} timer_status_t;

typedef void (*timer_isr_t)(void);

/**
 * @brief Initialize Timer HAL.
 *
 * Registers this HAL's own interrupt handlers with the driver so
 * that hardware timer interrupts are always routed through the HAL
 * first (EOI clearing) before reaching any application callback.
 * Shall be called once, before timer_start() is used on any timer
 * instance.
 *
 * @return TIMER_SUCCESS.
 */
timer_status_t timer_init(void);

/**
 * @brief Load a timer's count value.
 *
 * @param timer_no Timer instance number.
 * @param count Count value to load into the timer.
 *
 * @return TIMER_SUCCESS on success.
 * @return TIMER_ERROR for an invalid timer number.
 */
timer_status_t timer_set_count(uint8_t timer_no, uint32_t count);

/**
 * @brief Start a timer in periodic interrupt mode.
 *
 * Loads the requested clock count, configures the timer for
 * interrupt mode, unmasks its interrupt line and enables it.
 *
 * @param timer_no Timer instance number.
 * @param no_of_clocks Number of clocks before the timer interrupt fires.
 *
 * @return TIMER_SUCCESS on success.
 * @return TIMER_ERROR for an invalid timer number.
 */
timer_status_t timer_start(uint8_t timer_no, uint32_t no_of_clocks);

/**
 * @brief Stop a timer.
 *
 * @param timer_no Timer instance number.
 *
 * @return TIMER_SUCCESS on success.
 * @return TIMER_ERROR for an invalid timer number.
 */
timer_status_t timer_stop(uint8_t timer_no);

/**
 * @brief Attach an application callback to a timer interrupt.
 *
 * The callback is invoked by this HAL's timer interrupt handler
 * after the interrupt has already been cleared. Application code
 * shall not access the EOI register itself.
 *
 * @param timer_no Timer instance number.
 * @param isr Function to invoke when the timer interrupt fires.
 *
 * @return TIMER_SUCCESS on success.
 * @return TIMER_ERROR for an invalid timer number or a NULL isr.
 */
timer_status_t timer_attach_isr(uint8_t timer_no, timer_isr_t isr);

#endif /* HAL_TIMER_H */