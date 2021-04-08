
TARGET  = fir
C_SRCS  = $(wildcard *.c) 
HEADERS = $(wildcard *.h)
CFLAGS += -O2 -g -DISAX

BOARD=tgfs-vp
LINK_TARGET=flash
RISCV_ARCH:=rv32i
RISCV_ABI:=ilp32
LDFLAGS := -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI)

TRIPLET=riscv32-unknown-elf
BSP_BASE = ../bsp

compiler := $(shell which $(TRIPLET)-gcc)
TOOL_DIR=$(dir $(compiler))

#include $(BSP_BASE)/env/common-gcc.mk

clang := $(shell which clang)
LLVM_TOOL_DIR=$(dir $(clang))
include $(BSP_BASE)/env/common-clang.mk
