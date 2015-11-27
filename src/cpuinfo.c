/*----------------------------------------------------------------------------
  File    : cpuinfo.c
  Contents: processor information queries
  Author  : Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------------*/
#ifdef _WIN32                       /* if Microsoft Windows system */
#  include <windows.h>
#else
#  include <unistd.h>
#  include <stdio.h>
#  include <stdlib.h>
#  ifdef __APPLE__                  /* if Mac OS */
#    include <sys/sysctl.h>
#    include <sys/types.h>
#  endif
#  ifdef __linux__                  /* if Linux */
#    ifdef HAVE_HWLOC
#      include <hwloc.h>            /* needed for corecntHwloc() */
                                    /* link with -lhwloc */
                                    /* on ubuntu: install libhwloc-dev */
#    endif
#  endif
#endif  /* #ifdef _WIN32 .. #else .. */
#include "cpuinfo.h"

/*----------------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------------*/
static int cpuinfo[5];              /* cpu information */

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
#ifdef _WIN32                       /* if Microsoft Windows system */
#define cpuid   __cpuid             /* map existing function */
#else                               /* if Linux/Unix system */
static void cpuid (int info[4], int type)
{                                   /* --- get CPU information */
  __asm__ __volatile__ ("cpuid" :
                        "=a" (info[0]),
                        "=b" (info[1]),
                        "=c" (info[2]),
                        "=d" (info[3])
                        : "a" (type), "c" (0)); // : "a" (type));
}  /* cpuid() */
#endif  /* #ifdef _WIN32 .. #else .. */
/*----------------------------------------------------------------------------
References (cpuid):
  en.wikipedia.org/wiki/CPUID
  stackoverflow.com/a/7495023
  msdn.microsoft.com/en-us/library/vstudio/hskdteyh%28v=vs.100%29.aspx
----------------------------------------------------------------------------*/

#ifdef __linux__                    /* if Linux system */
#ifdef HAVE_HWLOC                   /* if hwloc is available and enabled */
int corecntHwloc (void)
{                                   /* --- number of processor cores */
  int cnt = -1;
  hwloc_topology_t topology;        /* init and load topology */
  hwloc_topology_init(&topology);
  hwloc_topology_load(topology);
  int depth = hwloc_get_type_depth( /* try to get the number of cores */
          topology, HWLOC_OBJ_CORE);/* from topology */
  if (depth != HWLOC_TYPE_DEPTH_UNKNOWN)
    cnt = (int)hwloc_get_nbobjs_by_depth(topology, (unsigned)depth);
  hwloc_topology_destroy(topology); /* destroy topology object */
  return cnt;                       /* return the number of cores */
}  /* corecntHwloc() */
#endif  /* #ifdef HAVE_HWLOC */
#endif  /* #ifdef __linux__ */
/*----------------------------------------------------------------------------
Additional info and references (corecntHwloc):
  This function depends on the Portable Hardware Locality (hwloc)
  software package.
  stackoverflow.com/a/12486105
  open-mpi.org/projects/hwloc
----------------------------------------------------------------------------*/

