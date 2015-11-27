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

#ifndef _WIN32                   /* if Linux or Apple Mac OS system */
extern int corecnt       (void); /* # processor cores */

#ifdef __linux__
#ifdef HAVE_HWLOC
extern int corecntHwloc (void);  /* # processor cores */
#endif
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
