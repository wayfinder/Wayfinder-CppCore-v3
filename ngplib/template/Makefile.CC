# This comes from template/overrides/CC/Makefile 

# compile flags
ifeq ($(DEBUG),yes)
# Debug mode
	CXXFLAGS=-Wall -g
else
# Release mode
	CXXFLAGS=-Wall -O2
endif

PALDIR = ../../PAL
COMMONDIR = ../../Common

# Name of the out file
OUT = libNGP.a

# Compiler to use
CC = g++

#============================== Source files ==================================

# Files in Utils
SRC += Coordinate.cpp
SRC += ConvertCoordinate.cpp
SRC += GunzipUtil.cpp
SRC += GzipUtil.cpp
SRC += NavPacket.cpp
SRC += NavReplyPacket.cpp
SRC += NavRequestPacket.cpp
SRC += NParam.cpp
SRC += NParamBlock.cpp
SRC += PacketFactory.cpp

#============================= End Source files ===============================

OBJ = $(SRC:.cpp=.o)

# include directories
INCLUDES += -I $(PALDIR)/include
INCLUDES += -I $(COMMONDIR)/Utils/include

.SUFFIXES: .cpp

default: $(OUT)

.cpp.o:
	$(CC) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak 