#ifdef __linux__                    /* if Linux system */
int corecnt (void)
{                                   /* --- number of processor cores */
  FILE *fp;                         /* file pointer */
  int c,i,j,
          n;                        /* # log. processors found in cpuinfo */
  int *phys;                        /* physical ids */
  int *core;                        /* core ids */
  int max_phys = -1;                /* the highest encountered physical id */
  int max_core = -1;                /* the highest encountered core id */
  int nphys = 0;                    /* number of physical processors */
  int ncores = 0;                   /* number of physical cores */
  int tmp = 0;
  int nprocs = proccnt();           /* # log. processors (reference) */
  if (nprocs == -1) return -1;      /* abort if nprocs can't be determined */
  int *phys2;
  int *core2;

  phys = calloc(2*(size_t)nprocs, sizeof(int));
  core = phys + nprocs;
  if (!phys) return -1;

  /* collect physical id and core id of each logical processor */
  fp = fopen("/proc/cpuinfo", "r");
  if (!fp) { free(phys); return -1; }
  for (c = n = 0; (c != EOF) && (n < nprocs); ) {
    if      (fscanf(fp, "physical id : %d", &i) > 0) {
      phys[n] = i;
      if (phys[n] > max_phys) max_phys = phys[n]; }
    else if (fscanf(fp, "core id : %d",     &i) > 0) {
      core[n] = i;
      if (core[n] > max_core) max_core = core[n];
      n++; }
    while (((c = fgetc(fp)) != EOF) && (c != '\n'));
  }
  fclose(fp);
  if (n != nprocs) { free(phys); return -1; }

  /* figure out how many unique physical ids exist */
  phys2 = calloc((size_t)max_phys+1, sizeof(int));
  if (!phys2) { free(phys); return -1; }
  for (i = 0; i < nprocs; i++)
    phys2[phys[i]]++;
  for (i = 0; i < max_phys+1; i++) {
    #ifndef NDEBUG
    printf("physical id: %d -> %d logical processor(s)\n", i, phys2[i]);
    #endif
    if (phys2[i] > 0)
      nphys++;
  }
  #ifndef NDEBUG
  printf("Number of physical processors: %d\n", nphys);
  #endif

  /* figure out how many unique core ids exist */
  core2 = calloc((size_t)max_core+1, sizeof(int));
  if (!core2) { free(phys); free(phys2); return -1; }
  for (i = 0; i < nphys; i++) {
    #ifndef NDEBUG
    printf("physical id: %d\n", i);
    #endif
    for (j = 0; j < nprocs; j++)
      if (phys[j] == i)
        core2[core[j]]++;
    for (j = 0; j < max_core+1; j++) {
      #ifndef NDEBUG
      printf("  core id: %2d -> %d logical processor(s)\n", j, core2[j]);
      #endif
      if (core2[j] > 0)
        tmp++;
    }
    #ifndef NDEBUG
    printf("  -> %d cores\n", tmp);
    #endif
    for (j = 0; j < max_core+1; j++)
      core2[j] = 0;
    ncores += tmp;
    tmp = 0;
  }
  if ((ncores != nprocs) && (ncores != nprocs/2)) {
    free(phys); free(phys2); free(core2); return -1; }

  free(phys);
  free(phys2);
  free(core2);
  return ncores;
}  /* corecnt() */
#elif defined __APPLE__             /* if Apple Mac OS system */
int corecnt (void)
{                                   /* --- number of processor cores */
  int ncores;
  size_t len = sizeof(ncores);
  sysctlbyname("hw.physicalcpu", &ncores, &len, NULL, (size_t)0);
  return ncores;
}  /* corecnt() */
#endif  /* #ifdef __linux__ .. elif defined __APPLE__ ..*/

/*--------------------------------------------------------------------------*/

#ifdef __linux__                    /* if Linux system */
#ifdef _SC_NPROCESSORS_ONLN         /* if glibc's sysconf is available */
int proccnt (void)
{                                   /* --- number of logical processors */
  return (int)sysconf(_SC_NPROCESSORS_ONLN);
}  /* proccnt() */
#endif  /* #ifdef _SC_NPROCESSORS_ONLN */
#elif defined _WIN32                /* if Microsoft Windows system */
int proccnt (void)
{                                   /* --- number of logical processors */
  SYSTEM_INFO sysinfo;              /* system information structure */
  GetSystemInfo(&sysinfo);          /* get system information */
  return sysinfo.dwNumberOfProcessors;
}  /* proccnt() */
#elif defined __APPLE__             /* if Apple Mac OS system */
int proccnt (void)
{                                   /* --- number of logical processors */
  int nproc;
  size_t len = sizeof(nproc);
  sysctlbyname("hw.logicalcpu", &nproc, &len, NULL, (size_t)0);
  return nproc;
}  /* proccnt() */
#endif  /* #ifdef __linux__ .. #elif def. WIN32 .. #elif def. __APPLE__ .. */
/*----------------------------------------------------------------------------
References (proccnt, Windows version):
  Info on SYSTEM_INFO structure:
  msdn.microsoft.com/en-us/library/windows/desktop/ms724958%28v=vs.85%29.aspx
Additional info and references (proccnt, Linux version):
  This function depends on glibc's sysconf extension.
  glibc also provides a function to access _SC_NPROCESSORS_ONLN
  in <sys/sysinfo.h> : get_nprocs which could be use instead of 
  sysconf(_SC_NPROCESSORS_ONLN).
  According to a comment by Joey Adams at stackoverflow.com/q/2693948
  sysconf(_SC_NPROCESSORS_ONLN) relies on /proc/stat if that exist and on
  /proc/cpuinfo if it doesn't.
  gnu.org/software/libc/manual/html_node/Processor-Resources.html
----------------------------------------------------------------------------*/

