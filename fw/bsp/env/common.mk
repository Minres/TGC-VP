# See LICENSE for license details.

ifndef _SIFIVE_MK_COMMON
_SIFIVE_MK_COMMON := # defined

.PHONY: all
all: $(TARGET)

include $(BSP_BASE)/libwrap/libwrap.mk

ENV_DIR = $(BSP_BASE)/env
PLATFORM_DIR = $(ENV_DIR)/$(BOARD)

ASM_SRCS += $(ENV_DIR)/start.S
ASM_SRCS += $(ENV_DIR)/entry.S
C_SRCS += $(PLATFORM_DIR)/init.c

LINKER_SCRIPT := $(PLATFORM_DIR)/$(LINK_TARGET).lds

INCLUDES += -I$(BSP_BASE)/include
INCLUDES += -I$(BSP_BASE)/drivers/
INCLUDES += -I$(ENV_DIR)
INCLUDES += -I$(PLATFORM_DIR)

TOOL_DIR ?= $(BSP_BASE)/../toolchain/bin/

LDFLAGS += -T $(LINKER_SCRIPT) -nostartfiles
LDFLAGS += -L$(ENV_DIR) --specs=nano.specs

ASM_OBJS := $(ASM_SRCS:.S=.o)
C_OBJS   := $(C_SRCS:.c=.o)
CXX_OBJS := $(CXX_SRCS:.cpp=.o)

LINK_OBJS += $(ASM_OBJS) $(C_OBJS) $(CXX_OBJS)
LINK_DEPS += $(LINKER_SCRIPT)

CLEAN_OBJS += $(TARGET) $(LINK_OBJS)

CFLAGS += -march=$(RISCV_ARCH)
CFLAGS += -mabi=$(RISCV_ABI)
CFLAGS += -mcmodel=medany

TRIPLET?=riscv64-unknown-elf
CXX=$(TOOL_DIR)$(TRIPLET)-c++
CC=$(TOOL_DIR)$(TRIPLET)-gcc
LD=$(TOOL_DIR)$(TRIPLET)-gcc
AR=$(TOOL_DIR)$(TRIPLET)-ar
OBJDUMP := $(TOOL_DIR)/$(TRIPLET)-objdump


$(TARGET): $(LINK_OBJS) $(LINK_DEPS)
	$(LD) $(LINK_OBJS) $(LDFLAGS) $(LIBWRAP) -o $@
	$(OBJDUMP) -d -S $(TARGET) > $(TARGET).dis
	
$(ASM_OBJS): %.o: %.S $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(C_OBJS): %.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -include sys/cdefs.h -c -o $@ $<

$(CXX_OBJS): %.o: %.cpp $(HEADERS)
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(INCLUDES) -include sys/cdefs.h -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(CLEAN_OBJS) $(LIBWRAP)

endif # _SIFIVE_MK_COMMON
