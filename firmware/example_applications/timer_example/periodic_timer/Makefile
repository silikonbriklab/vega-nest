#-------------------------------------------------------------------- 
#Project Name	: vega-nest
#Created		: 15-Aug-2026
#Filename		: Makefile
#Purpose		: Periodic timer interrupt program
#Description	: Periodic interrupt for periodic tasks
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
EXECUTABLE_NAME=periodic_timer


include $(SDK_PATH)/bsp/common/config.mk

#+++++++++++++++++++++++
# HAL / middleware sources
#+++++++++++++++++++++++
# config.mk auto-discovers *.c files only under the current
# directory (via `find .`), so it never sees hal_timer.c or
# periodic_timer.c living up in firmware/hal/timer and
# firmware/middleware/periodic_timer. We add them explicitly:
#
# NOTE: OBJECT_FILES_C is NOT reassigned here on purpose.
# $(PROGRAM_ELF)'s prerequisite list is already fixed by the
# time config.mk finishes including, so redefining
# OBJECT_FILES_C afterwards has no effect on the link step.
# Instead we append extra prerequisites + matching compile
# rules for the HAL and middleware objects below.

HAL_DIR := ../../../hal/timer
MIDDLEWARE_DIR := ../../../middleware/periodic_timer
RISCV_CFLAGS += -I$(HAL_DIR)
RISCV_CFLAGS += -I$(MIDDLEWARE_DIR)
RISCV_CFLAGS += -I$(SDK_PATH)/bsp

HAL_OBJS := $(BIN)/hal_timer.o
MIDDLEWARE_OBJS := $(BIN)/periodic_timer.o

# Append hal_timer.o and periodic_timer.o to the existing
# $(PROGRAM_ELF) prerequisites defined in config.mk (no recipe
# here, so this merges rather than overrides).
$(PROGRAM_ELF): $(HAL_OBJS) $(MIDDLEWARE_OBJS)

$(BIN)/hal_timer.o: $(HAL_DIR)/hal_timer.c
	@echo Compiling $<
	@mkdir -p $(@D)
	$(CC) $(RISCV_CFLAGS) -c $< -o $@

$(BIN)/periodic_timer.o: $(MIDDLEWARE_DIR)/periodic_timer.c
	@echo Compiling $<
	@mkdir -p $(@D)
	$(CC) $(RISCV_CFLAGS) -c $< -o $@