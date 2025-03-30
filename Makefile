# Wii homebrew application Makefile

# Compiler settings
DEVKITPPC = /opt/devkitpro/devkitPPC
CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc
CXX = $(DEVKITPPC)/bin/powerpc-eabi-g++
LD = $(DEVKITPPC)/bin/powerpc-eabi-ld
OBJCOPY = $(DEVKITPPC)/bin/powerpc-eabi-objcopy

# Compiler flags
CFLAGS = -O2 -Wall -Wextra -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float
LDFLAGS = -Wl,-Map,$(notdir $@).map

# Include paths
INCLUDES = -I$(DEVKITPPC)/include -I$(DEVKITPPC)/include/ogc

# Libraries
LIBS = -L$(DEVKITPPC)/lib -logc -lm

# Source files
SOURCES = $(wildcard src/*.c) \
          $(wildcard src/usb/*.c) \
          $(wildcard src/audio/*.c) \
          $(wildcard src/ui/*.c) \
          $(wildcard src/game/*.c)

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Target
TARGET = siing.elf
DOL = siing.dol

# Default target
all: $(DOL)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link the ELF file
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

# Create DOL file
$(DOL): $(TARGET)
	$(OBJCOPY) -O binary $< $@

# Clean
clean:
	rm -f $(OBJECTS) $(TARGET) $(DOL) $(notdir $(TARGET)).map

.PHONY: all clean 