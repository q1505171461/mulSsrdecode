/*
 * @Author: juntao at wuhan university
 * @Date: 2022-09-12 23:07:39
 * @Last Modified by: zgj
 * @Last Modified time: 2022-09-21 16:49:21
 */
#ifndef INCLUDE_TINYCORE_H_
#define INCLUDE_TINYCORE_H_

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "list.h"

// #define ARM 1
#ifdef ARM
#include "ks_printf.h"
#endif

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
#define China_GPT 1 /* ENABLE CHINA TROPOSPHERE */
// #define MKL
// #define PC_FILE

/* memory control for PPP*/
#define MAXUSEDSAT (9 + 9 + 9 + 9)                                                    /* GPS BD2 BD3 GAL */
#define MAXUPDATEPAM (3 + MAXSYS + 1 + MAXSYS + MAXUSEDSAT + 2)                       /* MAXIMUM parameters to update */
#define MAXRESETOBS (MAXUSEDSAT * 3 / 2)                                              /* MAXIMUM reset number of ambiguity */
#define MAXEPOBS (MAXUSEDSAT * 6)                                                     /* MAXIMUM observation count per epoch*/
#define MAXIMTX (3 + MAXSYS + 1 + MAXSYS + MAXUSEDSAT + MAXUSEDSAT * 3 + MAXRESETOBS) /* MAXIMUM X of infs*/
#define MAXIMTXY (MAXIMTX + MAXUSEDSAT * 6)                                           /* MAXIMUM Y of infs */
#define DIM_INFUX (MAXUPDATEPAM * 2 + MAXUSEDSAT * 3)                                 /* column count */
#define DIM_INFUY (MAXUPDATEPAM * 2)                                                  /* row count */

#define MAXSYS 5    /* max system */
#define SYS "GECBJ" /* system tag for processed */

#define MAXTYPES 50
#define MAXTYPE 20

#define Scalar double  /* scalar for data type */
#define kScalar double /* scalar for kalman filter */

#define _IF_0(f1, f2) ((f1 * f1) / (f1 * f1 - f2 * f2))
#define _IF_1(f1, f2) (-(f2 * f2) / (f1 * f1 - f2 * f2))
#define _WL_0(f1, f2) (f1 / (f1 - f2))
#define _WL_1(f1, f2) (-f2 / (f1 - f2))
#define _NL_0(f1, f2) (f1 / (f1 + f2))
#define _NL_1(f1, f2) (f2 / (f1 + f2))

#define SIGN(a, b) (b > 0 ? a : -a)
#define NINT(a) ((int)(a + SIGN(1, a) * 0.5))

#define SQR(x) ((x) * (x))
#define ROUND(x) (floor((x) + 0.5))
#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define SWAP(x, y) \
    do             \
    {              \
        int tmp_;  \
        tmp_ = x;  \
        x = y;     \
        y = tmp_;  \
    } while (0)

#define COPYRIGHT_TINYPPP \
    "Copyright (C) 2022- Jun Tao wuhan university\nAll rights reserved."

#define VER_TINYPPP "ver1.1"

#define MALLOC malloc
#define CALLOC calloc

#define FREE free

#define PI 3.1415926535897932  /* pi */
#define D2R (PI / 180.0)       /* deg to rad */
#define R2D (180.0 / PI)       /* rad to deg */
#define CLIGHT 299792458.0     /* speed of light (m/s) */
#define SC2RAD 3.1415926535898 /* semi-circle to radian (IS-GPS) */
#define AU 149597870691.0      /* 1 AU (m) */
#define AS2R (D2R / 3600.0)    /* arc sec to radian */

#define OMGE 7.2921151467E-5 /* earth angular velocity (IS-GPS) (rad/s) */

#define RE_WGS84 6378137.0             /* earth semimajor axis (WGS84) (m) */
#define FE_WGS84 (1.0 / 298.257223563) /* earth flattening (WGS84) */

#define HION 350000.0 /* ionosphere height (m) */

#define E_ROTATE 7.2921151467E-5

#define MAXFREQ 6 /* max NFREQ */

#define GME 3.986004415E+14 /* earth gravitational constant */

#define FREQL1 1.57542E9      /* L1/E1  frequency (Hz) */
#define FREQL2 1.22760E9      /* L2     frequency (Hz) */
#define FREQE5b 1.20714E9     /* E5b    frequency (Hz) */
#define FREQL5 1.17645E9      /* L5/E5a/B2a frequency (Hz) */
#define FREQL6 1.27875E9      /* E6/L6 frequency (Hz) */
#define FREQE5ab 1.191795E9   /* E5a+b  frequency (Hz) */
#define FREQs 2.492028E9      /* S      frequency (Hz) */
#define FREQ1_GLO 1.60200E9   /* GLONASS G1 base frequency (Hz) */
#define DFRQ1_GLO 0.56250E6   /* GLONASS G1 bias frequency (Hz/n) */
#define FREQ2_GLO 1.24600E9   /* GLONASS G2 base frequency (Hz) */
#define DFRQ2_GLO 0.43750E6   /* GLONASS G2 bias frequency (Hz/n) */
#define FREQ3_GLO 1.202025E9  /* GLONASS G3 frequency (Hz) */
#define FREQ1a_GLO 1.600995E9 /* GLONASS G1a frequency (Hz) */
#define FREQ2a_GLO 1.248060E9 /* GLONASS G2a frequency (Hz) */
#define FREQ1_CMP 1.561098E9  /* BDS B1I     frequency (Hz) */
#define FREQ2_CMP 1.20714E9   /* BDS B2I/B2b frequency (Hz) */
#define FREQ3_CMP 1.26852E9   /* BDS B3      frequency (Hz) */

#define SYS_NONE 0x00 /* navigation system: none */
#define SYS_GPS 0x01  /* navigation system: GPS */
#define SYS_SBS 0x02  /* navigation system: SBAS */
#define SYS_GLO 0x04  /* navigation system: GLONASS */
#define SYS_GAL 0x08  /* navigation system: Galileo */
#define SYS_QZS 0x10  /* navigation system: QZSS */
#define SYS_CMP 0x20  /* navigation system: BeiDou */
#define SYS_IRN 0x40  /* navigation system: IRNS */
#define SYS_LEO 0x80  /* navigation system: LEO */
#define SYS_ALL 0xFF  /* navigation system: all */

#define TSYS_GPS 0 /* time system: GPS time */
#define TSYS_UTC 1 /* time system: UTC */
#define TSYS_GLO 2 /* time system: GLONASS time */
#define TSYS_GAL 3 /* time system: Galileo time */
#define TSYS_QZS 4 /* time system: QZSS time */
#define TSYS_CMP 5 /* time system: BeiDou time */
#define TSYS_IRN 6 /* time system: IRNSS time */

#ifndef NFREQ
#define NFREQ 4 /* number of carrier frequencies */
#endif
#define NFREQGLO 2 /* number of carrier frequencies of GLONASS */

#ifndef NEXOBS
#define NEXOBS 0 /* number of extended obs codes */
#endif

#define SNR_UNIT 0.001 /* SNR unit (dBHz) */

#define MAXPRNGPS 32
#define MAXPRNGAL 32
#define MAXPRNQZS 5
#define MAXPRNCMP 47
#define MAXPRNIRN 0
#define MAXPRNSBS 0
#define MAXPRNLEO 0
#define MAXPRNGLO 0   // 27
#define MINPRNQZS 193 // by zzy
#define MINPRNSBS 120 // by zzy

#define CONS_NONE 0 /* no constrain for amb resolution */
#define CONS_EWL 1  /* ewl constrain for amb resolution */
#define CONS_WL 2   /* wl constrain for amb resolution */
#define CONS_N1 3   /* n1 constrain for amb resolution */

#define NSATGPS (MAXPRNGPS)
#define NSATGLO (MAXPRNGLO)
#define NSATGAL (MAXPRNGAL)
#define NSATQZS (MAXPRNQZS)
#define NSATCMP (MAXPRNCMP)
#define NSATIRN (MAXPRNIRN)
#define NSATSBS (MAXPRNSBS)
#define NSATLEO (MAXPRNLEO)

/* max satellite number (1 to MAXSAT) */
#define MAXSAT (MAXPRNGPS + MAXPRNGLO + MAXPRNGAL + MAXPRNQZS + MAXPRNCMP + MAXPRNIRN + MAXPRNSBS + MAXPRNLEO)

#define LEN_PRN 8 /* length of prn slot char */
#define LEN_FREQ 4
#define Max_Dimension 64800
#define TYPERA 1    /* range observation */
#define TYPEPH 2    /* phase observation */
#define MAXPHCOMB 3 /* phase bias combination */

#ifndef MAXOBS
#define MAXOBS 96 /* max number of obs in an epoch */
#endif

#define NIDXC 3
#define MAXPAM 20
#define MAXRCV 2      /* max receiver number (1 to MAXRCV) */
#define MAXOBSTYPE 64 /* max number of obs type in RINEX */
#ifdef OBS_100HZ
#define DTTOL 0.005 /* tolerance of time difference (s) */
#else
#define DTTOL 0.025 /* tolerance of time difference (s) */
#endif

#define MAXDTOE 7200.0      /* max time difference to GPS Toe (s) */
#define MAXDTOE_QZS 7200.0  /* max time difference to QZSS Toe (s) */
#define MAXDTOE_GAL 14400.0 /* max time difference to Galileo Toe (s) */
#define MAXDTOE_CMP 21600.0 /* max time difference to BeiDou Toe (s) */
#define MAXDTOE_GLO 1800.0  /* max time difference to GLONASS Toe (s) */
#define MAXDTOE_IRN 7200.0  /* max time difference to IRNSS Toe (s) */
#define MAXDTOE_SBS 360.0   /* max time difference to SBAS Toe (s) */
#define MAXDTOE_S 86400.0   /* max time difference to ephem toe (s) for other */

#define INT_SWAP_TRAC 86400.0 /* swap interval of trace file (s) */
#define INT_SWAP_STAT 86400.0 /* swap interval of solution status file (s) */

#define MAXEXFILE 1024    /* max number of expanded files */
#define MAXSBSAGEF 30.0   /* max age of SBAS fast correction (s) */
#define MAXSBSAGEL 1800.0 /* max age of SBAS long term corr (s) */
#define MAXSBSURA 8       /* max URA of SBAS satellite */
#define MAXBAND 10        /* max SBAS band of IGP */
#define MAXNIGP 201       /* max number of IGP in SBAS band */
#define MAXNGEO 4         /* max number of GEO satellites */
#define MAXCOMMENT 100    /* max number of RINEX comments */
#define MAXSTRPATH 1024   /* max length of stream path */
#define MAXSTRMSG 1024    /* max length of stream message */
#define MAXSTRRTK 8       /* max number of stream in RTK server */
#define MAXSBSMSG 32      /* max number of SBAS msg in RTK server */
#define MAXSOLMSG 8191    /* max length of solution message */
#define MAXRAWLEN 16384   /* max length of receiver raw message */
#define MAXERRMSG 4096    /* max length of error/warning message */
#define MAXANT 64         /* max length of station name/antenna type */
#define MAXSOLBUF 256     /* max number of solution buffer */
#define MAXOBSBUF 128     /* max number of observation data buffer */
#define MAXNRPOS 16       /* max number of reference positions */
#define MAXLEAPS 64       /* max number of leap seconds table */
#define MAXGISLAYER 32    /* max number of GIS data layers */
#define MAXRCVCMD 4096    /* max length of receiver commands */

#define RNX2VER 2.10 /* RINEX ver.2 default output version */
#define RNX3VER 3.00 /* RINEX ver.3 default output version */

#define CODE_NONE 0 /* obs code: none or unknown */
#define CODE_L1C 1  /* obs code: L1C/A,G1C/A,E1C (GPS,GLO,GAL,QZS,SBS) */
#define CODE_L1P 2  /* obs code: L1P,G1P,B1P (GPS,GLO,BDS) */
#define CODE_L1W 3  /* obs code: L1 Z-track (GPS) */
#define CODE_L1Y 4  /* obs code: L1Y        (GPS) */
#define CODE_L1M 5  /* obs code: L1M        (GPS) */
#define CODE_L1N 6  /* obs code: L1codeless,B1codeless (GPS,BDS) */
#define CODE_L1S 7  /* obs code: L1C(D)     (GPS,QZS) */
#define CODE_L1L 8  /* obs code: L1C(P)     (GPS,QZS) */
#define CODE_L1E 9  /* (not used) */
#define CODE_L1A 10 /* obs code: E1A,B1A    (GAL,BDS) */
#define CODE_L1B 11 /* obs code: E1B        (GAL) */
#define CODE_L1X 12 /* obs code: E1B+C,L1C(D+P),B1D+P (GAL,QZS,BDS) */
#define CODE_L1Z 13 /* obs code: E1A+B+C,L1S (GAL,QZS) */
#define CODE_L2C 14 /* obs code: L2C/A,G1C/A (GPS,GLO) */
#define CODE_L2D 15 /* obs code: L2 L1C/A-(P2-P1) (GPS) */
#define CODE_L2S 16 /* obs code: L2C(M)     (GPS,QZS) */
#define CODE_L2L 17 /* obs code: L2C(L)     (GPS,QZS) */
#define CODE_L2X 18 /* obs code: L2C(M+L),B1_2I+Q (GPS,QZS,BDS) */
#define CODE_L2P 19 /* obs code: L2P,G2P    (GPS,GLO) */
#define CODE_L2W 20 /* obs code: L2 Z-track (GPS) */
#define CODE_L2Y 21 /* obs code: L2Y        (GPS) */
#define CODE_L2M 22 /* obs code: L2M        (GPS) */
#define CODE_L2N 23 /* obs code: L2codeless (GPS) */
#define CODE_L5I 24 /* obs code: L5I,E5aI   (GPS,GAL,QZS,SBS) */
#define CODE_L5Q 25 /* obs code: L5Q,E5aQ   (GPS,GAL,QZS,SBS) */
#define CODE_L5X 26 /* obs code: L5I+Q,E5aI+Q,L5B+C,B2aD+P (GPS,GAL,QZS,IRN,SBS,BDS) */
#define CODE_L7I 27 /* obs code: E5bI,B2bI  (GAL,BDS) */
#define CODE_L7Q 28 /* obs code: E5bQ,B2bQ  (GAL,BDS) */
#define CODE_L7X 29 /* obs code: E5bI+Q,B2bI+Q (GAL,BDS) */
#define CODE_L6A 30 /* obs code: E6A,B3A    (GAL,BDS) */
#define CODE_L6B 31 /* obs code: E6B        (GAL) */
#define CODE_L6C 32 /* obs code: E6C        (GAL) */
#define CODE_L6X 33 /* obs code: E6B+C,LEXS+L,B3I+Q (GAL,QZS,BDS) */
#define CODE_L6Z 34 /* obs code: E6A+B+C,L6D+E (GAL,QZS) */
#define CODE_L6S 35 /* obs code: L6S        (QZS) */
#define CODE_L6L 36 /* obs code: L6L        (QZS) */
#define CODE_L8I 37 /* obs code: E5abI      (GAL) */
#define CODE_L8Q 38 /* obs code: E5abQ      (GAL) */
#define CODE_L8X 39 /* obs code: E5abI+Q,B2abD+P (GAL,BDS) */
#define CODE_L2I 40 /* obs code: B1_2I      (BDS) */
#define CODE_L2Q 41 /* obs code: B1_2Q      (BDS) */
#define CODE_L6I 42 /* obs code: B3I        (BDS) */
#define CODE_L6Q 43 /* obs code: B3Q        (BDS) */
#define CODE_L3I 44 /* obs code: G3I        (GLO) */
#define CODE_L3Q 45 /* obs code: G3Q        (GLO) */
#define CODE_L3X 46 /* obs code: G3I+Q      (GLO) */
#define CODE_L1I 47 /* obs code: B1I        (BDS) (obsolute) */
#define CODE_L1Q 48 /* obs code: B1Q        (BDS) (obsolute) */
#define CODE_L5A 49 /* obs code: L5A SPS    (IRN) */
#define CODE_L5B 50 /* obs code: L5B RS(D)  (IRN) */
#define CODE_L5C 51 /* obs code: L5C RS(P)  (IRN) */
#define CODE_L9A 52 /* obs code: SA SPS     (IRN) */
#define CODE_L9B 53 /* obs code: SB RS(D)   (IRN) */
#define CODE_L9C 54 /* obs code: SC RS(P)   (IRN) */
#define CODE_L9X 55 /* obs code: SB+C       (IRN) */
#define CODE_L1D 56 /* obs code: B1D        (BDS) */
#define CODE_L5D 57 /* obs code: L5D(L5S),B2aD (QZS,BDS) */
#define CODE_L5P 58 /* obs code: L5P(L5S),B2aP (QZS,BDS) */
#define CODE_L5Z 59 /* obs code: L5D+P(L5S) (QZS) */
#define CODE_L6E 60 /* obs code: L6E        (QZS) */
#define CODE_L7D 61 /* obs code: B2bD       (BDS) */
#define CODE_L7P 62 /* obs code: B2bP       (BDS) */
#define CODE_L7Z 63 /* obs code: B2bD+P     (BDS) */
#define CODE_L8D 64 /* obs code: B2abD      (BDS) */
#define CODE_L8P 65 /* obs code: B2abP      (BDS) */
#define CODE_L4A 66 /* obs code: G1aL1OCd   (GLO) */
#define CODE_L4B 67 /* obs code: G1aL1OCd   (GLO) */
#define CODE_L4X 68 /* obs code: G1al1OCd+p (GLO) */
#define MAXCODE 68  /* max number of obs code */

#define PMODE_SINGLE 0      /* positioning mode: single */
#define PMODE_PPP_STATIC 1  /* positioning mode: PPP-static */
#define PMODE_PPP_KINEMA 2  /* positioning mode: PPP-kinematic */
#define PMODE_PPP_DYNAMIC 3 /* positioning mode: PPP-dynamic */
#define PMODE_RTK_STATIC 4  /* positioning mode: RTK-kinematic */
#define PMODE_RTK_KINEMA 5  /* positioning mode: RTK-static */

#define SPPOPT_SF 0 /* spp mode: single frequency */
#define SPPOPT_IF 1 /* spp mode: dual frequency by if coupled */

#define SOLF_LLH 0  /* solution format: lat/lon/height */
#define SOLF_XYZ 1  /* solution format: x/y/z-ecef */
#define SOLF_ENU 2  /* solution format: e/n/u-baseline */
#define SOLF_NMEA 3 /* solution format: NMEA-183 */
#define SOLF_STAT 4 /* solution format: solution status */
#define SOLF_GSIF 5 /* solution format: GSI F1/F2 */

#define SOLQ_NONE 0   /* solution status: no solution */
#define SOLQ_FIX 1    /* solution status: fix */
#define SOLQ_FLOAT 2  /* solution status: float */
#define SOLQ_SBAS 3   /* solution status: SBAS */
#define SOLQ_DGPS 4   /* solution status: DGPS/DGNSS */
#define SOLQ_SINGLE 5 /* solution status: single */
#define SOLQ_PPP 6    /* solution status: PPP */
#define SOLQ_DR 7     /* solution status: dead reconing */
#define MAXSOLQ 7     /* max number of solution status */

#define TIMES_GPST 0 /* time system: gps time */
#define TIMES_UTC 1  /* time system: utc */
#define TIMES_JST 2  /* time system: jst */

#define P1P2DCB_OFF 0 /* receiver p1p2 dcb option: correction off */
#define P1P2DCB_ON 1  /* receiver p1p2 dcb option: correction on */

#define STR_NONE 0     /* stream type: none */
#define STR_SERIAL 1   /* stream type: serial */
#define STR_FILE 2     /* stream type: file */
#define STR_TCPSVR 3   /* stream type: TCP server */
#define STR_TCPCLI 4   /* stream type: TCP client */
#define STR_NTRIPSVR 5 /* stream type: NTRIP server */
#define STR_NTRIPCLI 6 /* stream type: NTRIP client */
#define STR_FTP 7      /* stream type: ftp */
#define STR_HTTP 8     /* stream type: http */
#define STR_NTRIPCAS 9 /* stream type: NTRIP caster */
#define STR_UDPSVR 10  /* stream type: UDP server */
#define STR_UDPCLI 11  /* stream type: UDP server */
#define STR_MEMBUF 12  /* stream type: memory buffer */

#define STRFMT_RTCM2 0   /* stream format: RTCM 2 */
#define STRFMT_RTCM3 1   /* stream format: RTCM 3 */
#define STRFMT_OEM4 2    /* stream format: NovAtel OEMV/4 */
#define STRFMT_CNAV 3    /* stream format: ComNav */
#define STRFMT_UBX 4     /* stream format: u-blox LEA-*T */
#define STRFMT_SBP 5     /* stream format: Swift Navigation SBP */
#define STRFMT_CRES 6    /* stream format: Hemisphere */
#define STRFMT_STQ 7     /* stream format: SkyTraq S1315F */
#define STRFMT_JAVAD 8   /* stream format: JAVAD GRIL/GREIS */
#define STRFMT_NVS 9     /* stream format: NVS NVC08C */
#define STRFMT_BINEX 10  /* stream format: BINEX */
#define STRFMT_RT17 11   /* stream format: Trimble RT17 */
#define STRFMT_SEPT 12   /* stream format: Septentrio */
#define STRFMT_TERSUS 13 /* stream format: TERSUS */
#define STRFMT_RINEX 14  /* stream format: RINEX */
#define STRFMT_SP3 15    /* stream format: SP3 */
#define STRFMT_RNXCLK 16 /* stream format: RINEX CLK */
#define STRFMT_SBAS 17   /* stream format: SBAS messages */
#define STRFMT_NMEA 18   /* stream format: NMEA 0183 */
#define MAXRCVFMT 13     /* max number of receiver format */

#define STR_MODE_R 0x1  /* stream mode: read */
#define STR_MODE_W 0x2  /* stream mode: write */
#define STR_MODE_RW 0x3 /* stream mode: read/write */

#define COMMENTH "%"                   /* comment line indicator for solution */
#define MSG_DISCONN "$_DISCONNECT\r\n" /* disconnect message */

#define LLI_SLIP 0x01   /* LLI: cycle-slip */
#define LLI_HALFC 0x02  /* LLI: half-cycle not resovled */
#define LLI_BOCTRK 0x04 /* LLI: boc tracking of mboc signal */
#define LLI_HALFA 0x40  /* LLI: half-cycle added */
#define LLI_HALFS 0x80  /* LLI: half-cycle subtracted */

#define INVALID_SSR 999
#define INVALID_UPD 999

#define RAW_NONE 0
#define RAW_FLOAT 1
#define EWL_FIXED 2
#define WL_FIXED 3
#define NL_FIXED 4
#define NL_HOLD 5

