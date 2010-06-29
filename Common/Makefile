

# compile flags
ifeq ($(DEBUG),yes)
# Debug mode
	CXXFLAGS=-Wall -g
else
# Release mode
	CXXFLAGS=-Wall -O2
endif

PALDIR = ../PAL

# Name of the out file
OUT = libCommon.a

# Compiler to use
CC=g++

CommonDir ?= .

#============================== Source files ==================================

# Files in Utils
SRC += Utils/src/FileSystem.cpp
SRC += Utils/src/WFString.cpp
SRC += Utils/src/WGS84Coordinate.cpp
SRC += Utils/src/crc32.cpp
SRC += Utils/src/Buffer.cpp

# Files for logging
SRC += Log/src/CoreDebugPrint.cpp
SRC += Log/src/GlobalLog.cpp

#============================= End Source files ===============================

OBJ = $(SRC:.cpp=.o)

# include directories
INCLUDES += -I Export/include
INCLUDES += -I Utils/include
INCLUDES += -I Log/include
INCLUDES += -I $(PALDIR)/include

.SUFFIXES: .cpp

default: $(OUT)

.cpp.o:
	$(CC) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak 


