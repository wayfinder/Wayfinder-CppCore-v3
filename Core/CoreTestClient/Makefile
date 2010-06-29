
CC=g++

ifeq ($(DEBUG),no)
	CXXFLAGS=-Wall 
	LDFLAGS=-Wall 
else
	CXXFLAGS=-Wall -ggdb
	LDFLAGS=-Wall -ggdb
endif

# Specifies the name of the output file
OUT = CoreTestClient

# Source paths
SRCPATH = src

# Lib paths
CORELIBPATH = ../Source
COMMONLIBPATH = ../../Common
PALLIBPATH = ../../PAL
NGPLIBPATH = ../../ngplib/NGP

CXXFLAGS += $(shell pkg-config --cflags gtk+-2.0)
CXXFLAGS += $(shell pkg-config --cflags gthread-2.0)
LDFLAGS += $(shell pkg-config --libs gtk+-2.0)
LDFLAGS += $(shell pkg-config --libs gthread-2.0)

# Libs to link with
LIBS += -lCore
LIBS += -lNGP
LIBS += -lCommon
LIBS += -lPAL
LIBS += -lpthread
LIBS += -L$(CORELIBPATH)
LIBS += -L$(NGPLIBPATH)
LIBS += -L$(COMMONLIBPATH)
LIBS += -L$(PALLIBPATH)

# Source files
SRC += main.cpp
SRC += $(SRCPATH)/ReverseGeocodeTest.cpp

OBJ = $(SRC:.cpp=.o)

# Base include paths
PALDIR = ../../PAL
COMMONDIR = ../../Common
NGPDIR = ../../ngplib/

# Include dirs
INCLUDES += -I include
INCLUDES += -I ../Source/CV3/Modules/CoreMain/include
INCLUDES += -I ../Source/CV3/Modules/MessageRouter/include
INCLUDES += -I ../Source/CV3/Modules/MessageRouter/Interface
INCLUDES += -I ../Source/ServerCom/include
INCLUDES += -I ../Source/CV3/Modules/Interface
INCLUDES += -I $(PALDIR)/include
INCLUDES += -I $(COMMONDIR)/Export/include
INCLUDES += -I $(COMMONDIR)/Utils/include
INCLUDES += -I $(COMMONDIR)/Log/include
INCLUDES += -I $(NGPDIR)/NGP

default: $(OUT)

.cpp.o:
	$(CC) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

$(OUT): $(OBJ) 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) 

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak 
