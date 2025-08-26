#!/bin/bash -x

ifndef $(MSPGCCDIR)
	MSPGCCDIR=$(HOME)/ti/msp430-gcc
endif

#DEVICE = msp430fr5739
DEVICE = msp430fr5994
#DEVICE = msp430fr6989

UPDEVICE=$(shell echo $(DEVICE) | tr a-z A-Z)

#paths
SUPPORT_FILE_DIRECTORY=$(MSPGCCDIR)/include

# compiler options
CC      = $(MSPGCCDIR)/bin/msp430-elf-g++

CXXFLAGS = -I . -I $(SUPPORT_FILE_DIRECTORY) -mmcu=$(DEVICE) -g -Os -mhwmult=f5series -std=c++11
LFLAGS = -L . -L $(SUPPORT_FILE_DIRECTORY)

# mspdebug driver
DRIVER:= tilib


# Compiling
all: bluetooth.elf

%.o: %.c
	$(CC) $(CXXFLAGS) -c -D__$(UPDEVICE)__ $< -o $@

%.elf : %.c
	$(CC) $(CXXFLAGS) $(LFLAGS) -D __$(UPDEVICE)__ $^ -o $@



# Upload to board
install_%: %.elf
		mspdebug $(DRIVER) "prog $<" --allow-fw-update

clean:
	rm -f  *.o *.elf
