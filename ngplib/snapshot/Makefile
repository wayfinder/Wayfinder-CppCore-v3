# Automated Makefile

LIBS += -lz

OUT = libsnapshotlib.a

CC = g++
CFLAGS = -Wall -O2
#COMPILE = $(CC) $(CFLAGS) $(LIBS) -c
COMPILE = $(CC) $(CFLAGS) -c
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
