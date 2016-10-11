#-----------------------------------------------------------------------------
# File    : makefile
# Contents: build cpuinfo module
# Author  : Kristian Loewe, Christian Borgelt
#-----------------------------------------------------------------------------

CC      = gcc -std=c99
CFBASE  = -Wall -Wextra -Wno-unused-parameter -Wconversion -Wshadow -pedantic
CFLAGS  = $(CFBASE) -DNDEBUG -O2
DEFS    =

LD      = gcc
LDFLAGS =
LIBS    =

PRGS    = cpuinfo

LIB_HWLOC := $(shell find /usr/lib -name libhwloc.so)
ifeq ($(LIB_HWLOC),)
  LIBS +=
else
  LIBS += -lhwloc
  DEFS += -DHAVE_HWLOC
endif

_DUMMY := $(shell mkdir -p ../obj; mkdir -p ../bin)

#-----------------------------------------------------------------------------
# Build Program
#-----------------------------------------------------------------------------
all: $(PRGS)

cpuinfo: ../bin/cpuinfo
	

../bin/cpuinfo: ../obj/cpuinfo_main.o makefile
	$(LD) $(LDFLAGS) ../obj/cpuinfo_main.o $(LIBS) -o $@

#-----------------------------------------------------------------------------
# Program
#-----------------------------------------------------------------------------
../obj/cpuinfo_main.o:  cpuinfo.h
../obj/cpuinfo_main.o:  cpuinfo.c makefile
	$(CC) $(CFLAGS) $(DEFS) -DCPUINFO_MAIN -c cpuinfo.c -o $@

#-----------------------------------------------------------------------------
# Module
#-----------------------------------------------------------------------------
cpuinfo.o: ../obj/cpuinfo.o
	

../obj/cpuinfo.o:  cpuinfo.h
../obj/cpuinfo.o:  cpuinfo.c makefile
	$(CC) $(CFLAGS) -c cpuinfo.c -o $@

#-----------------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------------
clean:
	rm -f ../obj/*.o ../bin/cpuinfo
