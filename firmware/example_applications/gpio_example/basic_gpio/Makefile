#-------------------------------------------------------------------- 
#Project Name	: vega-nest
#Created		: 15-Aug-2026
#Filename		: Makefile
#Purpose		: Basic gpio program
#Description	: Basic gpio program with uart print
#Author(s)		: Suffiyan Shaikh
#--------------------------------------------------------------------    
#See LICENSE for license details.
 
#+++++++++++++++++++++++
# Configurations        
#+++++++++++++++++++++++
# Include the BSP settings

CONFIG_PATH=~/.config/vega-tools/settings.mk
ifeq ("$(wildcard $(CONFIG_PATH))","")
$(error Please install [VEGA SDK]/[VEGA Tools] and setup the environment)
endif

include $(CONFIG_PATH)

ifeq ("$(wildcard $(VEGA_TOOLCHAIN_PATH))","")
$(error Please install [VEGA Tools] and setup the environment)
endif

ifeq ("$(wildcard $(VEGA_SDK))","")
$(error Please install [VEGA SDK] and setup the environment)
endif
SDK_PATH=${VEGA_SDK}

#+++++++++++++++++++++++
# Executable name
#+++++++++++++++++++++++
EXECUTABLE_NAME=basic_gpio


include $(SDK_PATH)/bsp/common/config.mk

#+++++++++++++++++++++++
# HAL sources
#+++++++++++++++++++++++
# config.mk auto-discovers *.c files only under the current
# directory (via `find .`), so it never sees hal_gpio.c living
# up in firmware/hal/gpio. We add it explicitly:
#
# NOTE: OBJECT_FILES_C is NOT reassigned here on purpose.
# $(PROGRAM_ELF)'s prerequisite list is already fixed by the
# time config.mk finishes including, so redefining
# OBJECT_FILES_C afterwards has no effect on the link step.
# Instead we append an extra prerequisite + a matching compile
# rule for the HAL object below.

HAL_DIR := ../../../hal/gpio
RISCV_CFLAGS += -I$(HAL_DIR)
RISCV_CFLAGS += -I$(SDK_PATH)/bsp

HAL_OBJS := $(BIN)/hal_gpio.o

# Append hal_gpio.o to the existing $(PROGRAM_ELF) prerequisites
# defined in config.mk (no recipe here, so this merges rather
# than overrides).
$(PROGRAM_ELF): $(HAL_OBJS)

$(BIN)/hal_gpio.o: $(HAL_DIR)/hal_gpio.c
	@echo Compiling $<
	@mkdir -p $(@D)
	$(CC) $(RISCV_CFLAGS) -c $< -o $@