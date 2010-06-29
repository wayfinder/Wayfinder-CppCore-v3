
CC=g++

ifeq ($(DEBUG),no)
	CXXFLAGS=-Wall 
	LDFLAGS=-Wall 
else
	CXXFLAGS=-Wall -ggdb
	LDFLAGS=-Wall -ggdb
endif

# Specifies the name of the output file
OUT = CoreRegTest

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
SRC += $(SRCPATH)/ConcurrentQueueTest.cpp
SRC += $(SRCPATH)/CoordinateTransformationTest.cpp
SRC += $(SRCPATH)/MessageDispatcherTest.cpp
SRC += $(SRCPATH)/MessageRouterTest.cpp
SRC += $(SRCPATH)/NgpReplySerializationTest.cpp
SRC += $(SRCPATH)/NgpServerInfoTest.cpp
SRC += $(SRCPATH)/PALThreadingTest.cpp
SRC += $(SRCPATH)/RegressionTestEnvironment.cpp
SRC += $(SRCPATH)/RegressionTestStarter.cpp
SRC += $(SRCPATH)/RegressionTestUtil.cpp
SRC += $(SRCPATH)/RegressionTest.cpp
SRC += $(SRCPATH)/AsynchRegressionTest.cpp
SRC += $(SRCPATH)/ServerConnectionTest.cpp
SRC += $(SRCPATH)/ServerMultiRequestTest.cpp
SRC += $(SRCPATH)/UnitTest.cpp
SRC += $(SRCPATH)/UnitTestFramework.cpp
SRC += $(SRCPATH)/MultiQueueListenerTest.cpp

OBJ = $(SRC:.cpp=.o)

# Base include paths
PALDIR = ../../PAL
COMMONDIR = ../../Common
NGPDIR = ../../ngplib/

# Include dirs
INCLUDES += -I include
INCLUDES += -I Export/include/Shared
INCLUDES += -I ../Source/CV3/Utils/include
INCLUDES += -I ../Source/CV3/Modules/CoreMain/include
INCLUDES += -I ../Source/CV3/Modules/Location/include
INCLUDES += -I ../Source/CV3/Modules/MessageRouter/include
INCLUDES += -I ../Source/CV3/Modules/MessageRouter/Interface
INCLUDES += -I ../Source/CV3/Modules/Interface
INCLUDES += -I ../Source/ConnectionManager/include
INCLUDES += -I ../Source/ServerCom/include
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
