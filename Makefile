# Wii homebrew application Makefile

# Detect OS
ifeq ($(OS),Windows_NT)
    # Windows settings
    DEVKITPPC = C:/devkitPro/devkitPPC
    DEVKITPRO = C:/devkitPro
    # Use MSYS2 shell
    SHELL = C:/devkitPro/msys2/usr/bin/bash.exe
    # Use Unix commands from MSYS2
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
    SEP = /
    PATH_SEP = :
else
    # Unix settings
    DEVKITPPC = /opt/devkitpro/devkitPPC
    DEVKITPRO = /opt/devkitpro
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
    SEP = /
    PATH_SEP = :
endif

# Compiler settings
CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc
CXX = $(DEVKITPPC)/bin/powerpc-eabi-g++
LD = $(DEVKITPPC)/bin/powerpc-eabi-ld
OBJCOPY = $(DEVKITPPC)/bin/powerpc-eabi-objcopy

# Compiler flags
CFLAGS = -O2 -Wall -Wextra -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float
LDFLAGS = -Wl,-Map,$(notdir $@).map

# Include paths
INCLUDES = -I$(DEVKITPPC)/include \
          -I$(DEVKITPPC)/include/ogc \
          -I$(DEVKITPRO)/libogc/include \
          -I$(DEVKITPRO)/libogc/include/ogc \
          -Isrc

# Libraries
LIBS = -L$(DEVKITPRO)/libogc/lib/wii -logc -lm

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
	$(MKDIR) build/src
	$(MKDIR) build/src/usb
	$(MKDIR) build/src/audio
	$(MKDIR) build/src/ui
	$(MKDIR) build/src/game

# Compile source files
%.o: %.c
	@echo "Compiling $<"
	@echo "Include paths: $(INCLUDES)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link the ELF file
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

# Create DOL file
$(DOL): $(TARGET)
	$(OBJCOPY) -O binary $< $@

# Clean
clean:
	$(RM) $(OBJECTS)
	$(RM) $(TARGET)
	$(RM) $(DOL)
	$(RM) $(notdir $(TARGET)).map
	$(RMDIR) build

# Debug info
debug:
	@echo "DEVKITPPC: $(DEVKITPPC)"
	@echo "DEVKITPRO: $(DEVKITPRO)"
	@echo "Include paths: $(INCLUDES)"
	@echo "Source files: $(SOURCES)"
	@echo "Object files: $(OBJECTS)"
	@echo "PATH: $(PATH)"

# Package target for SD card installation
.PHONY: package
package: $(DOL)
	@echo "Creating package for SD card installation..."
	@mkdir -p package/apps/siing
	@cp $(TARGET) package/apps/siing/boot.elf
	@cp $(DOL) package/apps/siing/boot.dol
	@cp meta.xml package/apps/siing/
	@if [ -f icon.png ]; then \
		cp icon.png package/apps/siing/; \
	else \
		echo "Warning: icon.png not found. Please create a 128x128 PNG icon file."; \
	fi
	@echo "Package created in package/apps/siing/"
	@echo "Copy the contents of package/apps/siing to your SD card at /apps/siing/"

# Deploy target for copying to SD card
.PHONY: deploy
deploy: package
	@echo "Deploying to SD card..."
	@if [ -d "D:/apps" ]; then \
		rm -rf "D:/apps/siing"; \
		mkdir -p "D:/apps/siing"; \
		cp package/apps/siing/* "D:/apps/siing/"; \
		echo "Successfully deployed to D:/apps/siing/"; \
	else \
		echo "Error: D:/apps directory not found. Please make sure your SD card is mounted at D: and has an apps directory."; \
		exit 1; \
	fi

.PHONY: all clean build_dirs debug 