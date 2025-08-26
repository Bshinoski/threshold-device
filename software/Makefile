#!/bin/bash -x

ifndef MSPGCCDIR
    MSPGCCDIR := $(HOME)/ti/msp430-gcc
endif

# Choose your device
DEVICE = msp430fr5994
UPDEVICE = $(shell echo $(DEVICE) | tr a-z A-Z)

# Paths
SUPPORT_FILE_DIRECTORY = $(MSPGCCDIR)/include

# Compiler & flags
CC       = $(MSPGCCDIR)/bin/msp430-elf-g++
CXXFLAGS = -I . \
           -I $(SUPPORT_FILE_DIRECTORY) \
           -mmcu=$(DEVICE) \
           -g \
           -Os \
           -mhwmult=f5series \
           -std=c++11 \
           -D__$(UPDEVICE)__ 

# Linker flags
LFLAGS   = -L . -L $(SUPPORT_FILE_DIRECTORY)

# mspdebug driver
DRIVER   = tilib

# Source and object files
SOURCES  = main.c hm10bluetooth/bluetooth.c lcd/lcd.c led/led.c myoware/myoware.c myoware/serialmonitor.c timer/timer.c  # Add more .c or .cpp files here
OBJECTS  = $(SOURCES:.c=.o)
TARGET   = main.elf

# Default rule
all: $(TARGET)

# Compile each .c into a .o
%.o: %.c
	$(CC) $(CXXFLAGS) -c $< -o $@

# Link all .o into main.elf
$(TARGET): $(OBJECTS)
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $@ $(LFLAGS)

# Flash to MSP430 using mspdebug
install: $(TARGET)
	mspdebug $(DRIVER) "prog $(TARGET)" --allow-fw-update

clean:
	rm -f *.o *.elf
