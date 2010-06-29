ifeq ($(DEBUG),no)
	DBG = no
else
	DBG = yes
endif

# Path where the makefile for Core lib is located
COREDIR = Core/Source
# Path where the makefile for Common lib is located
COMMONDIR = Common
# Path where the makefile for PAL lib is located
PALDIR = PAL
# Path where the makefile for NGP lib is located
NGPDIR = ngplib/NGP
# Path to where the CoreTestClient is located
CORETESTCLIENTDIR = Core/CoreTestClient
# Path to where the regression test client is located
COREREGTESTCLIENTDIR = Core/RegTest

# Subdirs for the different libraries 
SUBDIRSLIBS = $(COREDIR) 
SUBDIRSLIBS += $(COMMONDIR) 
SUBDIRSLIBS += $(PALDIR) 
SUBDIRSLIBS += $(NGPDIR)  

# Subdirs for the clients
SUBDIRSCLIENTS = $(CORETESTCLIENTDIR) 
SUBDIRSCLIENTS += $(COREREGTESTCLIENTDIR)

# Subdirs for all libs and clients
SUBDIRS = $(COREDIR) 
SUBDIRS += $(COMMONDIR) 
SUBDIRS += $(PALDIR) 
SUBDIRS += $(NGPDIR)  
SUBDIRS += $(CORETESTCLIENTDIR) 
SUBDIRS += $(COREREGTESTCLIENTDIR)

# Default target, builds all targets
all: 
	@echo
	@echo "#########################################################"
	@echo "################# Building all targets ##################"
	@echo "#########################################################"
	for i in $(SUBDIRS) ; do \
	(cd $$i ; make DEBUG=$(DBG)) ; \
	done

# Rules to build individual targets
libs:
	@echo
	@echo "#########################################################"
	@echo "################# Building libs #########################"
	@echo "#########################################################"
	for i in $(SUBDIRSLIBS) ; do \
	(cd $$i ; make DEBUG=$(DBG)) ; \
	done

clients:
	@echo
	@echo "#########################################################"
	@echo "################# Building clients ######################"
	@echo "#########################################################"
	for i in $(SUBDIRSCLIENTS) ; do \
	(cd $$i ; make DEBUG=$(DBG)) ; \
	done

core:
	@echo
	@echo "#########################################################"
	@echo "################# Building core lib #####################"
	@echo "#########################################################"
	@cd $(COREDIR) ; make DEBUG=$(DBG)

common:
	@echo
	@echo "#########################################################"
	@echo "################# Building common lib ###################"
	@echo "#########################################################"
	@cd $(COMMONDIR) ; make DEBUG=$(DBG)

pal:
	@echo
	@echo "#########################################################"
	@echo "################# Building pal lib ######################"
	@echo "#########################################################"
	@cd $(PALDIR) ; make DEBUG=$(DBG)

ngp:
	@echo
	@echo "#########################################################"
	@echo "################# Building ngp lib ######################"
	@echo "#########################################################"
	@cd $(NGPDIR) ; make DEBUG=$(DBG)

coretestclient:
	@echo
	@echo "#########################################################"
	@echo "################# Building CoreTestClient ###############"
	@echo "#########################################################"
	@cd $(CORETESTCLIENTDIR) ; make DEBUG=$(DBG)

coreregtestclient:
	@echo
	@echo "#########################################################"
	@echo "################# Building CoreRegTestClient ############"
	@echo "#########################################################"
	@cd $(COREREGTESTCLIENTDIR) ; make DEBUG=$(DBG)

clean:
	@echo
	@echo "#########################################################"
	@echo "################# Cleaning all targets ##################"
	@echo "#########################################################"
	rm -f *~
	for i in $(SUBDIRS) ; do \
	( cd $$i ; make clean ; ) ; \
	done

