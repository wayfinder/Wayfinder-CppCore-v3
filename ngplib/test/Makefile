
LIBS += -lz
LIBS += -ltemplatelib
LIBS += -lsnapshotlib

LIBPATH += -L../snapshot -L../template

OUT = testngp

INCLUDES = -I. -I../snapshot -I../template -I../gen

CC = g++
CFLAGS = -Wall -O2 -DHAVE_ZLIB
COMPILE = $(CC) $(INCLUDES) $(CFLAGS) -c
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))


all: myprog

myprog: $(OBJFILES)
	cd ../snapshot; make
	cd ../template; make
	$(CC) $(LIBPATH) -o $(OUT) $(OBJFILES) $(LIBS)


%.o: %.cpp
	$(COMPILE) -o $@ $<

clean:
	cd ../snapshot; make clean
	cd ../template; make clean
	rm -f $(OBJFILES) $(OUT)
