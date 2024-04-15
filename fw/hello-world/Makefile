
TARGET  = hello
ISA?=imc

C_SRCS  = $(wildcard *.c) 
HEADERS = $(wildcard *.h)
OPT ?= -O2
CFLAGS += $(OPT) -g 

BOARD=tgc_vp
LINK_TARGET=link
RISCV_ARCH:=rv32$(ISA)
ifeq ($(ISA),e)
    RISCV_ABI:=ilp32e
else
    RISCV_ABI:=ilp32
endif
LDFLAGS += -g -Wl,--wrap=printf

compiler := $(shell which riscv64-unknown-elf-gcc)
TOOL_DIR=$(dir $(compiler))

TRIPLET=riscv64-unknown-elf
BSP_BASE = ../bsp
include $(BSP_BASE)/env/common-gcc.mk

$(TARGET).vlog:$(TARGET)
	riscv32-unknown-elf-objcopy -O verilog $(TARGET) $(TARGET).vlog
