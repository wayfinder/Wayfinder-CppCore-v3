# Automated Makefile

LIBS += -lz

OUT = libtemplatelib.a

INCLUDES = -I. -I../snapshot -I../gen

CC = g++
CFLAGS = -Wall -O2 -DHAVE_ZLIB
#COMPILE = $(CC) $(CFLAGS) $(LIBS) -c
COMPILE = $(CC) $(INCLUDES) $(CFLAGS) -c
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

#all: myprog
all: $(OUT)

#myprog: $(OBJFILES)
#	$(CC) -o myprog $(OBJFILES)

$(OUT): $(OBJFILES)
	ar rcs $(OUT) $(OBJFILES)


%.o: %.cpp
	$(COMPILE) -o $@ $<

clean:
	rm -f $(OBJFILES) $(OUT)
