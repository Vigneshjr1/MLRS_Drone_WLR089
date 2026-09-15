# mLRS SAMR34 RX Build
# Target: WLR089U0 Xplained Pro (SAMR34J18B)

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
LD = $(PREFIX)g++
OBJCOPY = $(PREFIX)objcopy
SIZE = $(PREFIX)size

TARGET = mlrs-rx-wlr089u0-xpro

# Directories
MLRS_DIR = ..
COMMON_DIR = $(MLRS_DIR)/Common
COMMONRX_DIR = $(MLRS_DIR)/CommonRx
MODULES_DIR = $(MLRS_DIR)/modules
SAMR34_LIB = $(MODULES_DIR)/samr34-lib
CMSIS_DIR = $(SAMR34_LIB)/cmsis

# CPU flags
CPU = -mcpu=cortex-m0plus -mthumb

# Defines
DEFS = -D __SAMR34J18B__ -D SAMR34 -D RX_WLR089U0_XPRO

# Include paths
INCS = -I$(CMSIS_DIR) \
       -I$(CMSIS_DIR)/component \
       -I$(CMSIS_DIR)/instance \
       -I$(SAMR34_LIB)/src \
       -I$(COMMON_DIR) \
       -I$(COMMON_DIR)/hal \
       -I$(COMMON_DIR)/hal/samr34 \
       -I$(COMMON_DIR)/sx-drivers \
       -I$(COMMON_DIR)/thirdparty \
       -I$(COMMON_DIR)/thirdparty/gdisp \
       -I$(COMMON_DIR)/mavlink \
       -I$(COMMONRX_DIR) \
       -I$(MODULES_DIR)/stm32ll-lib/src \
       -I$(MODULES_DIR)/sx12xx-lib/src \
       -I$(MODULES_DIR)/fastmavlink/c_library \
       -I$(MODULES_DIR)/dronecan \
       -I.

# Compiler flags
CFLAGS = $(CPU) $(DEFS) $(INCS) -Os -Wall -ffunction-sections -fdata-sections -fno-exceptions -std=c11
CXXFLAGS = $(CPU) $(DEFS) $(INCS) -Os -Wall -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -std=c++17

# Linker flags
LDFLAGS = $(CPU) -T samr34j18b_flash.ld -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(TARGET).map --specs=nano.specs --specs=nosys.specs

# Source files
C_SOURCES = \
    startup_samr34.c \
    system_samr34.c

CXX_SOURCES = \
    main.cpp \
    $(COMMONRX_DIR)/mlrs-rx.cpp \
    $(COMMONRX_DIR)/out.cpp \
    $(COMMON_DIR)/channel_order.cpp \
    $(COMMON_DIR)/common_stats.cpp \
    $(COMMON_DIR)/common_types.cpp \
    $(COMMON_DIR)/diversity.cpp \
    $(COMMON_DIR)/fhss.cpp \
    $(COMMON_DIR)/link_types.cpp \
    $(COMMON_DIR)/lq_counter.cpp \
    $(COMMON_DIR)/setup_types.cpp \
    $(COMMON_DIR)/while.cpp \
    $(COMMON_DIR)/libs/filters.cpp \
    $(COMMON_DIR)/thirdparty/thirdparty.cpp \
    $(MODULES_DIR)/sx12xx-lib/src/sx127x.cpp \
    $(MODULES_DIR)/stm32ll-lib/src/stdstm32.cpp

# Object files
BUILD_DIR = build
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(CXX_SOURCES:.cpp=.o)))

VPATH = $(sort $(dir $(C_SOURCES) $(CXX_SOURCES)))

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
	$(SIZE) $(BUILD_DIR)/$(TARGET).elf

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
