#include <stdint.h>
#include <time.h>

#define MAXCODE 68 /* max number of obs code */
#define MAXSSRSAT 280
#define SSRCTX_BUFFLEN 256
#define MAXCOEFFNUM 10
#define MAXSIG 180
#define MAXNETWORK 32 /* Compact Network ID */
#define MAXGRID 50
#define QZSSSIGNUM 16

typedef struct
{                /* time struct */
    time_t time; /* time (s) expressed by standard time_t */
    double sec;  /* fraction of second under 1 s */
} gtime_t2;

typedef struct
{                    /* SSR correction type */
    gtime_t2 t0[6];   /* epoch time (GPST) {eph,clk,hrclk,ura,cbias,pbias} */
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
    int f_pbias[MAXCODE];
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
} ssr_t2;

typedef struct
{ /* SSR grid correction type */
    int coeff_num;
    double sigma;
    double rect[4];

    gtime_t2 t0;
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
    ssr_t2 ssr_epoch[MAXSSRSAT];
} ssr_network_t;

typedef struct
{
    unsigned char buff[SSRCTX_BUFFLEN]; /* binary data */
    int nbyte;                          /* number of bits in word buffer */
    gtime_t2 BDT;                        // 北斗时天内秒
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

    ssr_t2 ssr_epoch[MAXSSRSAT];
    ssr_network_t *ssr_network[MAXNETWORK];
} ssrctx_t;

extern void print_ssr(ssrctx_t *sc);
extern int input_b2bssr(ssrctx_t *, unsigned char);
extern int input_kplssr(ssrctx_t *, unsigned char);
extern int input_qzssr(ssrctx_t *, unsigned char);
extern int input_galssr(ssrctx_t *, unsigned char);
extern int index_string(const char *src, char key);