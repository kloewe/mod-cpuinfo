#-----------------------------------------------------------------------
# File    : makefile
# Contents: build cpuinfo module
# Author  : Christian Borgelt
#-----------------------------------------------------------------------
SHELL    = /bin/bash

CC       = gcc -std=c99 -march=native
# CC       = gcc -std=c99 -mavx
CFBASE   = -Wall -Wextra -Wno-unused-parameter -Wconversion \
           -pedantic $(ADDFLAGS)
CFLAGS   = $(CFBASE) -DNDEBUG -O3
# CFLAGS   = $(CFBASE) -g

#-----------------------------------------------------------------------
# Build Modules
#-----------------------------------------------------------------------
all:        cpuinfo.o

cpuinfo.o:  cpuinfo.h
cpuinfo.o:  cpuinfo.c makefile
	$(CC) $(CFLAGS) -c cpuinfo.c -o $@

#-----------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------
clean:
	rm -f *.o *~
