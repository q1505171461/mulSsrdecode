#include <stdint.h>
#include <time.h>

#define KPLSSR_STRUCT_SIZE 63

#define NUM_SYSTEMS 4
#define NUM_MODES 16
#define B2bPREAMB 0xD3
#define QZSSPREAMB 0x1ACFFC1D
#define MAXSSRSAT 256
#define MAXB2BAREA 10
#define MAXLEAPS 60
#define B2B_SYS "CGRE"
#define SSRCTX_BUFFLEN 256
#define MAXAGE_ORBIT 96
#define MAXAGE_CLK 100
#define MAXAGE_PBIAS 100
#define MAXAGE_CBIAS 86400
#define MAXCOEFFNUM 10
#define MAXGNSSYS 6
#define MAXSIG 180
#define MAXNETWORK 32 /* Compact Network ID */
#define MAXGRID 50
#define QZSSSIGNUM 16
#define QZSS_UPDATE_INTERVALS {1, 2, 5, 10, 15, 30, 60, 120, 240, 300, 600, 900, 1800, 3600, 7200, 10800};

#define OBSTYPE_PR 0x01  /* observation type: pseudorange */
#define OBSTYPE_CP 0x02  /* observation type: carrier-phase */
#define OBSTYPE_DOP 0x04 /* observation type: doppler-freq */
#define OBSTYPE_SNR 0x08 /* observation type: SNR */
#define OBSTYPE_ALL 0xFF /* observation type: all */

#define FREQTYPE_L1 0x01  /* frequency type: L1/E1 */
#define FREQTYPE_L2 0x02  /* frequency type: L2/B1 */
#define FREQTYPE_L5 0x04  /* frequency type: L5/E5a/L3 */
#define FREQTYPE_L6 0x08  /* frequency type: E6/LEX/B3 */
#define FREQTYPE_L7 0x10  /* frequency type: E5b/B2 */
#define FREQTYPE_L8 0x20  /* frequency type: E5(a+b) */
#define FREQTYPE_ALL 0xFF /* frequency type: all */

