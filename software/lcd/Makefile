#!/bin/bash -x

ifndef $(MSPGCCDIR)
	MSPGCCDIR=$(HOME)/ti/msp430-gcc
endif

DEVICE = msp430fr5994
UPDEVICE=$(shell echo $(DEVICE) | tr a-z A-Z)

# Paths
SUPPORT_FILE_DIRECTORY=$(MSPGCCDIR)/include

# Compiler options
CC      = $(MSPGCCDIR)/bin/msp430-elf-g++

CXXFLAGS = -I . -I $(SUPPORT_FILE_DIRECTORY) -mmcu=$(DEVICE) -g -Os -mhwmult=f5series -std=c++11
LFLAGS = -L . -L $(SUPPORT_FILE_DIRECTORY)

# mspdebug driver
DRIVER:= tilib

# File to compile
TARGET = serialmonitor

# Compiling
all: $(TARGET).elf

%.elf : %.c
	$(CC) $(CXXFLAGS) $(LFLAGS) -D __$(UPDEVICE)__ $< -o $@

# Upload to board
install: $(TARGET).elf
	mspdebug $(DRIVER) "prog $(TARGET).elf" --allow-fw-update

# Clean compiled files
clean:
	rm -f *.o *.elf

