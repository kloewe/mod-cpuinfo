/*----------------------------------------------------------------------
  File    : cpuinfo.c
  Contents: processor information queries
  Author  : Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------*/
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "cpuinfo.h"

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
static int cpuinfo[5];          /* cpu information */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
#ifdef _WIN32                   /* if Microsoft Windows system */
#define cpuid   __cpuid         /* map existing function */
#else                           /* if Linux/Unix system */

static void cpuid (int info[4], int type)
{                               /* --- get CPU information */
  __asm__ __volatile__ (
    "cpuid": "=a" (info[0]), "=b" (info[1]),
             "=c" (info[2]), "=d" (info[3]) : "a" (type) );
}  /* cpuid() */

#endif  /* #ifdef _WIN32 .. #else .. */
/*----------------------------------------------------------------------
References:
  en.wikipedia.org/wiki/CPUID
  stackoverflow.com/a/7495023
  msdn.microsoft.com/en-us/library/vstudio/hskdteyh%28v=vs.100%29.aspx
----------------------------------------------------------------------*/

int proccnt (void)
{                               /* --- get the number of processors */
  #ifdef _WIN32                 /* if Microsoft Windows system */
  SYSTEM_INFO sysinfo;          /* system information structure */
  GetSystemInfo(&sysinfo);      /* get system information */
  return sysinfo.dwNumberOfProcessors;
  #elif defined _SC_NPROCESSORS_ONLN   /* if Linux/Unix system */
  return (int)sysconf(_SC_NPROCESSORS_ONLN);
  #else                         /* if no direct function available */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[1] >> 16) & 0xff;
  #endif                        /* use the cpuid instruction */
}  /* proccnt() */

/*--------------------------------------------------------------------*/

int hasMMX (void)
{                               /* --- check for MMX instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[3] & (1 << 23)) != 0;
}  /* hasMMX() */

/*--------------------------------------------------------------------*/

int hasSSE (void)
{                               /* --- check for SSE instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[3] & (1 << 25)) != 0;
}  /* hasSSE() */

/*--------------------------------------------------------------------*/

int hasSSE2 (void)
{                               /* --- check for SSE2 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[3] & (1 << 26)) != 0;
}  /* hasSSE2() */

/*--------------------------------------------------------------------*/

int hasSSE3 (void)
{                               /* --- check for SSE3 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 <<  0)) != 0;
}  /* hasSSE3() */

/*--------------------------------------------------------------------*/

int hasSSSE3 (void)
{                               /* --- check for SSSE3 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 <<  9)) != 0;
}  /* hasSSSE3() */

/*--------------------------------------------------------------------*/

int hasSSE41 (void)
{                               /* --- check for SSE4.1 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 19)) != 0;
}  /* hasSSE41() */

/*--------------------------------------------------------------------*/

int hasSSE42 (void)
{                               /* --- check for SSE4.2 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 20)) != 0;
}  /* hasSSE42() */

/*--------------------------------------------------------------------*/

int hasPOPCNT (void)
{                               /* --- check for popcnt instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 23)) != 0;
}  /* hasPOPCNT() */

/*--------------------------------------------------------------------*/

int hasAVX (void)
{                               /* --- check for AVX instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 28)) != 0;
}  /* hasAVX() */