#define CODE_NONE 0 /* obs code: none or unknown */
#define CODE_L1C 1  /* obs code: L1C/A,G1C/A,E1C (GPS,GLO,GAL,QZS,SBS) */
#define CODE_L1P 2  /* obs code: L1P,G1P    (GPS,GLO) */
#define CODE_L1W 3  /* obs code: L1 Z-track (GPS) */
#define CODE_L1Y 4  /* obs code: L1Y        (GPS) */
#define CODE_L1M 5  /* obs code: L1M        (GPS) */
#define CODE_L1N 6  /* obs code: L1codeless (GPS) */
#define CODE_L1S 7  /* obs code: L1C(D)     (GPS,QZS) */
#define CODE_L1L 8  /* obs code: L1C(P)     (GPS,QZS) */
#define CODE_L1E 9  /* obs code: L1-SAIF    (QZS) */
#define CODE_L1A 10 /* obs code: E1A        (GAL) */
#define CODE_L1B 11 /* obs code: E1B        (GAL) */
#define CODE_L1X 12 /* obs code: E1B+C,L1C(D+P) (GAL,QZS) */ 1 2 3 7 8 0 16 17 18 19 20 24 25 26 0 0
#define CODE_L1Z 13 /* obs code: E1A+B+C,L1SAIF (GAL,QZS) */ 1 2 14 19 66 67 68 0 0 0 44 45 46 0 0 0
#define CODE_L2C 14 /* obs code: L2C/A,G2C/A (GPS,GLO) */ 11 1 12 24 25 26 27 28 29 37 38 39 0 0 0 0
#define CODE_L2D 15 /* obs code: L2 L1C/A-(P2-P1) (GPS) */ 40 41 18 42 43 33 27 28 29 0 0 0 0 0 0 0
#define CODE_L2S 16 /* obs code: L2C(M)     (GPS,QZS) */ 1 7 8 12 16 17 18 24 25 26 0 0 0 0 0 0
#define CODE_L2L 17 /* obs code: L2C(L)     (GPS,QZS) */ 1 24 25 26
#define CODE_L2X 18 /* obs code: L2C(M+L),B1I+Q (GPS,QZS,CMP) */
#define CODE_L2P 19 /* obs code: L2P,G2P    (GPS,GLO) */
#define CODE_L2W 20 /* obs code: L2 Z-track (GPS) */
#define CODE_L2Y 21 /* obs code: L2Y        (GPS) */
#define CODE_L2M 22 /* obs code: L2M        (GPS) */
#define CODE_L2N 23 /* obs code: L2codeless (GPS) */
#define CODE_L5I 24 /* obs code: L5/E5aI    (GPS,GAL,QZS,SBS) */
#define CODE_L5Q 25 /* obs code: L5/E5aQ    (GPS,GAL,QZS,SBS) */
#define CODE_L5X 26 /* obs code: L5/E5aI+Q  (GPS,GAL,QZS,SBS) */
#define CODE_L7I 27 /* obs code: E5bI,B2I   (GAL,CMP) */
#define CODE_L7Q 28 /* obs code: E5bQ,B2Q   (GAL,CMP) */
#define CODE_L7X 29 /* obs code: E5bI+Q,B2I+Q (GAL,CMP) */
#define CODE_L6A 30 /* obs code: E6A        (GAL) */
#define CODE_L6B 31 /* obs code: E6B        (GAL) */
#define CODE_L6C 32 /* obs code: E6C        (GAL) */
#define CODE_L6X 33 /* obs code: E6B+C,LEXS+L,B3I+Q (GAL,QZS,CMP) */
#define CODE_L6Z 34 /* obs code: E6A+B+C    (GAL) */
#define CODE_L6S 35 /* obs code: LEXS       (QZS) */
#define CODE_L6L 36 /* obs code: LEXL       (QZS) */
#define CODE_L8I 37 /* obs code: E5(a+b)I   (GAL) */
#define CODE_L8Q 38 /* obs code: E5(a+b)Q   (GAL) */
#define CODE_L8X 39 /* obs code: E5(a+b)I+Q (GAL) */
#define CODE_L2I 40 /* obs code: B1I        (CMP) */
#define CODE_L2Q 41 /* obs code: B1Q        (CMP) */
#define CODE_L6I 42 /* obs code: B3I        (CMP) */
#define CODE_L6Q 43 /* obs code: B3Q        (CMP) */
#define CODE_L3I 44 /* obs code: G3I        (GLO) */
#define CODE_L3Q 45 /* obs code: G3Q        (GLO) */
#define CODE_L3X 46 /* obs code: G3I+Q      (GLO) */
#define CODE_L1I 47 /* obs code: B1I        (BDS) */
#define CODE_L1Q 48 /* obs code: B1Q        (BDS) */
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
#define CODE_L4B 67 /* obs code: G1aL1OCp   (GLO) */
#define CODE_L4X 68 /* obs code: G1al1OCd+p (GLO) */
#define MAXCODE 68  /* max number of obs code */

static double leaps[MAXLEAPS + 1][7];

typedef struct
{                /* time struct */
    time_t time; /* time (s) expressed by standard time_t */
    double sec;  /* fraction of second under 1 s */
} gtime_t;

typedef struct
{                    /* SSR correction type */
    gtime_t t0[5];   /* epoch time (GPST) {eph,clk,hrclk,ura,bias} */
    double udi[5];   /* SSR update interval (s) */
    int iod[5];      /* iod ssr {eph,clk,hrclk,ura,bias} */
    int iode;        /* issue of data */
    int iodcrc;      /* issue of data crc for beidou/sbas */
    double ura;      /* URA indicator */
    int refd;        /* sat ref datum (0:ITRF,1:regional) */
    double deph[3];  /* delta orbit {radial,along,cross} (m) */
    double ddeph[3]; /* dot delta orbit {radial,along,cross} (m/s) */
    double dclk[3];  /* delta clock {c0,c1,c2} (m,m/s,m/s^2) */
    double hrclk;    /* high-rate clock corection (m) */
    double yaw;
    int8_t IODCorr;
    double yawrate;
    int f_cbias[MAXCODE];
    float cbias[MAXCODE]; /* code biases (m) */
    int i_pbias[MAXCODE];
    double pbias[MAXCODE]; /* phase biases (m) */
    unsigned char update;  /* update flag (0:no update,1:update) */

    double stura;       /* SSR STEC Quality Indicator */
    int sctype;         /* STEC Correction Type 0~3 */
    double c00;         /* STEC Polynomial Coefficients C00 */
    double c01;         /* STEC Polynomial Coefficients C01 */
    double c10;         /* STEC Polynomial Coefficients C10 */
    double c11;         /* STEC Polynomial Coefficients C11 */
    double c02;         /* STEC Polynomial Coefficients C02 */
    double c20;         /* STEC Polynomial Coefficients C20 */
    double sr[MAXGRID]; /* STEC Residual Grid*/
} ssr_t;

typedef struct
{ /* SSR grid correction type */
    int coeff_num;
    double sigma;
    double rect[4];

    gtime_t t0;
    int networkcorr; /* Type11 Network Correction */
    int tavail;
    int cnid;
    double ura;         /* URA indicator */
    int tctype;         /* Tropospheric Correction Type 0~2 */
    double t00;         /* Troposphere Polynomial Coefficients T00*/
    double t01;         /* Troposphere Polynomial Coefficients T01*/
    double t10;         /* Troposphere Polynomial Coefficients T10*/
    double t11;         /* Troposphere Polynomial Coefficients T11*/
    double tro;         /* Troposphere Residual Offset */
    int ngrid;          /* Tropospheric Residual Length */
    double tr[MAXGRID]; /* Tropospheric Residual */

    int savail;
    int mask_st12[MAXSSRSAT]; /* 掩码 */
    int mask_st11[MAXSSRSAT]; /* 掩码 */
    int mask_st6[MAXSSRSAT];  /* 掩码 */
    ssr_t ssr_epoch[MAXSSRSAT];
} ssr_network_t;

typedef struct
{
    unsigned char buff[SSRCTX_BUFFLEN]; /* binary data */
    int nbyte;                          /* number of bits in word buffer */
    gtime_t BDT;                        // 北斗时天内秒
    uint8_t IODSSR;                     // SSR 版本号 2bit
    uint8_t IODP;                       // 掩码版本号 4bit
    uint16_t IODN;                      // 基本导航电文版本号 10bit
    uint8_t IODCorr_f;                  // 改正数版本号 3bit

    int lend;               /* whether exist followed messages */
    int lvalid;             /* 当前是否mask消息有效 */
    int n_sat;              /* 当前改正数播发的卫星数 */
    int mask[MAXSSRSAT];    /* 掩码 */
    int maskb2b[MAXSSRSAT]; /* 掩码b2b */
    int mask_sig[MAXSIG];   /* signal mask */

    // QZSS
    int prn;
    int gnss_cell_mask[MAXSSRSAT * 16];
    int cellmask_n;
    unsigned char qbuff[1500];
    int qbuff_beg;
    int qbuff_end;
    int sub_n;
    int subframe_indicator; /* QZSS Subframe indicator */
    int alert_flag;
    int message_number;      /* Message Number */
    int message_sub_type_id; /* Message Sub Type ID */
    int gps_epoch_t;         /* GPS Epoch Time 1s */
    int gnss_hourly_epoch_t; /* GNSS Hourly Epoch Time */
    int ssr_update_interval; /* SSR Update Interval */
    int mul_msg_indicator;   /* mul_msg_indicator */
    int n_gnss;              /* No. of GNSS */
    int last_iodssr;
    int next_need_len;
    int qzss_wait_new_subframe;

    ssr_t ssr_epoch[MAXSSRSAT];
    ssr_network_t *ssr_network[MAXNETWORK];
} ssrctx_t;

uint64_t get_bits(unsigned char *encoded_data, int i_beg, int len);
ssr_network_t *get_ssr_network(ssrctx_t *sc, int cnid);
void get_bits_array(uint8_t *encoded_data, int i_beg, int len, uint8_t *output_array);
int64_t uint64_to_int64(uint64_t value, uint8_t originalLen);
static uint32_t crc24_pppB2b(uint8_t *data, uint16_t length);

const static double gpst0[] = {1980, 1, 6, 0, 0, 0}; /* gps time reference */
extern gtime_t timeadd(gtime_t t, double sec);
extern gtime_t epoch2time(const double *ep);
extern void time2epoch(gtime_t t, double *ep);
extern gtime_t utc2gpst(gtime_t t);
extern gtime_t timeget(void);
extern uint8_t obs2code(const char *obs);
gtime_t bdt2gpst(gtime_t t);
extern double timediff(gtime_t t1, gtime_t t2);
extern double time2gpst(gtime_t t, int *week);
extern gtime_t gpst2time(int week, double sec);
void prn2str(int prn, char *prnstr);
void print_ssr(ssrctx_t *sc);
double ura2dist(uint8_t ura);

extern int input_b2bssr(ssrctx_t *, unsigned char);
extern int input_kplssr(ssrctx_t *, unsigned char);
extern int input_qzssr(ssrctx_t *, unsigned char);
extern int input_galssr(ssrctx_t *, unsigned char);
extern int index_string(const char *src, char key);