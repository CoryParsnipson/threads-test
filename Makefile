# Makefile
#
# author: Cory Parsnipson, 2015
#

# makefile defines
CC = g++
TARGET = out

# compiler flags:
# -g         adds debugging information to the executable file
# -Wall      turns on most, but not all, compiler warnings
# -std=c++11 adhere to the "experimental" C++11 ISO standard
CFLAGS = -g -Wall -std=c++11 -pthread

# path defines
PROJECT_DIR := $(PWD)
OUTPUT_DIR := $(PWD)

# dll stuff
DL_INCLUDES = 

# include paths
INCLUDE_PATHS = $(PROJECT_DIR)

CXX_FILES = *.cpp

all:
	@echo ---- Making target: $(TARGET)
	@echo "  PROJECT_DIR : $(PROJECT_DIR)"
	@echo "  TARGET_DIR  : $(OUTPUT_DIR)"
	@echo ""

	$(CC) $(CFLAGS) $(CXX_FILES) -o $(TARGET) $(addprefix -I, $(INCLUDE_PATHS)) $(DL_INCLUDES)
	
	chmod a+x $(TARGET) 
	mkdir -p $(OUTPUT_DIR)

clean:
	$(RM) $(TARGET)
