# compile flags
ifeq ($(DEBUG),yes)
# Debug mode
	CXXFLAGS=-Wall -g
else
# Release mode
	CXXFLAGS=-Wall -O2
endif

# Name of the out file
OUT = libPAL.a

# Compiler to use
CC = g++

PALDir ?= .

#============================== Source files ==================================

# Files for POSIX
SRC += POSIX/src/PALThreading.cpp
SRC += POSIX/src/PALTimeUtils.cpp
SRC += POSIX/src/PALBearer.cpp
SRC += POSIX/src/PALConnection.cpp
SRC += POSIX/src/PALSocket.cpp
SRC += POSIX/src/JTC/JTCVersion.cpp
SRC += POSIX/src/JTC/JTCRWMutex.cpp
SRC += POSIX/src/JTC/JTCTSSManager.cpp
SRC += POSIX/src/JTC/JTCThread.cpp
SRC += POSIX/src/JTC/JTCThreadGroup.cpp
SRC += POSIX/src/JTC/JTCCond.cpp
SRC += POSIX/src/JTC/JTCInit.cpp
SRC += POSIX/src/JTC/JTCSync.cpp
SRC += POSIX/src/JTC/JTCMutex.cpp
SRC += POSIX/src/JTC/JTCTSS.cpp
SRC += POSIX/src/JTC/JTCMonitor.cpp

# Files specific for Linux
SRC += Linux/src/PALGraphics.cpp
SRC += Linux/src/PALEnvironment.cpp
SRC += Linux/src/PALLocation.cpp

#============================= End Source files ===============================

OBJ = $(SRC:.cpp=.o)

# include directories
CXXFLAGS += $(shell pkg-config --cflags gtk+-2.0) 
INCLUDES += -I include
INCLUDES += -I Linux/include
INCLUDES += -I POSIX/include
INCLUDES += -I POSIX/include/JTC

.SUFFIXES: .cpp

default: $(OUT)

.cpp.o:
	$(CC) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak 


