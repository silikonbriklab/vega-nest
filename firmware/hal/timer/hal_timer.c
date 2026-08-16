// SPDX-License-Identifier: Apache-2.0

/******************************************************************************
 * @file hal_timer.c
 *
 * @brief Timer HAL implementation.
 *
 * @author Suffiyan
 *
 * @version 1.0.0
 *
 ******************************************************************************/

#include "hal_timer.h"

#include <stdbool.h>

/**
 * Application callbacks registered per timer instance. Populated by
 * timer_attach_isr() and invoked by the HAL's own interrupt handlers
 * below, after the interrupt has been cleared.
 */
static timer_isr_t s_timer_callback[TIMER_INSTANCE_COUNT] =
{
    NULL,
    NULL,
    NULL
};

/**
 * @brief Check whether a timer instance number is valid.
 *
 * @param timer_no Timer instance number.
 *
 * @return true if timer_no refers to a valid timer instance.
 */
static bool timer_is_valid(uint8_t timer_no)
{
    return (timer_no < TIMER_INSTANCE_COUNT);
}


/**
 * @brief Initialize Timer HAL.
 *
 * Registers this HAL's own interrupt handlers with the driver so
 * that hardware timer interrupts are always routed through the HAL
 * first (EOI clearing) before reaching any application callback.
 * This shall be called once, before timer_start() is used on any
 * timer instance.
 *
 * @return TIMER_SUCCESS.
 */
timer_status_t timer_init(void)
{
    timer_register_isr(TIMER_0, timer0_intr_handler); /* Register timer0 Intr. */
    timer_register_isr(TIMER_1, timer1_intr_handler); /* Register timer1 Intr. */
    timer_register_isr(TIMER_2, timer2_intr_handler); /* Register timer2 Intr. */

    return TIMER_SUCCESS;
}


/**
 * @brief Load a timer's count value.
 *
 * @param timer_no Timer instance number.
 * @param count Count value to load into the timer.
 *
 * @return TIMER_SUCCESS on success.
 * @return TIMER_ERROR for an invalid timer number.
 */
timer_status_t timer_set_count(uint8_t timer_no, uint32_t count)
{
    if (!timer_is_valid(timer_no))
    {
        return TIMER_ERROR;
    }

    timer_load(timer_no, count);

    return TIMER_SUCCESS;
}


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
timer_status_t timer_start(uint8_t timer_no, uint32_t no_of_clocks)
{
    if (!timer_is_valid(timer_no))
    {
        return TIMER_ERROR;
    }

    // timer_load(timer_no, no_of_clocks);
    timer_run_in_intr_mode(timer_no, no_of_clocks);
    // timer_unmask_intr(timer_no);
    timer_enable(timer_no);

    return TIMER_SUCCESS;
}


/**
 * @brief Stop a timer.
 *
 * @param timer_no Timer instance number.
 *
 * @return TIMER_SUCCESS on success.
 * @return TIMER_ERROR for an invalid timer number.
 */
timer_status_t timer_stop(uint8_t timer_no)
{
    if (!timer_is_valid(timer_no))
    {
        return TIMER_ERROR;
    }

    timer_disable(timer_no);

    return TIMER_SUCCESS;
}


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
timer_status_t timer_attach_isr(uint8_t timer_no, timer_isr_t isr)
{
    if (!timer_is_valid(timer_no))
    {
        return TIMER_ERROR;
    }

    if (isr == NULL)
    {
        return TIMER_ERROR;
    }

    s_timer_callback[timer_no] = isr;

    return TIMER_SUCCESS;
}


/**
 * @brief Timer 0 interrupt handler.
 *
 * Clears the Timer 0 interrupt by reading the EOI register, then
 * invokes the application callback attached with timer_attach_isr(),
 * if any. Application code is never responsible for clearing the
 * interrupt.
 */
void timer0_intr_handler(void)
{
    (void)Timer(0).EOI; /* Read clears the Timer 0 interrupt. */

    if (s_timer_callback[GP_TIMER_0] != NULL)
    {
        s_timer_callback[GP_TIMER_0]();
    }
}


/**
 * @brief Timer 1 interrupt handler.
 *
 * Clears the Timer 1 interrupt by reading the EOI register, then
 * invokes the application callback attached with timer_attach_isr(),
 * if any. Application code is never responsible for clearing the
 * interrupt.
 */
void timer1_intr_handler(void)
{
    (void)Timer(1).EOI; /* Read clears the Timer 1 interrupt. */

    if (s_timer_callback[GP_TIMER_1] != NULL)
    {
        s_timer_callback[GP_TIMER_1]();
    }
}


/**
 * @brief Timer 2 interrupt handler.
 *
 * Clears the Timer 2 interrupt by reading the EOI register, then
 * invokes the application callback attached with timer_attach_isr(),
 * if any. Application code is never responsible for clearing the
 * interrupt.
 */
void timer2_intr_handler(void)
{
    (void)Timer(2).EOI; /* Read clears the Timer 2 interrupt. */

    if (s_timer_callback[GP_TIMER_2] != NULL)
    {
        s_timer_callback[GP_TIMER_2]();
    }
}