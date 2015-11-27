/*----------------------------------------------------------------------------
  File    : cpuinfo.h
  Contents: processor information queries
  Author  : Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------------*/
#ifndef __CPUINFO__
#define __CPUINFO__

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
extern int proccnt       (void); /* # logical processors */

#ifdef __linux__                 /* if Linux system */
extern int corecnt       (void); /* # processor cores, parses /proc/cpuinfo */
#ifdef HAVE_HWLOC
extern int corecntHwloc (void);  /* # processor cores, uses hwloc */
#endif
#endif

extern int hasMMX        (void);
extern int hasSSE        (void);
extern int hasSSE2       (void);
extern int hasSSE3       (void);
extern int hasSSSE3      (void);
extern int hasSSE41      (void);
extern int hasSSE42      (void);
extern int hasPOPCNT     (void);
extern int hasAVX        (void);

#endif
