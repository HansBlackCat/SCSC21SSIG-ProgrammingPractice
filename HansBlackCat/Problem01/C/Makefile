CC = gcc
CFLAGS = -Wall
HEADERS = dbg.h stack.h rbtree.h
OBJS = rbtree.o main.o
BUILD_DIR = ./build
CONFIG = debug
TARGET = main

ifneq "$(findstring clean, $(MAKECMDGOALS))" ""
    ARG.CLEAN  = 1
endif

ifneq "$(findstring release, $(MAKECMDGOALS))" ""
    ARG.RELEASE = 1
endif

ifneq "$(findstring debug, $(MAKECMDGOALS))" ""
    ARG.DEBUG = 1
endif

ifeq ($(ARG.RELEASE),1)
	CONFIG = release
else
	CFLAGS += -DDEBUG -g
endif

rbtree.o : $(HEADERS) rbtree.c
	$(CC) $(CFLAGS) -c rbtree.c

main.o : main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

release : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

debug : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(OBJS) $(DOBJS) rbtree