# Compiler and Linker
CC = gcc

# Compiler Flags
# -Wall: enable all warnings
# -Iinclude: look for header files in the include/ directory
CFLAGS = -Wall -Iinclude

# Linker Flags
# -lgpiod: link the GPIO library
# -lpaho-mqtt3cs: Paho MQTT C library with SSL support (for HiveMQ)
# -lm: link the math library
LDFLAGS = -lgpiod -lm -lpaho-mqtt3cs

# Directories
SRC_DIR = src
DRV_DIR = drivers
OBJ_DIR = obj

# Source files - Add new .c files here only
SRCS = $(SRC_DIR)/main.c \
       $(DRV_DIR)/adxl.c \
       $(DRV_DIR)/DS3231.c \
       $(DRV_DIR)/i2c_master.c \
       $(DRV_DIR)/mqtt_client.c

# Automated Object List
# This takes the filenames from SRCS, removes the path, and adds obj/ prefix
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

# VPATH tells 'make' to search these directories for .c source files
VPATH = $(SRC_DIR):$(DRV_DIR)

# Target executable
TARGET = firmware_app

# Default rule
all: $(OBJ_DIR) $(TARGET)

# Rule to create the object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Linking stage
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilation stage - Generic rule for all .o files
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean