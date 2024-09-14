#ifndef INCLUDE_TINYPPP_H_
#define INCLUDE_TINYPPP_H_

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tCore.h"

#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <pthread.h>
#include <sys/select.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN_DLL
#define EXPORT __declspec(dllexport) /* for Windows DLL */
#else
#define EXPORT
#endif

#ifdef WIN32
#define thread_t HANDLE
#define lock_t CRITICAL_SECTION
#define initlock(f) InitializeCriticalSection(f)
#define lock(f) EnterCriticalSection(f)
#define unlock(f) LeaveCriticalSection(f)
#define FILEPATHSEP '\\'
#else
#define thread_t pthread_t
#define lock_t pthread_mutex_t
#define initlock(f) pthread_mutex_init(f, NULL)
#define lock(f) pthread_mutex_lock(f)
#define unlock(f) pthread_mutex_unlock(f)
#define FILEPATHSEP '/'
#endif

    typedef struct
    {                                /* RINEX control struct type */
        gtime_t time;                /* message time */
        double ver;                  /* RINEX version */
        char type;                   /* RINEX file type ('O','N',...) */
        int sys;                     /* navigation system */
        int tsys;                    /* time system */
        char tobs[8][MAXOBSTYPE][4]; /* rinex obs types */
        obs_t obs;                   /* observation data */
        nav_t nav;                   /* navigation data */
        sta_t sta;                   /* station info */
        int ephsat;                  /* input ephemeris satellite number */
        int ephset;                  /* input ephemeris set (0-1) */
        char opt[256];               /* rinex dependent options */
    } rnxctr_t;

    typedef struct
    {                                 /* RINEX options type */
        gtime_t ts, te;               /* time start/end */
        double tint;                  /* time interval (s) */
        double ttol;                  /* time tolerance (s) */
        double tunit;                 /* time unit for multiple-session (s) */
        int rnxver;                   /* RINEX version (x100) */
        int navsys;                   /* navigation system */
        int obstype;                  /* observation type */
        int freqtype;                 /* frequency type */
        char mask[7][64];             /* code mask {GPS,GLO,GAL,QZS,SBS,CMP,IRN} */
        char staid[32];               /* station id for rinex file name */
        char prog[32];                /* program */
        char runby[32];               /* run-by */
        char marker[64];              /* marker name */
        char markerno[32];            /* marker number */
        char markertype[32];          /* marker type (ver.3) */
        char name[2][32];             /* observer/agency */
        char rec[3][32];              /* receiver #/type/vers */
        char ant[3][32];              /* antenna #/type */
        double apppos[3];             /* approx position x/y/z */
        double antdel[3];             /* antenna delta h/e/n */
        double glo_cp_bias[4];        /* GLONASS code-phase biases (m) */
        char comment[MAXCOMMENT][64]; /* comments */
        char rcvopt[256];             /* receiver dependent options */
        uint8_t exsats[MAXSAT];       /* excluded satellites */
        int glofcn[32];               /* glonass fcn+8 */
        int outiono;                  /* output iono correction */
        int outtime;                  /* output time system correction */
        int outleaps;                 /* output leap seconds */
        int autopos;                  /* auto approx position */
        int phshift;                  /* phase shift correction */
        int halfcyc;                  /* half cycle correction */
        int sep_nav;                  /* separated nav files */
        gtime_t tstart;               /* first obs time */
        gtime_t tend;                 /* last obs time */
        gtime_t trtcm;                /* approx log start time for rtcm */
        char tobs[7][MAXOBSTYPE][4];  /* obs types {GPS,GLO,GAL,QZS,SBS,CMP,IRN} */
        double shift[7][MAXOBSTYPE];  /* phase shift (cyc) {GPS,GLO,GAL,QZS,SBS,CMP,IRN} */
        int nobs[7];                  /* number of obs types {GPS,GLO,GAL,QZS,SBS,CMP,IRN} */
    } rnxopt_t;

    typedef struct
    {                          /* stream type */
        int type;              /* type (STR_???) */
        int mode;              /* mode (STR_MODE_?) */
        int state;             /* state (-1:error,0:close,1:open) */
        uint32_t inb, inr;     /* input bytes/rate */
        uint32_t outb, outr;   /* output bytes/rate */
        uint32_t tick_i;       /* input tick tick */
        uint32_t tick_o;       /* output tick */
        uint32_t tact;         /* active tick */
        uint32_t inbt, outbt;  /* input/output bytes at tick */
        lock_t lock;           /* lock flag */
        void *port;            /* type dependent port control struct */
        char path[MAXSTRPATH]; /* stream path */
        char msg[MAXSTRMSG];   /* stream message */
    } stream_t;

    EXPORT int reppath(const char *path, char *rpath, gtime_t time, const char *rov, const char *base);
    EXPORT int reppaths(const char *path, char *rpaths[], int nmax, gtime_t ts, gtime_t te, const char *rov, const char *base);
    /* input and output functions ------------------------------------------------*/
    EXPORT int sortobs(obs_t *obs);
    EXPORT void uniqnav(nav_t *nav);
    EXPORT int screent(gtime_t time, gtime_t ts, gtime_t te, double tint);
    EXPORT int readnav(const char *file, nav_t *nav);
    EXPORT int savenav(const char *file, const nav_t *nav);
    EXPORT void freeobs(obs_t *obs);
    EXPORT void freenav(nav_t *nav, int opt);

    /* platform dependent functions ----------------------------------------------*/
    EXPORT int execcmd(const char *cmd);
    EXPORT int expath(const char *path, char *paths[], int nmax);
    EXPORT void createdir(const char *path);
    EXPORT int rtk_uncompress(const char *file, char *uncfile);
    /* antenna models ------------------------------------------------------------*/
    extern void read_Antatx(prcopt_t *opt, const char *file, gtime_t t_s, gtime_t t_e, nav_t *nav);

    /* rinex functions */
    EXPORT int readrnx(const char *file, int rcv, const char *opt, obs_t *obs, nav_t *nav, sta_t *sta);
    EXPORT int readrnxt(const char *file, int rcv, gtime_t ts, gtime_t te, double tint, const char *opt, obs_t *obs, nav_t *nav, sta_t *sta);
    EXPORT int readrnxc(const char *file, nav_t *nav);
    EXPORT int readrnxh(FILE *fp, double *ver, char *type, int *sys, int *tsys, char tobs[][MAXOBSTYPE][4], nav_t *nav, sta_t *sta);
    EXPORT void free_rnxctr(rnxctr_t *rnx);
    EXPORT int readrnxobsb_ex(FILE *fp, gtime_t *time, const char *opt, double ver, int *tsys, char tobs[][MAXOBSTYPE][4], int *flag, obs_t *obs, sta_t *sta);
    EXPORT int readrnxnav(FILE *fp, const char *opt, double ver, int sys, nav_t *nav);
    EXPORT int read_upd_epoch(const prcopt_t *opt, const char *infile, gtime_t t_r, nav_t *nav, int b_reopen, int usetype[MAXSYS][NFREQ]);
    EXPORT int readcbias(char **infile, int n, prcopt_t *prcopt, nav_t *nav);
    EXPORT int read_osb_epoch(prcopt_t *opt, const char *file, gtime_t t_r, nav_t *nav, int b_reopen);

    /* stream data input and output functions ------------------------------------*/
    EXPORT void strinitcom(void);
    EXPORT void strinit(stream_t *stream);
    EXPORT void strlock(stream_t *stream);
    EXPORT void strunlock(stream_t *stream);
    EXPORT int stropen(stream_t *stream, int type, int mode, const char *path);
    EXPORT void strclose(stream_t *stream);
    EXPORT int strread(stream_t *stream, uint8_t *buff, int n);
    EXPORT int strwrite(stream_t *stream, uint8_t *buff, int n);
    EXPORT void strsync(stream_t *stream1, stream_t *stream2);
    EXPORT int strstat(stream_t *stream, char *msg);
    EXPORT int strstatx(stream_t *stream, char *msg);
    EXPORT void strsum(stream_t *stream, int *inb, int *inr, int *outb, int *outr);
    EXPORT void strsetopt(const int *opt);
    EXPORT gtime_t strgettime(stream_t *stream);
    EXPORT void strsendnmea(stream_t *stream, const sol_t *sol);
    EXPORT void strsendcmd(stream_t *stream, const char *cmd);
    EXPORT void strsettimeout(stream_t *stream, int toinact, int tirecon);
    EXPORT void strsetdir(const char *dir);
    EXPORT void strsetproxy(const char *addr);

    /* Data save */
    extern FILE *getFileptr(gtime_t td, filopt_t *fopt, prcopt_t *opt, char *type);
    extern void outputeph(eph_t *eph, FILE *fp);
    extern void outputobs(obsd_t *obt, int n, prcopt_t *opt, FILE *fp);
    extern void outputssr(ssr_t *ssr);

    /* processing */
    extern int post_process(gtime_t ts, gtime_t te, double ti, prcopt_t *popt, const solopt_t *sopt, const filopt_t *fopt, char **infile, int n, char *outfile);
    void realtime_process(prcopt_t *popt, const solopt_t *sopt, const filopt_t *fopt, char sstream[4][1024]);
#ifdef __cplusplus
}
#endif
#endif