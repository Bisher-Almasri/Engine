
# Compiler and flags
CC = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -DBX_CONFIG_DEBUG=1

# Directories for the includes
BGFX_INCLUDE_DIR = include
BX_INCLUDE_DIR = bx_include
BIMG_INCLUDE_DIR = bimg_include

# Include directories
BGFX_HEADERS = -I$(BGFX_INCLUDE_DIR) -I$(BX_INCLUDE_DIR) -I$(BIMG_INCLUDE_DIR)

# Library directories (update these if needed)
BGFX_LIB_DIR = .  # Adjust if libbgfx is in another directory

# Detect OS
ifeq ($(OS),Windows_NT)
    PLATFORM = WINDOWS
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        PLATFORM = MACOS
    endif
endif

# Platform-specific linker flags
ifeq ($(PLATFORM),WINDOWS)
    LINKER_FLAGS = -L$(BGFX_LIB_DIR) -Wl,-rpath,. -lbgfx-shared-libRelease -lSDL2 -lgdi32 -lopengl32 -ldxguid
endif

ifeq ($(PLATFORM),LINUX)
    ifeq ($(WAYLAND),1)
        LINKER_FLAGS = -L$(BGFX_LIB_DIR) -Wl,-rpath,. -lbgfx-shared-libRelease -lSDL2 -lGL -lwayland-client -ldl -lpthread -lrt
    else
        LINKER_FLAGS = -L$(BGFX_LIB_DIR) -Wl,-rpath,. -lbgfx-shared-libRelease -lSDL2 -lGL -lX11 -ldl -lpthread -lrt
    endif
endif

ifeq ($(PLATFORM),MACOS)
    BGFX_HEADERS += $(shell sdl2-config --cflags)
    LINKER_FLAGS = -L$(BGFX_LIB_DIR) -Wl,-rpath,. -lbgfx-shared-libRelease -lSDL2 -framework Cocoa -framework Metal -framework QuartzCore
endif

# Target for compiling the executable
all: main

main: main.cpp
	$(CC) $(CXXFLAGS) $(BGFX_HEADERS) main.cpp -o main $(LINKER_FLAGS)

# Rule to clean the project
clean:
	rm -f main
