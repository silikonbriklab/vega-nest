// SPDX-License-Identifier: Apache-2.0

/******************************************************************************
 * @file hal_gpio.c
 *
 * @brief GPIO HAL implementation.
 *
 * @author Suffiyan
 *
 * @version 1.0.0
 *
 ******************************************************************************/

#include "hal_gpio.h"


/**
 * @brief Initialize GPIO HAL.
 *
 * The existing Vega SDK GPIO driver does not provide
 * a GPIO initialization API. Therefore no hardware
 * initialization is required here.
 *
 * @return GPIO_SUCCESS.
 */
gpio_status_t gpio_init(void)
{
    return GPIO_SUCCESS;
}


/**
 * @brief Configure GPIO pin direction.
 *
 * The existing Vega SDK configures the GPIO direction
 * automatically inside GPIO_write_pin() and
 * GPIO_read_pin().
 *
 * Therefore this HAL function uses the corresponding
 * SDK operation to establish the requested direction.
 *
 * @param pin GPIO pin number.
 * @param direction GPIO direction.
 *
 * @return GPIO_SUCCESS on success.
 * @return GPIO_ERROR for an invalid direction.
 */
gpio_status_t gpio_set_direction(uint8_t pin,gpio_direction_t direction)
{
    if (direction == GPIO_OUTPUT)
    {
        /*
         * Writing through the SDK automatically
         * configures the pin as OUTPUT.
         *
         * Preserve the current output value by
         * writing LOW.
         */
        GPIO_write_pin(pin, 0U);

        return GPIO_SUCCESS;
    }

    if (direction == GPIO_INPUT)
    {
        /*
         * The SDK does not provide a separate
         * GPIO_set_direction() API.
         *
         * GPIO_read_pin() configures the pin as INPUT
         * before reading it.
         */
        (void)GPIO_read_pin(pin); //ignore return value

        return GPIO_SUCCESS;
    }

    return GPIO_ERROR;
}


/**
 * @brief Write a logic level to a GPIO pin.
 *
 * @param pin GPIO pin number.
 * @param level GPIO logic level.
 *
 * @return GPIO_SUCCESS on success.
 * @return GPIO_ERROR for an invalid level.
 */
gpio_status_t gpio_write(uint8_t pin,gpio_level_t level)
{
    if (level == GPIO_HIGH)
    {
        GPIO_write_pin(pin, 1U);

        return GPIO_SUCCESS;
    }

    if (level == GPIO_LOW)
    {
        GPIO_write_pin(pin, 0U);

        return GPIO_SUCCESS;
    }

    return GPIO_ERROR;
}


/**
 * @brief Read the logic level of a GPIO pin.
 *
 * @param pin GPIO pin number.
 * @param level Pointer to store the GPIO level.
 *
 * @return GPIO_SUCCESS on success.
 * @return GPIO_ERROR if level is NULL.
 */
gpio_status_t gpio_read(uint8_t pin,gpio_level_t *level)
{
    if (level == NULL)
    {
        return GPIO_ERROR;
    }

    if (GPIO_read_pin(pin) != 0U)
    {
        *level = GPIO_HIGH;
    }
    else
    {
        *level = GPIO_LOW;
    }

    return GPIO_SUCCESS;
}


/**
 * @brief Measure the duration of a GPIO pulse.
 *
 * @param pin GPIO pin number.
 * @param level GPIO level to measure.
 * @param duration Pointer to store the pulse duration.
 *
 * @return GPIO_SUCCESS on success.
 * @return GPIO_ERROR if duration is NULL.
 */
gpio_status_t gpio_get_pulse_duration(uint8_t pin,gpio_level_t level,uint32_t *duration)
{
    if (duration == NULL)
    {
        return GPIO_ERROR;
    }

    *duration = pulse_duration(pin, (uint16_t)level);

    return GPIO_SUCCESS;
}