int proccntmax (void)
{                                   /* --- max. number of logical processors
                                     *     per physical processor */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[1] >> 16) & 0xff; /* EBX[23:16] */
}  /* proccntmax() */
/*----------------------------------------------------------------------------
References (proccntmax):
  "CPUID.1:EBX[23:16] represents the maximum number of addressable IDs
  (initial APIC ID) that can be assigned to logical processors in a
  physical package. The value may not be the same as the number of
  logical processors that are present in the hardware of a physical package."
  software.intel.com/en-us/articles/
    intel-64-architecture-processor-topology-enumeration
  software.intel.com/sites/default/files/63/1a/
    Kuo_CpuTopology_rc1.rh1.final.pdf
----------------------------------------------------------------------------*/

int hasMMX (void)
{                                   /* --- check for MMX instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[3] & (1 << 23)) != 0;
}  /* hasMMX() */

/*--------------------------------------------------------------------------*/

int hasSSE (void)
{                                   /* --- check for SSE instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[3] & (1 << 25)) != 0;
}  /* hasSSE() */

/*--------------------------------------------------------------------------*/

int hasSSE2 (void)
{                                   /* --- check for SSE2 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[3] & (1 << 26)) != 0;
}  /* hasSSE2() */

/*--------------------------------------------------------------------------*/

int hasSSE3 (void)
{                                   /* --- check for SSE3 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 <<  0)) != 0;
}  /* hasSSE3() */

/*--------------------------------------------------------------------------*/

int hasSSSE3 (void)
{                                   /* --- check for SSSE3 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 <<  9)) != 0;
}  /* hasSSSE3() */

/*--------------------------------------------------------------------------*/

int hasSSE41 (void)
{                                   /* --- check for SSE4.1 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 19)) != 0;
}  /* hasSSE41() */

/*--------------------------------------------------------------------------*/

int hasSSE42 (void)
{                                   /* --- check for SSE4.2 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 20)) != 0;
}  /* hasSSE42() */

/*--------------------------------------------------------------------------*/

int hasPOPCNT (void)
{                                   /* --- check for popcnt instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 23)) != 0;
}  /* hasPOPCNT() */

/*--------------------------------------------------------------------------*/

int hasAVX (void)
{                                   /* --- check for AVX instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 28)) != 0;
}  /* hasAVX() */

/*--------------------------------------------------------------------------*/

void getVendorID(char *buf)
{                                   /* --- get vendor id */
  /* the string is going to be exactly 12 characters long, allocate
     the buffer outside this function accordingly */
  int regs[4];
  cpuid(regs, 0);
  ((unsigned *)buf)[0] = (unsigned)regs[1]; // EBX
  ((unsigned *)buf)[1] = (unsigned)regs[3]; // EDX
  ((unsigned *)buf)[2] = (unsigned)regs[2]; // ECX
}
/*----------------------------------------------------------------------------
References (getVendorID):
  stackoverflow.com/a/3082553
----------------------------------------------------------------------------*/

#ifdef CPUINFO_MAIN
int main (int argc, char* argv[])
{
  char vendor[12];
  getVendorID(vendor);
  printf("Vendor             %s\n", vendor);
  #ifndef _WIN32
  printf("Processor cores    %d\n", corecnt());
  #endif
  printf("Logical processors %d\n", proccnt());
  printf("MMX                %d\n", hasMMX());
  printf("SSE                %d\n", hasSSE());;
  printf("SSE2               %d\n", hasSSE2());;
  printf("SSE3               %d\n", hasSSE3());;
  printf("SSSE3              %d\n", hasSSSE3());;
  printf("SSE41              %d\n", hasSSE41());;
  printf("SSE42              %d\n", hasSSE42());;
  printf("POPCNT             %d\n", hasPOPCNT());;
  printf("AVX                %d\n", hasAVX());;

/* corecnt    -> number of processor cores
   proccnt    -> number of logical processors
   proccntmax -> max. number of logical processors per core
   See the glossary at:
     software.intel.com/en-us/articles/
       intel-64-architecture-processor-topology-enumeration */

}  /* main() */
#endif
