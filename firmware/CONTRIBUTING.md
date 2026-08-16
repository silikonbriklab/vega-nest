# Contributing to Vega Nest Firmware

Thanks for considering a contribution to Vega Nest firmware. This document
defines the coding rules every contribution must follow.

For directory placement, file naming, and the overall repository layout, see
[`firmware/README.md`](README.md) instead — this document covers only the
code-level rules.

## Table of Contents

1. [Header Guards](#1-header-guards)
2. [File Header](#2-file-header)
3. [Naming Convention](#3-naming-convention)
4. [Formatting](#4-formatting)
5. [Comments](#5-comments)
6. [Function Rules](#6-function-rules)
7. [Constants](#7-constants)
8. [Integer Types](#8-integer-types)
9. [Memory Allocation](#9-memory-allocation)
10. [Error Handling](#10-error-handling)
11. [Compiler Warnings](#11-compiler-warnings)
12. [Static Analysis](#12-static-analysis)
13. [Logging](#13-logging)
14. [Include Order](#14-include-order)
15. [Switch Statements](#15-switch-statements)
16. [Loops](#16-loops)
17. [Preprocessor Rules](#17-preprocessor-rules)
18. [Before You Open a PR](#before-you-open-a-pr)

---

## 1. Header Guards

Every header file shall use include guards or `#pragma once` — no exceptions.

```c
#ifndef ADC_H
#define ADC_H
...
#endif
```

or

```c
#pragma once
```

## 2. File Header

Every source file shall open with a standard header block naming the file,
a brief description, the author, and a version:

```c
/************************************************************************************
 * @file      adc.c
 *
 * @brief     ADC Driver
 *
 * @author    Suffiyan
 *
 * @version   1.0.0
 *
 ************************************************************************************/
```

## 3. Naming Convention

- **Variables**: `snake_case` — `uint32_t adc_value;`
- **Global variables**: prefix `g_` — `device_config_t g_device_config;`
- **Static (file-scope) variables**: prefix `s_` — `static uint8_t s_buffer[128];`
- **Booleans**: prefix `is_` / `has_` / `can_` — `bool is_connected;`
- **Structs**: suffix `_t` — `typedef struct { uint32_t id; } device_info_t;`
- **Enums**: suffix `_t`, members `SCREAMING_SNAKE_CASE` prefixed with the enum's subject:

  ```c
  typedef enum
  {
      DEVICE_STATE_IDLE,
      DEVICE_STATE_RUNNING,
      DEVICE_STATE_ERROR
  } device_state_t;
  ```

- **Functions**: prefixed with the owning module's name — `adc_init()`,
  `adc_read()`, `flash_write()`, `mqtt_publish()`. Generic names like
  `Init()`, `Read()`, `Write()` are not acceptable.

## 4. Formatting

- 4-space indentation, **no tabs**.
- Maximum 100 characters per line.
- **Allman** brace style (opening brace on its own line):

  ```c
  if (status == SUCCESS)
  {
      process();
  }
  else
  {
      error();
  }
  ```

  Not: `if(status){ }`

## 5. Comments

Document every public API with a doxygen-style block. Avoid comments that
just restate what the code already says.

```c
/************************************************************************************
 * @brief Reads ADC value
 *
 * @param channel ADC channel
 *
 * @return ADC count
 ************************************************************************************/
```

## 6. Function Rules

- Maximum 100 lines per function; **prefer 40–60**.
- Maximum four parameters — beyond that, group arguments into a struct:

  ```c
  typedef struct
  {
      uint8_t *buffer;
      uint16_t length;
      uint32_t timeout;
  } uart_transfer_t;
  ```

## 7. Constants

No magic numbers. Every literal that means something gets a named constant
or macro.

```c
// Bad
delay_ms(75);

// Good
#define SENSOR_STARTUP_DELAY_MS 75
delay_ms(SENSOR_STARTUP_DELAY_MS);
```

## 8. Integer Types

Use the fixed-width types from `<stdint.h>` exclusively:
`uint8_t`, `uint16_t`, `uint32_t`, `int8_t`, `int16_t`, `int32_t`.

Never use plain `int`, `long`, or `short`, unless a standard library
interface specifically requires it.

## 9. Memory Allocation

Runtime dynamic memory allocation is **prohibited** unless formally
approved. `malloc()`, `calloc()`, `realloc()`, and `free()` are forbidden by
default — flag any exception explicitly in the PR description and get sign-off
before merging.

## 10. Error Handling

Every API shall return a status. Errors are never silently ignored.

```c
sdk_status_t flash_write(...);
```

## 11. Compiler Warnings

All firmware compiles with `-Wall -Wextra -Werror`. **Zero warnings are
permitted** — a warning is a build failure, not a suggestion.

- `-Wall` catches common risky patterns (unused/uninitialized variables, etc).
- `-Wextra` catches more (signed/unsigned comparisons, unused parameters).
- `-Werror` turns every enabled warning into a hard compile error.

## 12. Static Analysis

Every commit shall pass:

- `clang-format`
- `cppcheck`
- Build verification

These run both as a local pre-commit hook and in CI.

## 13. Logging

Use the project's logging macros:

```c
LOG_INFO(...)
LOG_WARN(...)
LOG_ERROR(...)
```

Never use `printf()` inside drivers.

## 14. Include Order

1. Own header
2. Standard library headers
3. Project headers

```c
#include "adc.h"

#include <stdint.h>
#include <stdbool.h>

#include "gpio.h"
#include "system.h"
```

## 15. Switch Statements

Every `switch` shall include a `default` case, even if it's a no-op.

```c
switch (state)
{
    case DEVICE_IDLE:
        break;
    case DEVICE_RUN:
        break;
    default:
        break;
}
```

## 16. Loops

Initialize loop variables properly, and **never modify the loop counter
inside the loop body** — it makes execution flow unpredictable and invites
off-by-one or infinite-loop bugs.

```c
// Dangerous: the body alters execution flow unpredictably
for (int i = 0; i < 10; )
{
    printf("%d\n", i);
    i++;              // Modifying counter inside body
    if (some_condition)
    {
        i--;          // Risk of infinite loop!
    }
}
```

## 17. Preprocessor Rules

Avoid nested `#ifdef` conditionals.

```c
// Good
#if defined(CONFIG_UART)

// Bad
#ifdef A
    #ifdef B
        #ifdef C
```

---

## Before You Open a PR

Run through this checklist — it mirrors the review checklist in the coding
standard:

- [ ] Project builds successfully with no changes to warning flags
- [ ] Zero compiler warnings (`-Wall -Wextra -Werror` clean)
- [ ] `clang-format` applied
- [ ] `cppcheck` passes
- [ ] No dead code
- [ ] All public APIs documented (Section 5, above)
- [ ] No magic numbers (Section 7)
- [ ] No dynamic memory allocation, or an approved, documented exception (Section 9)
- [ ] Every `switch` has a `default` (Section 15)
- [ ] Unit tests updated, if applicable

If your change touches `hal/`, `middleware/`, or adds a new
`example_applications/` project, also check that your Makefile follows the
pattern described in [`firmware/README.md`](README.md#2-how-our-makefiles-pick-up-the-sdk)
for pulling in sources outside the project directory.
