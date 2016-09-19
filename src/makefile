#-----------------------------------------------------------------------------
# File    : makefile
# Contents: build cpuinfo module
# Author  : Kristian Loewe, Christian Borgelt
#-----------------------------------------------------------------------------

CC      = gcc -std=c99
CFBASE  = -Wall -Wextra -Wno-unused-parameter -Wconversion -pedantic
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


#-----------------------------------------------------------------------------
# Build Program
#-----------------------------------------------------------------------------
all:        $(PRGS)

cpuinfo: cpuinfo_main.o makefile
	$(LD) $(LDFLAGS) cpuinfo_main.o $(LIBS) -o $@

#-----------------------------------------------------------------------------
# Program
#-----------------------------------------------------------------------------
cpuinfo_main.o:  cpuinfo.h
cpuinfo_main.o:  cpuinfo.c makefile
	$(CC) $(CFLAGS) $(DEFS) -DCPUINFO_MAIN -c cpuinfo.c -o $@

#-----------------------------------------------------------------------------
# Module
#-----------------------------------------------------------------------------
cpuinfo.o:  cpuinfo.h
cpuinfo.o:  cpuinfo.c makefile
	$(CC) $(CFLAGS) -c cpuinfo.c -o $@

#-----------------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------------
clean:
	rm -f *.o *~
