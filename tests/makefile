# =========================================================================
#   Unity - A Test Framework for C
#   Host-Based Testing (with MSP430 headers + device defines)
# =========================================================================

# 1) If MSPGCCDIR not set, point to your default MSP430 toolchain folder
ifndef MSPGCCDIR
  MSPGCCDIR = $(HOME)/ti/msp430-gcc
endif

# 2) Choose an MSP430 device for conditional compilation
#    (Your code may check #ifdef __MSP430FR5994__, etc.)
DEVICE   = msp430fr5994
UPDEVICE = $(shell echo $(DEVICE) | tr a-z A-Z)

# 3) Host compiler (builds for your PC, not MSP430 hardware)
CC = gcc

# 4) Unity & source paths
UNITY_ROOT = ./unity
SRC_DIR    = ../software

# 5) MSP430 include paths
#    Adjust these if your "msp430.h" is really in a different subfolder.
CFLAGS = -std=c99 -Wall -Wextra
CFLAGS += -Wpointer-arith -Wcast-align -Wwrite-strings -Wswitch-default
CFLAGS += -Wunreachable-code -Winit-self -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas -Wundef

# We'll add both include paths just in case:
CFLAGS += -I$(MSPGCCDIR)/include -I$(MSPGCCDIR)/include/msp430

# Define HOST_BUILD so we skip real hardware code & use stubs
CFLAGS += -DHOST_BUILD

# Also define __$(UPDEVICE)__ so code can #ifdef the device if needed
CFLAGS += -D__$(UPDEVICE)__

# 6) Test target
TARGET = tests.out

# 7) Source files
#    "mock_msp430.c" provides dummy references for hardware registers if needed
SRC_FILES = \
    tests.c \
    $(SRC_DIR)/hm10bluetooth/bluetooth.c \
    $(SRC_DIR)/myoware/myoware.c \
    $(SRC_DIR)/myoware/serialmonitor.c \
    $(SRC_DIR)/led/led.c \
    $(SRC_DIR)/timer/timer.c \
    $(UNITY_ROOT)/src/unity.c \
    $(SRC_DIR)/lcd/lcd.c \
    mock_msp430.c


# 8) Build / Run / Clean
all: clean build run

build:
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(TARGET)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)
