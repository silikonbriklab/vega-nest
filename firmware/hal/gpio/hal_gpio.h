/******************************************************************************
 * @file gpio.h
 *
 * @brief GPIO HAL interface.
 *
 * @author Suffiyan
 *
 * @version 1.0.0
 *
 ******************************************************************************/

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include <stddef.h>
/*
 * Vega SDK GPIO driver.
 *
 * This header is supplied by the existing Vega SDK.
 */
#include <include/uart.h>
#include <include/gpio.h>

typedef enum
{
    GPIO_INPUT = 0,
    GPIO_OUTPUT = 1
} gpio_direction_t;

typedef enum
{
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} gpio_level_t;

typedef enum
{
    GPIO_SUCCESS = 0,
    GPIO_ERROR
} gpio_status_t;

/**
 * @brief Initialize GPIO HAL.
 *
 * @return GPIO_SUCCESS if initialization is successful.
 */
gpio_status_t gpio_init(void);

/**
 * @brief Configure GPIO pin direction.
 *
 * @param pin GPIO pin number.
 * @param direction GPIO direction.
 *
 * @return GPIO_SUCCESS on success.
 * @return GPIO_ERROR on failure.
 */
gpio_status_t gpio_set_direction(uint8_t pin,gpio_direction_t direction);

/**
 * @brief Write a logic level to a GPIO pin.
 *
 * @param pin GPIO pin number.
 * @param level GPIO logic level.
 *
 * @return GPIO_SUCCESS on success.
 * @return GPIO_ERROR on failure.
 */
gpio_status_t gpio_write(uint8_t pin,gpio_level_t level);

/**
 * @brief Read the logic level of a GPIO pin.
 *
 * @param pin GPIO pin number.
 * @param level Pointer to store the GPIO level.
 *
 * @return GPIO_SUCCESS on success.
 * @return GPIO_ERROR on failure.
 */
gpio_status_t gpio_read(uint8_t pin,gpio_level_t *level);

/**
 * @brief Measure the duration of a GPIO pulse.
 *
 * @param pin GPIO pin number.
 * @param level GPIO level to measure.
 * @param duration Pointer to store the pulse duration.
 *
 * @return GPIO_SUCCESS on success.
 * @return GPIO_ERROR on failure.
 */
gpio_status_t gpio_get_pulse_duration(uint8_t pin,gpio_level_t level,uint32_t *duration);

#endif /* GPIO_H */