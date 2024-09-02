/*
 * @Author: juntao at wuhan university
 * @Date: 2022-09-12 23:07:39
 * @Last Modified by: zgj
 * @Last Modified time: 2022-09-21 16:49:21
 */
#ifndef INCLUDE_TINYCOREPUBLIC_H_
#define INCLUDE_TINYCOREPUBLIC_H_

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "list.h"

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

    /* operation control */
#define KPL_MAXSYS 5 /* max system */
#define KPL_NFREQ 4  /* number of carrier frequencies */
#define KPL_MAXSAT 116
#define KPL_LEN_FREQ 4
#define KPL_LEN_PRN 8
#define KPL_MAXCODE 68 /* max number of obs code */

    typedef struct
    {                /* time struct */
        time_t time; /* time (s) expressed by standard time_t */
        double sec;  /* fraction of second under 1 s */
    } kpl_gtime_t;

    typedef struct
    {                            /* observation data record */
        kpl_gtime_t time;        /* receiver sampling time (GPST) */
        uint8_t sat, rcv;        /* satellite/receiver number */
        uint16_t SNR[KPL_NFREQ]; /* signal strength (0.001 dBHz) */
        uint8_t LLI[KPL_NFREQ];  /* loss of lock indicator */
        uint8_t code[KPL_NFREQ]; /* code indicator (CODE_???) */
        double L[KPL_NFREQ];     /* observation data carrier-phase (cycle) */
        double P[KPL_NFREQ];     /* observation data pseudorange (m) */
        float D[KPL_NFREQ];      /* observation data doppler frequency (Hz) */

        kpl_gtime_t eventime;    /* time of event (GPST) */
        uint8_t Lstd[KPL_NFREQ]; /* stdev of carrier phase (0.004 cycles)  */
        uint8_t Pstd[KPL_NFREQ]; /* stdev of pseudorange (0.01*2^(n+5) meters) */
        uint8_t freq;            /* GLONASS frequency channel (0-13) */
    } kpl_obsd_t;

    typedef struct
    {                     /* observation data */
        int n, nmax;      /* number of obervation data/allocated */
        int flag;         /* epoch flag (0:ok,1:power failure,>1:event flag) */
        kpl_obsd_t *data; /* observation data records */
    } kpl_obs_t;

    typedef struct                 // by zzy
    {                              /* SSR correction type */
        kpl_gtime_t t0[7];         /* epoch time (GPST) {eph,clk,hrclk,ura,bias,pbias,ifpb} */
        double udi[7];             /* SSR update interval (s) */
        int iod[7];                /* iod ssr {eph,clk,hrclk,ura,bias,pbias,ifpb} */
        int iode;                  /* issue of data */
        int iodcrc;                /* issue of data crc for beidou/sbas */
        int ura;                   /* URA indicator */
        int refd;                  /* sat ref datum (0:ITRF,1:regional) */
        double deph[3];            /* delta orbit {radial,along,cross} (m) */
        double ddeph[3];           /* dot delta orbit {radial,along,cross} (m/s) */
        double dclk[3];            /* delta clock {c0,c1,c2} (m,m/s,m/s^2) */
        double hrclk;              /* high-rate clock corection (m) */
        double ifpb;               /* ifpb for GPS satellites */
        double quaterniond[4];     /* Quaterniond for satellites */
        kpl_gtime_t ifpb_t0;       /* t0 for ifpb */
        float cbias[KPL_MAXCODE];  /* code biases (m) */
        double pbias[KPL_MAXCODE]; /* phase biases (m) */
        float stdpb[KPL_MAXCODE];  /* std-dev of phase biases (m) */
        double quateratt[4];       /* attitude quaterniond, w/x/y/z */
        double yaw_ang, yaw_rate;  /* yaw angle and yaw rate (deg,deg/s) */
        uint8_t update;            /* update flag (0:no update,1:update) */
    } kpl_ssr_t;

    typedef struct
    {                              /* GPS/QZS/GAL broadcast ephemeris type */
        int sat;                   /* satellite number */
        int iode, iodc;            /* IODE,IODC */
        int sva;                   /* SV accuracy (URA index) */
        int svh;                   /* SV health (0:ok) */
        int week;                  /* GPS/QZS: gps week, GAL: galileo week */
        int code;                  /* GPS/QZS: code on L2 */
                                   /* GAL: data source defined as rinex 3.03 */
                                   /* BDS: data source (0:unknown,1:B1I,2:B1Q,3:B2I,4:B2Q,5:B3I,6:B3Q) */
        int flag;                  /* GPS/QZS: L2 P data flag */
                                   /* BDS: nav type (0:unknown,1:IGSO/MEO,2:GEO) */
        kpl_gtime_t toe, toc, ttr; /* Toe,Toc,T_trans, in GPST  */
                                   /* SV orbit parameters */
        double A, e, i0, OMG0, omg, M0, deln, OMGd, idot;
        double crc, crs, cuc, cus, cic, cis;
        double toes;       /* Toe (s) in week, in GPST(GE)/BDST(C) */
        double fit;        /* fit interval (h) */
        double f0, f1, f2; /* SV clock parameters (af0,af1,af2) */
        double tgd[6];     /* group delay parameters */
                           /* GPS/QZS:tgd[0]=TGD */
                           /* GAL:tgd[0]=BGD_E1E5a,tgd[1]=BGD_E1E5b */
                           /* CMP:tgd[0]=TGD_B1I ,tgd[1]=TGD_B2I/B2b,tgd[2]=TGD_B1Cp */
                           /*     tgd[3]=TGD_B2ap,tgd[4]=ISC_B1Cd   ,tgd[5]=ISC_B2ad */
        double Adot, ndot; /* Adot,ndot for CNAV */
    } kpl_eph_t;

    typedef struct
    {                      /* GLONASS broadcast ephemeris type */
        int sat;           /* satellite number */
        int iode;          /* IODE (0-6 bit of tb field) */
        int frq;           /* satellite frequency number */
        int svh, sva, age; /* satellite health, accuracy, age of operation */
        kpl_gtime_t toe;   /* epoch of epherides (gpst) */
        kpl_gtime_t tof;   /* message frame time (gpst) */
        double pos[3];     /* satellite position (ecef) (m) */
        double vel[3];     /* satellite velocity (ecef) (m/s) */
        double acc[3];     /* satellite acceleration (ecef) (m/s^2) */
        double taun, gamn; /* SV clock bias (s)/relative freq bias */
        double dtaun;      /* delay between L1 and L2 (s) */
    } kpl_geph_t;

    typedef struct
    {                              /* precise ephemeris type */
        kpl_gtime_t time;          /* time (GPST) */
        int index;                 /* ephemeris index for multiple files */
        double pos[KPL_MAXSAT][4]; /* satellite position/clock (ecef) (m|s) */
        float std[KPL_MAXSAT][4];  /* satellite position/clock std (m|s) */
        double vel[KPL_MAXSAT][4]; /* satellite velocity/clk-rate (m/s|s/s) */
        float vst[KPL_MAXSAT][4];  /* satellite velocity/clk-rate std (m/s|s/s) */
        float cov[KPL_MAXSAT][3];  /* satellite position covariance (m^2) */
        float vco[KPL_MAXSAT][3];  /* satellite velocity covariance (m^2) */
    } kpl_peph_t;

    typedef struct
    {                              /* precise clock type */
        kpl_gtime_t time;          /* time (GPST) */
        int index;                 /* clock index for multiple files */
        double clk[KPL_MAXSAT][1]; /* satellite clock (s) */
        float std[KPL_MAXSAT][1];  /* satellite clock std (s) */
    } kpl_pclk_t;

    typedef struct
    {                    /* SBAS ephemeris type */
        int sat;         /* satellite number */
        kpl_gtime_t t0;  /* reference epoch time (GPST) */
        kpl_gtime_t tof; /* time of message frame (GPST) */
        int sva;         /* SV accuracy (URA index) */
        int svh;         /* SV health (0:ok) */
        double pos[3];   /* satellite position (m) (ecef) */
        double vel[3];   /* satellite velocity (m/s) (ecef) */
        double acc[3];   /* satellite acceleration (m/s^2) (ecef) */
        double af0, af1; /* satellite clock-offset/drift (s,s/s) */
    } kpl_seph_t;

    typedef struct
    {
        int isel;                 /* whether isel */
        int sat;                  /* satellite number (0:receiver) */
        char type[64];            /* antenna type */
        char code[64];            /* serial number or satellite code */
        kpl_gtime_t ts, te;       /* valid time start and end */
        double off[KPL_NFREQ][3]; /* phase center offset e/n/u or x/y/z (m) */
    } kpl_spcv_t;

    typedef struct
    {                                /* navigation data type */
        int n, nmax;                 /* number of broadcast ephemeris */
        int ng, ngmax;               /* number of glonass ephemeris */
        int ns, nsmax;               /* number of sbas ephemeris */
        int ne, nemax;               /* number of precise ephemeris */
        int nc, ncmax;               /* number of precise clock */
        int na, namax;               /* number of precise quad products  */
        int np, nmaxp;               /* current count and maximum count of upd */
        int ni, nmaxi;               /* current count and maximum count of ifpb */
        kpl_eph_t *eph;              /* GPS/QZS/GAL/BDS/IRN ephemeris */
        kpl_geph_t *geph;            /* GLONASS ephemeris */
        kpl_seph_t *seph;            /* SBAS ephemeris */
        kpl_peph_t *peph;            /* precise ephemeris */
        kpl_pclk_t *pclk;            /* precise clock */
        double utc_gps[8];           /* GPS delta-UTC parameters {A0,A1,Tot,WNt,dt_LS,WN_LSF,DN,dt_LSF} */
        double utc_glo[8];           /* GLONASS UTC time parameters {tau_C,tau_GPS} */
        double utc_gal[8];           /* Galileo UTC parameters */
        double utc_qzs[8];           /* QZS UTC parameters */
        double utc_cmp[8];           /* BeiDou UTC parameters */
        double utc_irn[9];           /* IRNSS UTC parameters {A0,A1,Tot,...,dt_LSF,A2} */
        double utc_sbs[4];           /* SBAS UTC parameters */
        double ion_gps[8];           /* GPS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
        double ion_gal[4];           /* Galileo iono model parameters {ai0,ai1,ai2,0} */
        double ion_qzs[8];           /* QZSS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
        double ion_cmp[8];           /* BeiDou iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
        double ion_irn[8];           /* IRNSS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
        int glo_fcn[32];             /* GLONASS FCN + 8 */
        double cbias[KPL_MAXSAT][3]; /* satellite DCB (0:P1-P2,1:P1-C1,2:P2-C2) (m) */
        kpl_spcv_t *pcvs;            /* satellite antenna pcv */
        kpl_ssr_t ssr[KPL_MAXSAT];   /* SSR corrections, for real-time  */
    } kpl_nav_t;

    typedef struct
    {                       /* solution options type */
        int posf;           /* solution format (SOLF_???) */
        int times;          /* time system (TIMES_???) */
        int timef;          /* time format (0:sssss.s,1:yyyy/mm/dd hh:mm:ss.s) */
        int timeu;          /* time digits under decimal point */
        int degf;           /* latitude/longitude format (0:ddd.ddd,1:ddd mm ss) */
        int outhead;        /* output header (0:no,1:yes) */
        int outopt;         /* output processing options (0:no,1:yes) */
        int outvel;         /* output velocity options (0:no,1:yes) */
        int datum;          /* datum (0:WGS84,1:Tokyo) */
        int height;         /* height (0:ellipsoidal,1:geodetic) */
        int geoid;          /* geoid model (0:EGM96,1:JGD2000) */
        int solstatic;      /* solution of static mode (0:all,1:single) */
        int sstat;          /* solution statistics level (0:off,1:states,2:residuals) */
        int trace;          /* debug trace level (0:off,1-5:debug) */
        double nmeaintv[2]; /* nmea output interval (s) (<0:no,0:all) */
                            /* nmeaintv[0]:gprmc,gpgga,nmeaintv[1]:gpgsv */
        char sep[64];       /* field separator */
        char prog[64];      /* program name */
        double maxsolstd;   /* max std-dev for solution output (m) (0:all) */
    } kpl_solopt_t;

    typedef struct
    {                      /* file options type */
        char satantp[256]; /* satellite antenna parameters file */
        char rcvantp[256]; /* receiver antenna parameters file */
        char stapos[256];  /* station positions file */
        char geoid[256];   /* external geoid data file */
        char gptf[256];    /* GPT2 file */
        char iono[256];    /* ionosphere data file */
        char osb[256];     /* osb data file */
        char eop[256];     /* eop data file */
        char blq[256];     /* ocean tide loading blq file */
        char tempdir[256]; /* ftp/http temporaly directory */
        char geexe[256];   /* google earth exec file */
        char solstat[256]; /* solution statistics file */
        char trace[256];   /* debug trace file */
        char outdir[256];  /* output directory */
    } kpl_filopt_t;

    typedef struct
    {                          /* station parameter type */
        char name[64];         /* marker name */
        char marker[64];       /* marker number */
        char antdes[64];       /* antenna descriptor */
        char antsno[64];       /* antenna serial number */
        char rectype[64];      /* receiver type descriptor */
        char recver[64];       /* receiver firmware version */
        char recsno[64];       /* receiver serial number */
        int antsetup;          /* antenna setup id */
        int itrf;              /* ITRF realization year */
        int deltype;           /* antenna delta type (0:enu,1:xyz) */
        double pos[3];         /* station position (ecef) (m) */
        double del[3];         /* antenna position delta (e/n/u or x/y/z) (m) */
        double hgt;            /* antenna height (m) */
        int glo_cp_align;      /* GLONASS code-phase alignment (0:no,1:yes) */
        double glo_cp_bias[4]; /* GLONASS code-phase biases {1C,1P,2C,2P} (m) */
    } kpl_sta_t;

    typedef struct
    {                      /* solution of KPL type */
        int week;          /* GPS week */
        double sow;        /* second in GPS week */
        double ep[6];      /* yyddmmhhmmss */
        double vdop[5];    /* tdop pdop hdop vdop NULL */
        double sig[3];     /* sigma of enu */
        double enu[3];     /* residual of enu */
        double xyz[3];     /* xyz */
        double blh[3];     /* blh */
        uint8_t ns_track;  /* nsat of tracked */
        uint8_t ns;        /* nsat of used */
        uint8_t stat;      /* solution state */
        float t_delay_ssr; /* time delay of SSR */
    } kpl_sol_t;

    extern const kpl_solopt_t solopt_default; /* default solution output options */

    extern void KPL_initialize();
    extern void KPL_finilize();
    extern void KPL_restart();
    extern void KPL_setOption(char *key, void *value);
    extern int KPL_inputObs(kpl_gtime_t, kpl_obs_t *obs);
    extern void KPL_inputEph(kpl_nav_t *nav, int psat, int offset);
    extern void KPL_inputSsr(kpl_ssr_t *ssr, int solid);
    extern void KPL_inputSsrBias(kpl_ssr_t *ssr);
    extern void KPL_getNMEA(const char *, char *buff, int len);
    extern void KPL_getSolution(kpl_sol_t *sol);

    /* for kpl_satno.c */
    extern int kpl_satno(int sys, int prn);
    extern int kpl_cprn2sat(const char *id);
    extern void kpl_sat2cprn(int sat, char *id);
    extern uint8_t kpl_obs2code(const char *obs);
    extern char *kpl_code2obs(uint8_t code);

    /* atmosphere augment */
    typedef struct
    {
        int len;
        int outtype;        /* output message type */
        char msgtype[256];  /* last message type */
        uint8_t buff[1200]; /* buffer */
        kpl_gtime_t time;   /* message time */
    } kpl_atmo_rtcm_t;

    typedef struct
    {
        int mjd;        /* modify jualian day */
        double sod;     /* second of day */
        double coef[7]; /* ZTD model coefficient */
        double height;  /* model height */
        double sigma;   /* sigma */
    } kpl_ztdmodel_t;

    typedef struct
    {
        kpl_gtime_t time;
        double zhd; /* zhd */
        double zwd; /* zwd */
    } kpl_ztdvmf_t;

    typedef struct
    {
        kpl_gtime_t time;           /* time of model */
        int n;                      /* degree of latitude */
        int m;                      /* degree of longitude */
        kpl_gtime_t t0;             /* central time */
        double lat0;                /* central latitude */
        double lon0;                /* central longitude */
        double coef[KPL_MAXSAT][9]; /* model coeffecient */
        float sig[KPL_MAXSAT];      /* model sigma */
        /*解码新增*/
        int nsat;
        int udint;
        int iod;
        int sync;
        double tow; /*没有使用*/
        char cprn[KPL_MAXSAT][KPL_LEN_PRN];
    } kpl_ionmodel_t;

    typedef struct
    {
        kpl_gtime_t time;         /* time of atmo (GPST) */
        int id;                   /* index */
        double geod[3];           /* station blh */
        float ionval[KPL_MAXSAT]; /* ionospher correction value */
        float trpval[KPL_MAXSAT]; /* troposphere correction value */
        float ionsig[KPL_MAXSAT]; /* ionospher correction sigma */
        float trpsig[KPL_MAXSAT]; /* troposphere correction sigma */
        /*解码新增*/
        int nsat;
        int dtype;
        int udint;
        int iod;
        int sync;
        // int solid;
        char cprn[KPL_MAXSAT][KPL_LEN_PRN];
        double elev[KPL_MAXSAT];

    } kpl_iongrid_t;

    extern int KPL_decode_ssr9_grid(kpl_atmo_rtcm_t *rtcm, int sys, int subtype);
    extern int KPL_decode_ssr9_model(kpl_atmo_rtcm_t *rtcm, int sys, int subtype);
    extern float KPL_getIonGrid(int isat);
    extern float KPL_getIonModel(int isat, double ipplat, double ipplon, kpl_gtime_t t);

#ifdef __cplusplus
}
#endif
#endif
