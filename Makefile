CROSS_COMPILE = arm-none-eabi-
ARCH = arm
VERSION = v7-a
CPU = cortex-a7
BOARD = sunxi

ARM_CFLAGS = -Wall -fomit-frame-pointer -nostdlib -mcpu=$(CPU) -DCPU_CORE_COUNT=4
CROSS_CC = $(CROSS_COMPILE)gcc
CROSS_LD = $(CROSS_COMPILE)ld

#ARM_ELF_FLAGS = -fpic
ARM_ELF_FLAGS = -Os -fno-common -fno-builtin -ffreestanding -nostdinc -fno-strict-aliasing
ARM_ELF_FLAGS += -mno-thumb-interwork -fno-stack-protector -fno-toplevel-reorder
ARM_ELF_FLAGS += -Wstrict-prototypes -Wno-format-nonliteral -Wno-format-security

MKSUNXIBOOT ?= ./tools/mksunxiboot

CODE_DIR = app
ARCH_DIR = arch/$(ARCH)
ARCH_LIB = $(ARCH_DIR)/lib
BIN_DIR = bin
IMPORTED = $(ARCH_DIR)/sunxi/imported
INCLUDES =	-Iarch/include -Iinclude -Idrivers/ccu -Idrivers/gpio -Idrivers/pmu -Idrivers/prcm -Idrivers/ram -Idrivers/uart \
			-Idrivers/mmc -Idrivers/mmc/$(BOARD) -Idrivers/cpucfg -Ifs -Iapp

all: bootloader.elf bootloader.bin bootloader.sunxi

bootloader.elf: 
	$(CROSS_CC) -nostartfiles $(ARM_CFLAGS) $(ARM_ELF_FLAGS) -T $(ARCH_DIR)/$(BOARD)/lscript.lds \
	$(ARCH_DIR)/boot.S $(ARCH_LIB)/_ashldi3.S $(ARCH_LIB)/string.S \
	drivers/ccu/ccu.c drivers/gpio/gpio.c drivers/pmu/pmu.c  drivers/pmu/pmu.S drivers/uart/uart.c \
	drivers/ram/dram_helpers.c drivers/ram/ddr3_1333.c drivers/ram/dram.c \
	drivers/mmc/mmc.c drivers/mmc/$(BOARD)/mmc_bsp.c drivers/cpucfg/cpucfg.c \
	lib/delay.c lib/strtoul.c lib/itoa.c fs/fat32.c \
	$(CODE_DIR)/cmd.c $(CODE_DIR)/parser.c $(CODE_DIR)/loader.c \
	$(CODE_DIR)/main.c $(INCLUDES) -o $(BIN_DIR)/bootloader.elf

bootloader.bin: bootloader.elf
	$(CROSS_COMPILE)objcopy -O binary $(BIN_DIR)/bootloader.elf $(BIN_DIR)/bootloader.bin

bootloader.sunxi: bootloader.bin
	$(MKSUNXIBOOT) $(BIN_DIR)/bootloader.bin $(BIN_DIR)/bootloader.sunxi