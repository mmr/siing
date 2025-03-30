# Wii homebrew application Makefile

# Detect OS
ifeq ($(OS),Windows_NT)
    # Windows settings
    DEVKITPPC = C:/devkitPro/devkitPPC
    RM = del /F /Q
    RMDIR = rmdir /S /Q
    MKDIR = mkdir
    SEP = \\
else
    # Unix settings
    DEVKITPPC = /opt/devkitpro/devkitPPC
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
    SEP = /
endif

# Compiler settings
CC = "$(DEVKITPPC)/bin/powerpc-eabi-gcc"
CXX = "$(DEVKITPPC)/bin/powerpc-eabi-g++"
LD = "$(DEVKITPPC)/bin/powerpc-eabi-ld"
OBJCOPY = "$(DEVKITPPC)/bin/powerpc-eabi-objcopy"

# Compiler flags
CFLAGS = -O2 -Wall -Wextra -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float
LDFLAGS = -Wl,-Map,$(subst /,$(SEP),$(notdir $@).map)

# Include paths
INCLUDES = -I"$(DEVKITPPC)/include" -I"$(DEVKITPPC)/include/ogc"

# Libraries
LIBS = -L"$(DEVKITPPC)/lib" -logc -lm

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

# Create build directories
$(OBJECTS): | build_dirs

build_dirs:
	$(MKDIR) build
	$(MKDIR) build\src
	$(MKDIR) build\src\usb
	$(MKDIR) build\src\audio
	$(MKDIR) build\src\ui
	$(MKDIR) build\src\game

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
	$(RM) $(subst /,$(SEP),$(OBJECTS))
	$(RM) $(subst /,$(SEP),$(TARGET))
	$(RM) $(subst /,$(SEP),$(DOL))
	$(RM) $(subst /,$(SEP),$(notdir $(TARGET)).map)
	$(RMDIR) build

.PHONY: all clean build_dirs 