#define P2_5 0.03125                /* 2^-5 */
#define P2_6 0.015625               /* 2^-6 */
#define P2_11 4.882812500000000E-04 /* 2^-11 */
#define P2_15 3.051757812500000E-05 /* 2^-15 */
#define P2_17 7.629394531250000E-06 /* 2^-17 */
#define P2_19 1.907348632812500E-06 /* 2^-19 */
#define P2_20 9.536743164062500E-07 /* 2^-20 */
#define P2_21 4.768371582031250E-07 /* 2^-21 */
#define P2_23 1.192092895507810E-07 /* 2^-23 */
#define P2_24 5.960464477539063E-08 /* 2^-24 */
#define P2_27 7.450580596923828E-09 /* 2^-27 */
#define P2_29 1.862645149230957E-09 /* 2^-29 */
#define P2_30 9.313225746154785E-10 /* 2^-30 */
#define P2_31 4.656612873077393E-10 /* 2^-31 */
#define P2_32 2.328306436538696E-10 /* 2^-32 */
#define P2_33 1.164153218269348E-10 /* 2^-33 */
#define P2_35 2.910383045673370E-11 /* 2^-35 */
#define P2_38 3.637978807091710E-12 /* 2^-38 */
#define P2_39 1.818989403545856E-12 /* 2^-39 */
#define P2_40 9.094947017729280E-13 /* 2^-40 */
#define P2_43 1.136868377216160E-13 /* 2^-43 */
#define P2_48 3.552713678800501E-15 /* 2^-48 */
#define P2_50 8.881784197001252E-16 /* 2^-50 */
#define P2_55 2.775557561562891E-17 /* 2^-55 */

    typedef struct
    {                /* time struct */
        time_t time; /* time (s) expressed by standard time_t */
        double sec;  /* fraction of second under 1 s */
    } gtime_t;
    #include "ssr.h"
    typedef struct
    {                                 /* observation data record */
        gtime_t time;                 /* receiver sampling time (GPST) */
        uint8_t sat, rcv;             /* satellite/receiver number */
        uint16_t SNR[NFREQ + NEXOBS]; /* signal strength (0.001 dBHz) */
        uint8_t LLI[NFREQ + NEXOBS];  /* loss of lock indicator */
        uint8_t code[NFREQ + NEXOBS]; /* code indicator (CODE_???) */
        double L[NFREQ + NEXOBS];     /* observation data carrier-phase (cycle) */
        double P[NFREQ + NEXOBS];     /* observation data pseudorange (m) */
        float D[NFREQ + NEXOBS];      /* observation data doppler frequency (Hz) */

        int timevalid;                /* time is valid (Valid GNSS fix) for time mark */
        gtime_t eventime;             /* time of event (GPST) */
        uint8_t Lstd[NFREQ + NEXOBS]; /* stdev of carrier phase (0.004 cycles)  */
        uint8_t Pstd[NFREQ + NEXOBS]; /* stdev of pseudorange (0.01*2^(n+5) meters) */
        uint8_t freq;                 /* GLONASS frequency channel (0-13) */

    } obsd_t;

    typedef struct
    {                               /* observation data */
        int n, nmax;                /* number of obervation data/allocated */
        int flag;                   /* epoch flag (0:ok,1:power failure,>1:event flag) */
        int tstore[MAXSYS][NFREQ];  /* record whether obs channel has been determined */
        int usetype[MAXSYS][NFREQ]; /* record obs channel usage in the Systems Development Life Cycle*/
        obsd_t *data;               /* observation data records */
    } obs_t;

    typedef struct
    {
        int sat;
        int nrow;    /* nrow for pcv */
        int ncol;    /* ncol for pcv */
        double dazi; /* dazimuth */
        double dzen;
        double zen2;
        double zen1;
        double *var; /* if sat == 0 pcv in [MAXSYS][NFREQ][nrow][ncol] shape else in [NFREQ][nrow][ncol] */
    } pcvvar_t;

    typedef struct
    {
        int isel;                        /* whether isel */
        int sat;                         /* satellite number (0:receiver) */
        char type[MAXANT];               /* antenna type */
        char code[MAXANT];               /* serial number or satellite code */
        gtime_t ts, te;                  /* valid time start and end */
        double off[MAXSYS][NFREQ][3];    /* phase center offset e/n/u or x/y/z (m) */
        pcvvar_t pcvgrid; /* pcv grid */ /* pcv grid */
        // double var[MAXSYS][NFREQ][19]; /* phase center variation (m) */ /* el=90,85,...,0 or nadir=0,1,2,3,... (deg) */
    } rpcv_t;

    typedef struct
    {
        int isel;             /* whether isel */
        int sat;              /* satellite number (0:receiver) */
        char type[MAXANT];    /* antenna type */
        char code[MAXANT];    /* serial number or satellite code */
        gtime_t ts, te;       /* valid time start and end */
        double off[NFREQ][3]; /* phase center offset e/n/u or x/y/z (m) */
        pcvvar_t pcvgrid;     /* pcv grid */
        // double var[MAXSYS][NFREQ][19]; /* phase center variation (m) */             /* el=90,85,...,0 or nadir=0,1,2,3,... (deg) */
    } spcv_t;

    typedef struct
    {                  /* antenna parameters type */
        int ns, nmaxs; /* number of data/allocated */
        int nr, nmaxr; /* number of data/allocated */
        spcv_t *pcvs;  /* antenna parameters data */
        rpcv_t *pcvr;  /* antenna parameters data */
    } pcvs_t;

    /* -------------------------SELF DEFINED VARIABLE------------------------------- */
    typedef struct
    {
        int ns[MAXSAT];                    /* number of DCB types */
        int ts[MAXSAT];                    /* number of OSB type */
        double dcb_corr[MAXSAT][MAXTYPES]; /* satellite dcb corrections */
        char tys[MAXSAT][MAXTYPES][8];     /* type of dcb,such as C1C-C1W, for DCB */
        char ty[MAXSAT][MAXTYPE][4];       /* type of frequency,such as C1C, for OSB */
    } dcb_t;

    typedef struct
    {                    /* earth rotation parameter data type */
        double mjd;      /* mjd (days) */
        double xp, yp;   /* pole offset (rad) */
        double xpr, ypr; /* pole offset rate (rad/day) */
        double ut1_utc;  /* ut1-utc (s) */
        double lod;      /* length of day (s/day) */
    } erpd_t;

    typedef struct
    {                 /* earth rotation parameter type */
        int n, nmax;  /* number and max number of data */
        erpd_t *data; /* earth rotation parameter data */
    } erp_t;
    /* -------------------------SELF DEFINED VARIABLE------------------------------- */

    typedef struct
    { /* vmf1 grid data */
        double pgrid[5];
        double Tgrid[5];
        double Qgrid[5];
        double dTgrid[5];
        double ahgrid[5];
        double awgrid[5];
        double Tmgrid[5];
        double lagrid[5];
    } vmfgrid_t;

    typedef struct
    {                          /* GPS/QZS/GAL broadcast ephemeris type */
        int sat;               /* satellite number */
        int iode, iodc;        /* IODE,IODC */
        int sva;               /* SV accuracy (URA index) */
        int svh;               /* SV health (0:ok) */
        int week;              /* GPS/QZS: gps week, GAL: galileo week */
        int code;              /* GPS/QZS: code on L2 */
                               /* GAL: data source defined as rinex 3.03 */
                               /* BDS: data source (0:unknown,1:B1I,2:B1Q,3:B2I,4:B2Q,5:B3I,6:B3Q) */
        int flag;              /* GPS/QZS: L2 P data flag */
                               /* BDS: nav type (0:unknown,1:IGSO/MEO,2:GEO) */
        gtime_t toe, toc, ttr; /* Toe,Toc,T_trans, in GPST  */
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
        double dA, Adot, ndot,dn0; /* Adot,ndot for CNAV */
        int type;
    } eph_t;

    typedef struct
    {                      /* GLONASS broadcast ephemeris type */
        int sat;           /* satellite number */
        int iode;          /* IODE (0-6 bit of tb field) */
        int frq;           /* satellite frequency number */
        int svh, sva, age; /* satellite health, accuracy, age of operation */
        gtime_t toe;       /* epoch of epherides (gpst) */
        gtime_t tof;       /* message frame time (gpst) */
        double pos[3];     /* satellite position (ecef) (m) */
        double vel[3];     /* satellite velocity (ecef) (m/s) */
        double acc[3];     /* satellite acceleration (ecef) (m/s^2) */
        double taun, gamn; /* SV clock bias (s)/relative freq bias */
        double dtaun;      /* delay between L1 and L2 (s) */
    } geph_t;

    typedef struct
    {                          /* precise ephemeris type */
        gtime_t time;          /* time (GPST) */
        int index;             /* ephemeris index for multiple files */
        double pos[MAXSAT][4]; /* satellite position/clock (ecef) (m|s) */
        float std[MAXSAT][4];  /* satellite position/clock std (m|s) */
        double vel[MAXSAT][4]; /* satellite velocity/clk-rate (m/s|s/s) */
        float vst[MAXSAT][4];  /* satellite velocity/clk-rate std (m/s|s/s) */
        float cov[MAXSAT][3];  /* satellite position covariance (m^2) */
        float vco[MAXSAT][3];  /* satellite velocity covariance (m^2) */
    } peph_t;

    typedef struct
    {                          /* precise clock type */
        gtime_t time;          /* time (GPST) */
        int index;             /* clock index for multiple files */
        double clk[MAXSAT][1]; /* satellite clock (s) */
        float std[MAXSAT][1];  /* satellite clock std (s) */
    } pclk_t;
    typedef struct
    {                    /* SBAS ephemeris type */
        int sat;         /* satellite number */
        gtime_t t0;      /* reference epoch time (GPST) */
        gtime_t tof;     /* time of message frame (GPST) */
        int sva;         /* SV accuracy (URA index) */
        int svh;         /* SV health (0:ok) */
        double pos[3];   /* satellite position (m) (ecef) */
        double vel[3];   /* satellite velocity (m/s) (ecef) */
        double acc[3];   /* satellite acceleration (m/s^2) (ecef) */
        double af0, af1; /* satellite clock-offset/drift (s,s/s) */
    } seph_t;

    typedef struct                                              // by zzy
    {                                                           /* SSR correction type */
        gtime_t t0[7];                                          /* epoch time (GPST) {eph,clk,hrclk,ura,bias,pbias,ifpb} */
        double udi[7];                                          /* SSR update interval (s) */
        int iod[7];                                             /* iod ssr {eph,clk,hrclk,ura,bias,pbias,ifpb} */
        int iode;                                               /* issue of data */
        int iodcrc;                                             /* issue of data crc for beidou/sbas */
        int ura;                                                /* URA indicator */
        int refd;                                               /* sat ref datum (0:ITRF,1:regional) */
        double deph[3];                                         /* delta orbit {radial,along,cross} (m) */
        double ddeph[3];                                        /* dot delta orbit {radial,along,cross} (m/s) */
        double dclk[3];                                         /* delta clock {c0,c1,c2} (m,m/s,m/s^2) */
        double hrclk;                                           /* high-rate clock corection (m) */
        double ifpb;                                            /* ifpb for GPS satellites */
        double quaterniond[4]; /* Quaterniond for satellites */ // by zzy
        gtime_t ifpb_t0;                                        /* t0 for ifpb */
        float cbias[MAXCODE];                                   /* code biases (m) */
        double pbias[MAXCODE];                                  /* phase biases (m) */
        float stdpb[MAXCODE];                                   /* std-dev of phase biases (m) */
        double quateratt[4];                                    /* attitude quaterniond, w/x/y/z */
        double yaw_ang, yaw_rate;                               /* yaw angle and yaw rate (deg,deg/s) */
        uint8_t update;                                         /* update flag (0:no update,1:update) */
    } ssr_t;

    typedef struct
    {
        int m_iset;
        double m_sum, angle;
        time_t m_t0, m_tc;
    } ifpb_t;

    typedef struct
    {
        gtime_t toe;
        int sat;
        double quaterniond[4];
    } quater_t;

    typedef struct yaw_predict
    {
        gtime_t t0, t1;
        double yaw0, yaw1;
    } yaw_adapter;

    typedef struct
    {                               /* navigation data type */
        int n, nmax;                /* number of broadcast ephemeris */
        int ng, ngmax;              /* number of glonass ephemeris */
        int ns, nsmax;              /* number of sbas ephemeris */
        int ne, nemax;              /* number of precise ephemeris */
        int nc, ncmax;              /* number of precise clock */
        int na, namax;              /* number of precise quad products  */
        int np, nmaxp;              /* current count and maximum count of upd */
        int ni, nmaxi;              /* current count and maximum count of ifpb */
        eph_t *eph;                 /* GPS/QZS/GAL/BDS/IRN ephemeris */
        geph_t *geph;               /* GLONASS ephemeris */
        seph_t *seph;               /* SBAS ephemeris */
        peph_t *peph;               /* precise ephemeris */
        pclk_t *pclk;               /* precise clock */
        quater_t *att;              /* precise attitude products */
        ifpb_t *ifpb;               /* satellite ifpb */
        double utc_gps[8];          /* GPS delta-UTC parameters {A0,A1,Tot,WNt,dt_LS,WN_LSF,DN,dt_LSF} */
        double utc_glo[8];          /* GLONASS UTC time parameters {tau_C,tau_GPS} */
        double utc_gal[8];          /* Galileo UTC parameters */
        double utc_qzs[8];          /* QZS UTC parameters */
        double utc_cmp[8];          /* BeiDou UTC parameters */
        double utc_irn[9];          /* IRNSS UTC parameters {A0,A1,Tot,...,dt_LSF,A2} */
        double utc_sbs[4];          /* SBAS UTC parameters */
        double ion_gps[8];          /* GPS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
        double ion_gal[4];          /* Galileo iono model parameters {ai0,ai1,ai2,0} */
        double ion_qzs[8];          /* QZSS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
        double ion_cmp[8];          /* BeiDou iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
        double ion_irn[8];          /* IRNSS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
        int glo_fcn[32];            /* GLONASS FCN + 8 */
        double cbias[MAXSAT][3];    /* satellite DCB (0:P1-P2,1:P1-C1,2:P2-C2) (m) */
        double rbias[MAXRCV][2][3]; /* receiver DCB (0:P1-P2,1:P1-C1,2:P2-C2) (m) */
        spcv_t *pcvs;               /* satellite antenna pcv */
        ssr_t2 ssr[MAXSAT];          /* SSR corrections, for real-time  */
        /* -------------------------SELF DEFINED VARIABLE------------------------------- */
        dcb_t dcb; /* satellite dcb */
        erp_t erp;
        yaw_adapter yawAdapter[MAXSAT]; // by zzy
    } nav_t;

    typedef struct
    {                              /* station parameter type */
        char name[MAXANT];         /* marker name */
        char marker[MAXANT];       /* marker number */
        char antdes[MAXANT];       /* antenna descriptor */
        char antsno[MAXANT];       /* antenna serial number */
        char rectype[MAXANT];      /* receiver type descriptor */
        char recver[MAXANT];       /* receiver firmware version */
        char recsno[MAXANT];       /* receiver serial number */
        int antsetup;              /* antenna setup id */
        int itrf;                  /* ITRF realization year */
        int deltype;               /* antenna delta type (0:enu,1:xyz) */
        double pos[3];             /* station position (ecef) (m) */
        double del[3];             /* antenna position delta (e/n/u or x/y/z) (m) */
        double hgt;                /* antenna height (m) */
        int glo_cp_align;          /* GLONASS code-phase alignment (0:no,1:yes) */
        double glo_cp_bias[4];     /* GLONASS code-phase biases {1C,1P,2C,2P} (m) */
        Scalar gf[MAXSAT][NFREQ];  /* geometry-free phase (m) */
        Scalar mw[MAXSAT][NFREQ];  /* MW-LC (m) */
        gtime_t pt[MAXSAT][NFREQ]; /* each combination time */
    } sta_t;

    typedef struct
    {                       /* solution type */
        gtime_t time;       /* time (GPST) */
        gtime_t eventime;   /* time of event (GPST) */
        double rr[6];       /* position/velocity (m|m/s) */
                            /* {x,y,z,vx,vy,vz} or {e,n,u,ve,vn,vu} */
        float qr[6];        /* position variance/covariance (m^2) */
                            /* {c_xx,c_yy,c_zz,c_xy,c_yz,c_zx} or */
                            /* {c_ee,c_nn,c_uu,c_en,c_nu,c_ue} */
        float qv[6];        /* velocity variance/covariance (m^2/s^2) */
        double dtr[MAXSYS]; /* receiver clock bias to time systems (s),  only 6 degree_???*/
        uint8_t type;       /* type (0:xyz-ecef,1:enu-baseline) */
        uint8_t stat;       /* solution status (SOLQ_???) */
        uint8_t ns;         /* number of valid satellites */
        float age;          /* age of differential (s) */
        float ratio[3];     /* AR ratio factor for valiation */
        float prev_ratio1;  /* previous initial AR ratio factor for validation */
        float prev_ratio2;  /* previous final AR ratio factor for validation */
        float thres;        /* AR ratio threshold for valiation */
    } sol_t;

    typedef struct
    {
        gtime_t time;                         /* time of atmo (GPST) */
        char name[5];                         /* station name */
        double geod[3];                       /* station blh */
        double elev[MAXSAT], azim[MAXSAT];    /* satellite elevation and azimuth */
        float ionval[MAXSAT], ionsig[MAXSAT]; /* ionospher correction value and sigma */
        float trpval[MAXSAT], trpsig[MAXSAT]; /* troposphere correction value and sigma */
        struct list_head list;                /* list head */
    } atmo_t;

    typedef struct                              // adjust by jtao
    {                                           /* RTCM control struct type */
        int staid;                              /* station id */
        int stah;                               /* station health */
        int seqno;                              /* sequence number for rtcm 2 or iods msm */
        int outtype;                            /* output message type */
        gtime_t time;                           /* message time */
        gtime_t time_s;                         /* message start time */
        obs_t obs;                              /* observation data (uncorrected) */
        nav_t nav;                              /* satellite ephemerides */
        sta_t sta;                              /* station parameters */
        ssr_t2 ssr[MAXSAT];                      /* output of ssr corrections */
        ssr_t ssss;
        char msg[128];                          /* special message */
        char msgtype[256];                      /* last message type */
        char msmtype[7][128];                   /* msm signal types */
        int obsflag;                            /* obs data complete flag (1:ok,0:not complete) */
        int ephsat;                             /* input ephemeris satellite number */
        int ephset;                             /* input ephemeris set (0-1) */
        double cp[MAXSAT][NFREQ + NEXOBS];      /* carrier-phase measurement */
        uint16_t lock[MAXSAT][NFREQ + NEXOBS];  /* lock time */
        uint16_t loss[MAXSAT][NFREQ + NEXOBS];  /* loss of lock count */
        gtime_t lltime[MAXSAT][NFREQ + NEXOBS]; /* last lock time */
        int nbyte;                              /* number of bytes in message buffer */
        int nbit;                               /* number of bits in word buffer */
        int len;                                /* message length (bytes) */
        uint8_t buff[1200];                     /* message buffer */
        uint32_t word;                          /* word buffer for rtcm 2 */
        uint32_t nmsg2[100];                    /* message count of RTCM 2 (1-99:1-99,0:other) */
        uint32_t nmsg3[400];                    /* message count of RTCM 3 (1-299:1001-1299,300-329:4070-4099,0:ohter) */
        char opt[256];                          /* RTCM dependent options */
        /* newly added for SSR */
        int solid_ssr; /* solution ID for corrections, if different, should reset */
        atmo_t atmo;   /* atomsphere corrections */
    } rtcm_t;

    typedef struct
    {                        /* option type */
        const char *name;    /* option name */
        int format;          /* option format (0:int,1:double,2:string,3:enum) */
        void *var;           /* pointer to option variable */
        const char *comment; /* option comment/enum labels/unit */
    } opt_t;

    typedef struct
    {                          /* SNR mask type */
        int ena[2];            /* enable flag {rover,base} */
        double mask[NFREQ][9]; /* mask (dBHz) at 5,10,...85 deg */
    } snrmask_t;

    typedef struct
    {
        char tag[8];            /* ambiguity configure tag */
        int iq_comb[MAXSYS][2]; /* ambiguity combination */
        double min_lock;        /* minimum common time */
        double min_elev;        /* miniumu elevation */
        int btry_fix;           /* try to fix if upper layer fixed */
        uint8_t bresolve;       /* whether to resolve this ambiguity */
        uint8_t bupd_use;       /* whether use upd */
        struct list_head list;  /* list payload */
    } ambopt_t;

    typedef struct
    {
        int mode;                           /* positioning mode (PMODE_???) */
        int sppopt;                         /* spp mode */
        double elmin;                       /* elevation mask angle (rad) */
        double elmina;                      /* elevation mask for ambiguity */
        double minlock[3];                  /* minlock for EWL/WL/N1 */
        double thresslip;                   /* slip threshold of geometry-free phase (m) */
        snrmask_t snrmask;                  /* SNR mask */
        int sateph;                         /* satellite ephemeris/clock (EPHOPT_???) */
        int ionoopt;                        /* ionosphere option (IONOOPT_???) */
        double std[6];                      /* initial-state std [0]pos, [1]rclk [2]trop [3]l5dcb [4]p1p2dcb [5]ion */
        double prn[6];                      /* process-noise prn [0]pos, [1]rclk [2]trop [3]l5dcb [4]p1p2dcb [5]ion */
        double ru[3];                       /* rover position for fixed mode {x,y,z} (ecef) (m) */
        char anttype[MAXANT];               /* antenna types {rover} */
        double antdel[3];                   /* antenna delta {rov_e,rov_n,rov_u} */
        rpcv_t pcvr;                        /* receiver antenna parameters */
        char p1p2dcbopt[64];                /* receiver p1p2 dcb option (P1P2OPT_???) */
        char tropopt[64];                   /* troposphere command */
        char l5dcbopt[64];                  /* l5 dcb option*/
        int ifpbopt;                        /* ifpb option */
        char system[MAXSYS];                /* system usage */
        int nsystem;                        /* the number of system usage */
        int iref;                           /* index of reference constellation in SYS */
        int refpoint;                       /* reference point */
        int yawmodel;                       /* yaw model */
        char freqseq[256];                  /* frequency sequence */
        char freq[MAXSYS][NFREQ][LEN_FREQ]; /* frequency usage */
        int nfreq[MAXSYS];                  /* the number of frequency usage */
        char L5freq[MAXSYS][LEN_FREQ];      /* the L5 dcb parameter */
        double tint;                        /* interval */
        gtime_t ts;                         /* begin time */
        gtime_t te;                         /* end time */
        int nf;                             /* max frequency to process */
        double phnoise[MAXSYS];             /* phase noise (m) */
        double ranoise[MAXSYS];             /* range noise (m) */
        double odisp[6 * 11];               /* ocean tide loading parameters */
        int lpost;                          /* real-time and post flag */
        int ldia;                           /* sriffilter dia flag */
        int liar;                           /* sriffilter iar flag */
        ambopt_t ambopt_h;                  /* ambiguity configures */
    } prcopt_t;

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
    } solopt_t;

    typedef struct
    {                             /* file options type */
        char satantp[MAXSTRPATH]; /* satellite antenna parameters file */
        char rcvantp[MAXSTRPATH]; /* receiver antenna parameters file */
        char stapos[MAXSTRPATH];  /* station positions file */
        char geoid[MAXSTRPATH];   /* external geoid data file */
        char gptf[MAXSTRPATH];    /* GPT2 file */
        char iono[MAXSTRPATH];    /* ionosphere data file */
        char osb[MAXSTRPATH];     /* osb data file */
        char eop[MAXSTRPATH];     /* eop data file */
        char blq[MAXSTRPATH];     /* ocean tide loading blq file */
        char tempdir[MAXSTRPATH]; /* ftp/http temporaly directory */
        char geexe[MAXSTRPATH];   /* google earth exec file */
        char solstat[MAXSTRPATH]; /* solution statistics file */
        char trace[MAXSTRPATH];   /* debug trace file */
    } filopt_t;

    typedef struct
    {                          /* satellite status type */
        uint8_t sys;           /* navigation system */
        uint8_t vs;            /* valid satellite flag single */
        double azel[2];        /* azimuth/elevation angles {az,el} (rad) */
        uint8_t vsat[NFREQ];   /* ambiguity valid satellite flag */
        uint8_t fix[NFREQ];    /* ambiguity fix flag (1:float,2:fix,3:hold) (1:fix,2:float,3:hold), for all */
        uint8_t slip[NFREQ];   /* cycle-slip flag */
        uint8_t half[NFREQ];   /* half-cycle valid flag */
        int lock[NFREQ];       /* lock counter of phase */
        uint32_t outc[NFREQ];  /* obs outage counter of phase */
        uint32_t slipc[NFREQ]; /* cycle-slip counter */
        uint32_t rejc[NFREQ];  /* reject counter */
        double phw;            /* phase windup (cycle) */
        Scalar mapw;           /* ztd mapw */
        Scalar mapw_bs;        /* base ztd mapw */
        double e[3];           /* line sight vector */
        Scalar zhddel;         /* ztd dry part */
        Scalar zhddel_bs;      /* base ztd dry part */
        gtime_t ifpb_tsav;     /* save t0 for ifpb */
        int ifirst;            /* flag whether to reset prephi */
        double prephi;         /* previous for phase windup correction */
        uint8_t ifix;          /* ambiguity fix flag for EWL/WL/NL */
    } ssat_t;

    /******************* SELF DEFINED *********************/

    typedef struct
    {
        uint8_t *buffer;
        size_t blksize; // unit byte
        size_t blknum;  // unit byte
        uint16_t *ind_unused;
        uint16_t n_unused;
        uint8_t *ind_set;

    } BaseBuffer_t;

    typedef struct
    {
        int sat, ifreq, ind, isys, i_updv, iobs, isel; /* sat number, ifreq and the position of the parameter,sat started with 1, others started with 0 */
        double rw, map, span, esig;                    /* coefficient of this parameter */
        gtime_t ptime[2];                              /* start and end time of this parameter */
        char pname[32];                                /* parameters name */
        double xini, xest, xcor, selev;                /* initial value, estimated value, corrections */
        int ipob;                                      /* dia utils variables */
        void *psw;                                     /* dia utils variables */
        struct list_head list;                         /* list payload */
    } pamt_t;

    typedef struct
    {
        int isit, sat, ifreq;
        struct list_head list; /* list payload */
    } diafinder_t;

    typedef struct
    {
        int tags;
        double values;
        double dvalues;
        struct list_head list;
    } smoothelem_t;

    typedef struct
    {
        int ncur;
        int nmax;
        double mean;
        double xsig;
        smoothelem_t smelem;
    } smoothadapter_t;

    typedef struct
    {
        int nk;                         /* number of normal parameters :xyz,vxvyvz,axayaz recclk ztd dcb */
        int ni;                         /* number of ion parameters  */
        int npi;                        /* number of ion+normal parameters */
        int nc;                         /* number of ambiguities */
        int imtx;                       /* total number of all parameters */
        int nmtx;                       /* size of allocated memories */
        int nmtxY;                      /* size of allocated parameters plus observed observations */
        pamt_t *ipm[MAXSAT][NFREQ + 1]; /* index of ambiguities and ionosphere in the filter */
        pamt_t ionh;                    /* ionosphere parameters head */
        pamt_t ambh;                    /* ambiguities head */
        pamt_t *pm;                     /* normal parameters */
        Scalar *infs;                   /* informatio matrxi for ppp */
        Scalar *infs_u;                 /* time update matrix for ppp */
        int *iptx;                      /* pointer for column in infs */
        int nobs;                       /* number of observations */
        int (*idx)[NIDXC], *npm;
        Scalar *pkresi, *resi, esig, smsig;
        Scalar *v, *R, (*H)[MAXPAM][2]; /* omc, idx, R, H information for this epoch */
        char pname[16];                 /* parameters name,Debug  */
        diafinder_t diah;               /* ambiguities head */

        BaseBuffer_t *ptr_pamt_t;

    } srif_t;

    typedef struct
    {
        int sat, ifreq, isys, ind, map, iobs; /* sat, ifreq and the position of the parameter,sat started with 1, others started with 0 */
        double span, selev;                   /* coefficient of this parameter */
        double xini, xest, xcor;              /* initial value, estimated value, corrections */
        gtime_t ptime[2];                     /* start and end time of this parameter */
        char pname[32];                       /* parameters name */
        struct list_head list;                /* list payload */
    } kf_pamt_t;

    typedef struct
    {
        int isinit;                         /* filter is initialed or not */
        int nk;                             /* number of normal parameters :xyz,vxvyvz,axayaz recclk ztd dcb */
        int ni;                             /* number of ion parameters  */
        int npi;                            /* number of ion+normal parameters */
        int nc;                             /* number of ambiguities */
        int imtx;                           /* total number of all parameters */
        int nmtx;                           /* size of allocated memories */
        int nv;                             /* number of maximun observations */
        int nobs;                           /* number of observations */
        kf_pamt_t *ipob[MAXSAT][NFREQ + 1]; /* index of ambiguities and ionosphere in the filter */
        kf_pamt_t ionh;                     /* ionosphere parameters head */
        kf_pamt_t *pm;                      /* normal parameters */
        kf_pamt_t ambh;                     /* ambiguities head */
        int (*idx)[NIDXC];                  /* index of observations, {sat,iq,tp,idx2v}*/
        kScalar *pkresi, *resi, esig;
        kScalar *v, *R, *H; /* omc, idx, R, H information for this epoch */
        kScalar *F, *K;     /* F for x/v/a status tranfer in dynamic mode, K for gain matirx in measurement update */
        kScalar *Pf;
    } kf_t;

    typedef struct
    {                           /* RTK control/result type */
        sol_t sol;              /* RTK solution */
        double ru[6];           /* rove position/velocity (ecef) (m|m/s) */
        int nx, na;             /* number of float states/fixed states */
        double tt;              /* time difference between current and previous (s) */
        double *x, *P;          /* float states and their covariance */
        double *xa, *Pa;        /* fixed states and their covariance */
        int nfix;               /* number of continuous fixes of ambiguity */
        int excsat;             /* index of next satellite to be excluded for partial ambiguity resolution */
        int nb_ar;              /* number of ambiguities used for AR last epoch */
        int nsat[MAXSYS];       /* satellite numbers */
        int solstate;           /* current solution state */
        char holdamb;           /* set if fix-and-hold has occurred at least once */
        ssat_t ssat[MAXSAT];    /* satellite status */
        int neb;                /* bytes in error message buffer */
        char errbuf[MAXERRMSG]; /* error message buffer */
        prcopt_t opt;           /* processing options */
        int initial_mode;       /* initial positioning mode */
        srif_t srifs;           /* srif filter for processing */
    } rtk_t;

    // attitude description
    typedef struct
    {
        double w, x, y, z;
    } Quaternion;

    typedef void fatalfunc_t(const char *); /* fatal callback function type */
    /* global variables ----------------------------------------------------------*/
    extern char cprn_[MAXSAT][LEN_PRN];
    extern int nprn_;

    extern const double chisqr[]; /* chi-sqr(n) table (alpha=0.001) */

    extern const char cprn_default[MAXSAT][LEN_PRN];
    extern const int nprn_default;
    extern const prcopt_t prcopt_default;  /* default positioning options */
    extern const prcopt_t prcopt_default1; /* default positioning options */
    extern const solopt_t solopt_default;  /* default solution output options */
    extern const char *formatstrs[];       /* stream format strings */
    extern opt_t sysopts[];                /* system options table */

    /* satellites, systems, codes functions --------------------------------------*/
    EXPORT int satno(int sys, int prn);
    EXPORT int satsys(int sat, int *prn);
    EXPORT int satid2no(const char *id);
    EXPORT void satno2id(int sat, char *id);
    EXPORT uint8_t obs2code(const char *obs);
    EXPORT char *code2obs(uint8_t code);
    EXPORT double code2freq(int sys, uint8_t code, int fcn);
    EXPORT double sat2freq(int sat, uint8_t code, const nav_t *nav);
    EXPORT int code2idx(int sys, uint8_t code);
    EXPORT double freqbytp(int isys, const char *obs, int fcn);
    EXPORT int testsnr(int base, int freq, double el, double snr, const snrmask_t *mask);
    EXPORT void setcodepri(int sys, int idx, const char *pri);
    EXPORT int getcodepri(int sys, uint8_t code, const char *opt);

    /* matrix and vector functions -----------------------------------------------*/
    EXPORT double *mat(int n, int m);
    EXPORT float *matf(int n, int m);
    EXPORT int *imat(int n, int m);
    EXPORT double *zeros(int n, int m);
    EXPORT double *eye(int n);
    EXPORT float *eyef(int n);
    EXPORT double dot(const double *a, const double *b, int n);
    EXPORT double norm(const double *a, int n);
    EXPORT void cross3(const double *a, const double *b, double *c);
    EXPORT int normv3(const double *a, double *b);
    EXPORT void unit_vector(int n, double *v, double *u, double *length);
    EXPORT void matcpy(double *A, const double *B, int n, int m);
    EXPORT void matmul(const char *tr, int n, int k, int m, double alpha, const double *A, const double *B, double beta, double *C);
    EXPORT void matmpy(double *A, double *B, double *C, int row, int colA, int colB);
    EXPORT int matinv(double *A, int n);
    EXPORT int solve(const char *tr, const double *A, const double *Y, int n, int m, double *X);
    EXPORT int lsq(const double *A, const double *y, int n, int m, double *x, double *Q);
    EXPORT int filter(double *x, double *P, const double *H, const double *v, const double *R, int n, int m);
    EXPORT void matprintf_w(const Scalar *A, int tk, int n, int m, int p, int q);
    extern void matfprintf_w(const Scalar A[], int tk, int n, int m, int p, int q, FILE *fp);
    EXPORT int sum(int *array, int ndim);
    EXPORT void add_fatal(fatalfunc_t *func);
    /* time and string functions -------------------------------------------------*/
    EXPORT double str2num(const char *s, int i, int n);
    EXPORT int str2time(const char *s, int i, int n, gtime_t *t);
    EXPORT void time2str(gtime_t t, char *str, int n);
    EXPORT gtime_t epoch2time(const double *ep);
    EXPORT void time2epoch(gtime_t t, double *ep);
    EXPORT void time2epoch_n(gtime_t t, double *ep, int n);
    EXPORT gtime_t gpst2time(int week, double sec);
    EXPORT double time2gpst(gtime_t t, int *week);
    EXPORT int ymd2mjd(int iyear, int imonth, int iday);
    EXPORT double time2mjd(gtime_t t);
    EXPORT gtime_t gst2time(int week, double sec);
    EXPORT double time2gst(gtime_t t, int *week);
    EXPORT gtime_t bdt2time(int week, double sec);
    EXPORT double time2bdt(gtime_t t, int *week);
    EXPORT double mjd2wksow(int mjd, double sod, int *week, double *sow);
    EXPORT gtime_t mjd2time(int mjd, double sod);
    EXPORT char *time_str(gtime_t t, int n);
    EXPORT gtime_t timeadd(gtime_t t, double sec);
    EXPORT double timediff(gtime_t t1, gtime_t t2);
    EXPORT gtime_t gpst2utc(gtime_t t);
    EXPORT gtime_t utc2gpst(gtime_t t);
    EXPORT gtime_t gpst2bdt(gtime_t t);
    EXPORT gtime_t bdt2gpst(gtime_t t);
    EXPORT gtime_t timeget(void);
    EXPORT void timeset(gtime_t t);
    EXPORT void timereset(void);
    EXPORT double time2doy(gtime_t t);
    EXPORT double utc2gmst(gtime_t t, double ut1_utc);

    EXPORT int adjgpsweek(int week);
    EXPORT uint32_t tickget(void);
    EXPORT void sleepms(int ms);

    EXPORT int pointer_string(int row, int col, const char *string_array, const char *string);

    /* coordinates transformation ------------------------------------------------*/
    EXPORT void ecef2pos(const double *r, double *pos);
    EXPORT void pos2ecef(const double *pos, double *r);
    EXPORT void ecef2enu(const double *pos, const double *r, double *e);
    EXPORT void enu2ecef(const double *pos, const double *e, double *r);
    EXPORT void covenu(const double *pos, const double *P, double *Q);
    EXPORT void covecef(const double *pos, const double *Q, double *P);
    EXPORT void xyz2enu(const double *pos, double *E);
    EXPORT void rot_enu2xyz(double lat, double lon, double (*rotmat)[3]);
    EXPORT void eci2ecef(gtime_t tutc, const double *erpv, double *U, double *dU, double *gmst);
    EXPORT void deg2dms(double deg, double *dms, int ndec);
    EXPORT double dms2deg(const double *dms);

    /* debug trace functions -----------------------------------------------------*/
    EXPORT void traceopen(const char *file);
    EXPORT void traceclose(void);
    EXPORT void tracelevel(int level);
    EXPORT void trace(int level, const char *format, ...);
    EXPORT void tracet(int level, const char *format, ...);
    EXPORT void tracemat(int level, const double *A, int n, int m, int p, int q);
    EXPORT void traceobs(int level, const obsd_t *obs, int n);
    EXPORT void tracenav(int level, const nav_t *nav);
    EXPORT void tracegnav(int level, const nav_t *nav);
    EXPORT void tracehnav(int level, const nav_t *nav);
    EXPORT void tracepeph(int level, const nav_t *nav);
    EXPORT void tracepclk(int level, const nav_t *nav);
    EXPORT void traceb(int level, const uint8_t *p, int n);
    EXPORT int gettracelevel(void);

    /* positioning models --------------------------------------------------------*/
    EXPORT double satazel(const double *pos, const double *e, double *azel);
    EXPORT double geodist(const double *rs, const double *rr, double *e);
    EXPORT void dops(int ns, const double *azel, double elmin, double *dop);

    /* earth tide models ---------------------------------------------------------*/
    EXPORT void sunmoonpos_eci(gtime_t tut, double *rsun, double *rmoon);
    EXPORT void sunmoonpos(gtime_t tutc, const double *erpv, double *rsun, double *rmoon, double *gmst);

    EXPORT void s_cor_bias(gtime_t tm, rtk_t *ppp, nav_t *nav, obs_t *obt);
    EXPORT void s_bds_code_cor(int sat, int nfreq, char cfreq[NFREQ][LEN_FREQ], double elev, double *bias);

    /* ephemeris and clock functions ---------------------------------------------*/
    EXPORT double eph2clk(gtime_t time, const eph_t *eph);
    EXPORT double geph2clk(gtime_t time, const geph_t *geph);
    EXPORT double seph2clk(gtime_t time, const seph_t *seph);
    EXPORT void eph2pos(gtime_t time, const eph_t *eph, double *rs, double *dts, double *var);
    EXPORT void geph2pos(gtime_t time, const geph_t *geph, double *rs, double *dts, double *var);
    EXPORT void seph2pos(gtime_t time, const seph_t *seph, double *rs, double *dts, double *var);
    EXPORT int peph2pos(gtime_t time, int sat, const nav_t *nav, int opt, double *rs, double *dts, double *var);
    EXPORT int satpos_ssr(gtime_t time, gtime_t teph, int sat, const nav_t *nav,
                          int opt, double *rs, double *dts, double *var, int *svh);
    EXPORT void setseleph(int sys, int sel);
    EXPORT int getseleph(int sys);
    EXPORT void readsp3(const char *file, nav_t *nav, int opt);
    EXPORT int readdcb(const char *file, nav_t *nav, const sta_t *sta);
    EXPORT int ephpos(gtime_t time, gtime_t teph, int sat, const nav_t *nav, int iode, double *rs, double *dts, double *var, int *svh);

    /* rtcm functions ------------------------------------------------------------*/
    EXPORT int init_rtcm(rtcm_t *rtcm);
    EXPORT void free_rtcm(rtcm_t *rtcm);
    EXPORT int input_rtcm2(rtcm_t *rtcm, uint8_t data);
    EXPORT int input_rtcm3(rtcm_t *rtcm, uint8_t data);
    EXPORT int input_rtcm2f(rtcm_t *rtcm, FILE *fp);
    EXPORT int input_rtcm3f(rtcm_t *rtcm, FILE *fp);
    EXPORT int gen_rtcm2(rtcm_t *rtcm, int type, int sync);
    EXPORT int gen_rtcm3(rtcm_t *rtcm, int type, int subtype, int sync);

    EXPORT int decode_word(uint32_t word, uint8_t *data);
    EXPORT uint32_t getbitu(const uint8_t *buff, int pos, int len);
    EXPORT int32_t getbits(const uint8_t *buff, int pos, int len);
    EXPORT void setbitu(uint8_t *buff, int pos, int len, uint32_t data);
    EXPORT void setbits(uint8_t *buff, int pos, int len, int32_t data);
    EXPORT uint32_t crc24q(const uint8_t *buff, int len);
    EXPORT uint32_t crc32(const uint8_t *buff, int len);
    EXPORT uint16_t crc16(const uint8_t *buff, int len);

    /* lambda */
    EXPORT int lambda(int n, int m, const double *a, const double *Q, double *F,
                      double *s);
    EXPORT int lambda_search(int n, int m, const double *a, const double *Q,
                             double *F, double *s);
    EXPORT int lambda_reduction(int n, const double *Q, double *L, double *D, double *Z);
    EXPORT void s_pf_lookup_u(double *Q, int n, double esig, double pf0, int N, double *u, double *pf_ILS);

    /* options */
    EXPORT void resetsysopts(void);
    EXPORT void getsysopts(prcopt_t *popt, solopt_t *sopt, filopt_t *fopt);
    EXPORT void setsysopts(const prcopt_t *prcopt, const solopt_t *solopt, const filopt_t *filopt);
    EXPORT int loadopts(const char *file, opt_t *opts);

    /* tinyPPP functions */
    EXPORT void s_checkobs(obsd_t *obs, int n);
    EXPORT int s_validobs(const obsd_t *obs, int iq);
    EXPORT void solve_update(rtk_t *ppp);
    EXPORT void ppp_test_slip(rtk_t *ppp, sta_t *sta, obs_t *obt);
    EXPORT int udmodel_ppp(rtk_t *rtk, const sta_t *sta, const obsd_t *obs, const int n, nav_t *nav, Scalar *v, Scalar *R, int (*idx)[NIDXC]);
    EXPORT int execses_ppp(gtime_t ts, gtime_t te, double ti, prcopt_t *popt, const solopt_t *sopt, const filopt_t *fopt, char **infile, int n, char *outfile);
    EXPORT void ppp_pos(rtk_t *rtk, sta_t *sta, obs_t *obs, int n, nav_t *nav);
    EXPORT int udmodel_spp(rtk_t *rtk, const sta_t *sta, obsd_t *obs, const int n, nav_t *nav, int usegeod, double (*H)[MAXPAM][2], int *npm, double *v, double *R, int (*idx)[NIDXC]);
    EXPORT void ppp_resi_get(double *x, Scalar *v, int nobs, Scalar (*H)[20][2], int *npm, Scalar *resi);
    EXPORT void ppp_setPntPos(rtk_t *ppp, double *x);
    EXPORT void ppp_free(rtk_t *ppp);
    EXPORT void ppp_reset(rtk_t *ppp);
    /* iar functions */
    EXPORT ambopt_t *sel_ambopt(prcopt_t *opt, const char *tag);
    EXPORT void ppp_iar(rtk_t *ppp, nav_t *nav);

    /* matrix functions */
    EXPORT int inverse(double *A, int lda, int n);
    EXPORT int inverUU(Scalar *U, Scalar *inv, int lda, int n, char uctl);
    EXPORT void sHouseholder(Scalar *A, int lda, int itrans, int m, int n, int lsav);
    EXPORT void solvetri(int n, Scalar *A, int lda, Scalar *est, int lde);
    EXPORT int sHougenu(Scalar *A, int lda, int index, int row, int col, Scalar *u, Scalar *beta);
    EXPORT int s_ondia(rtk_t *ppp);
    EXPORT int ludcmp(double *A, int n, int *indx, double *d);
    EXPORT void lubksb(const double *A, int n, const int *indx, double *b);

    /* fatal error functions */
    EXPORT void ppp_memerror_fatal();
    EXPORT int index_sys(int sat);
    EXPORT int index_string(const char *src, char key);
    EXPORT int index_sys(int sat);
    EXPORT void mkfq2idx(prcopt_t *opt, const obsd_t *obs, int *fq2idx);

    EXPORT void readvmf1(const char *file);
    EXPORT double tropmf(gtime_t time, double cah_, double caw_, double *geod, double elev, double *maph);
    EXPORT void tropvmf1(gtime_t time, double *cah_, double *caw_, double *geod, double *zhd, double *zwd);
    EXPORT double tropmf_chi(gtime_t time, double cah_, double caw_, double *geod, double elev, double *maph);
    EXPORT void tropvmf1_chi(gtime_t time, double *cah_, double *caw_, double *geod, double *zhd, double *zwd);

    EXPORT Quaternion slerp(Quaternion starting, Quaternion ending, double t);
    EXPORT void updateSatAtts(const char *attitudeTypes, gtime_t t_now, int isat, const char *blk, double *xsat_f,
                              double *xscf, double *yscf, double *zscf);
    EXPORT void updateSatAtts_withYaw(const char *attitudeTypes, gtime_t t_now, int isat, const double yawAngle, double *xsat_j,
                                      double *xscf, double *yscf, double *zscf);
    EXPORT void itrs2gcrs_IERS2010(gtime_t tgps, double *erpv, double *U, double *dU,
                                   double (*dxmat)[3], double (*dymat)[3], int *mjdut1, double *sodut1,
                                   double *gmst, double *xp, double *yp);
    EXPORT void tide_displace_itrs(gtime_t tgps, double *xsit_f, double (*rot_f2j)[3],
                                   double (*rot_l2f)[3], double lat, double lon, double xpole, double ypole, double (*olc)[11], double *disp);
    EXPORT int m_checkrange(int bpccheck, rtk_t *rtk, const obsd_t *obs, const int nobs, int *omcflag, double *drecclk, int nb, Scalar *v, Scalar *R, int (*idx)[NIDXC]);

    /* average smooth */
    EXPORT void m_smooth_(smoothadapter_t *smadt, double value, double *omean, double *oxsig, double inxsig);
    EXPORT void m_smooth(smoothadapter_t *smadt, double value, double *omean, double *oxsig);
    EXPORT void m_smooth_reset(smoothadapter_t *smadt);
    EXPORT void m_smooth_setmax(smoothadapter_t *smadt, int in);
    EXPORT void m_smooth_remove(smoothadapter_t *smadt, int (*fun)(int));

    /* memory functions */
    EXPORT void *CALLOC(size_t nm, size_t sz);
    EXPORT void *MALLOC(size_t size);
    EXPORT void FREE(void *s);
    EXPORT BaseBuffer_t *BaseBuffer_Init(size_t unitsize, size_t totalnum);
    EXPORT void BaseBuffer_Delete(BaseBuffer_t *cbuf);
    EXPORT void *BaseBuffer_Malloc(BaseBuffer_t *cbuf);
    EXPORT void BaseBuffer_Free(BaseBuffer_t *cbuf, void *ptr);

#ifdef __cplusplus
}
#endif
#endif
