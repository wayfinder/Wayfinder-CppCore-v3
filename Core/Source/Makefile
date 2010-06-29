CC=g++

# compile flags
ifeq ($(DEBUG),yes)
# Debug mode
	CXXFLAGS=-Wall -g
else
# Release mode
	CXXFLAGS=-Wall -O2
endif

# Name of the out file
OUT = libCore.a

PALDIR = ../../PAL
COMMONDIR = ../../Common
NGPDIR = ../../ngplib

#============================== Source files ==================================

# ConnectionManager
SRC += ConnectionManager/src/ConnectionManager.cpp
SRC += ConnectionManager/src/NetworkBearerWiFi.cpp
SRC += ConnectionManager/src/NetworkBearerPDP.cpp
SRC += ConnectionManager/src/NetworkBearer.cpp
SRC += ConnectionManager/src/CMConnection.cpp
SRC += ConnectionManager/src/ConnectionWriter.cpp
SRC += ConnectionManager/src/Connection.cpp
SRC += ConnectionManager/src/ConnectionReader.cpp

# ServerCom
SRC += ServerCom/src/ReceiverMapHolder.cpp
SRC += ServerCom/src/RequestMapHolder.cpp
SRC += ServerCom/src/ServerHandler.cpp
SRC += ServerCom/src/RequestIdGenerator.cpp
SRC += ServerCom/src/ServerParser.cpp

# Utils
SRC += CV3/Utils/src/MessageQueue.cpp
SRC += CV3/Utils/src/EventDispatcher.cpp

# Location module
SRC += CV3/Modules/Location/src/ProviderProxy.cpp
SRC += CV3/Modules/Location/src/LocationModule.cpp
SRC += CV3/Modules/Location/src/LocationConfig.cpp
SRC += CV3/Modules/Location/src/InternalPositionProvider.cpp
SRC += CV3/Modules/Location/src/PALPositionProviderWrapper.cpp

# MessageRouter
SRC += CV3/Modules/MessageRouter/src/MessageDispatcher.cpp
SRC += CV3/Modules/MessageRouter/src/MessageRouter.cpp

# CoreMain
SRC += CV3/Modules/CoreMain/src/CoreFactory.cpp
SRC += CV3/Modules/CoreMain/src/Core.cpp

#============================= End Source files ===============================

OBJ = $(SRC:.cpp=.o)

# include directories
INCLUDES += -I CV3/Utils/include/
INCLUDES += -I CV3/Modules/CoreMain/include
INCLUDES += -I CV3/Modules/Location/include
INCLUDES += -I CV3/Modules/MessageRouter/include
INCLUDES += -I CV3/Modules/MessageRouter/Interface
INCLUDES += -I CV3/Modules/Interface
INCLUDES += -I CV3/Modules/SessionSettings/include
INCLUDES += -I ConnectionManager/include
INCLUDES += -I ServerCom/include
INCLUDES += -I $(PALDIR)/include
INCLUDES += -I $(COMMONDIR)/Log/include
INCLUDES += -I $(COMMONDIR)/Utils/include
INCLUDES += -I $(COMMONDIR)/Export/include
INCLUDES += -I $(NGPDIR)/NGP

.SUFFIXES: .cpp

default: $(OUT)

.cpp.o:
	$(CC) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak 


