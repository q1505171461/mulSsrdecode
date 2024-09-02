//
// Created by jyh on 2024/7/17.
//
#include "interface.h"
#include "unistd.h"
#include "sys/stat.h"
#include <sys/time.h>



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



#define SYS_NONE 0x00                               /* navigation system: none */
#define SYS_GPS 0x01                                /* navigation system: GPS */
#define SYS_SBS 0x02                                /* navigation system: SBAS */
#define SYS_GLO 0x04                                /* navigation system: GLONASS */
#define SYS_GAL 0x08                                /* navigation system: Galileo */
#define SYS_QZS 0x10                                /* navigation system: QZSS */
#define SYS_CMP 0x20                                /* navigation system: BeiDou */
#define SYS_IRN 0x40                                /* navigation system: IRNS */
#define SYS_LEO 0x80                                /* navigation system: LEO */
#define SYS_ALL 0xFF                                /* navigation system: all */
#define KPL_SYS " G  R   E       J               C" /* system tag for processed */
#define PI 3.1415926535897932                       /* pi */
static kpl_iongrid_t s_atmo_interp = {0};
static kpl_iongrid_t s_atmo_grid = {0};
static kpl_iongrid_t s_atmo_grids[4] = {0};
static kpl_ionmodel_t s_ionmodel = {0};
// FILE *fp_ion;

static const double gpst0[] = {1980, 1, 6, 0, 0, 0}; /* gps time reference */
static double ep_set[6] = {0};
static double timeoffset_ = 0.0;
static const double ssrudint[16] = {1, 2, 5, 10, 15, 30, 60, 120, 240, 300, 600, 900, 1800, 3600, 7200, 10800};
static double leaps[64 + 1][7] = {/* leap seconds (y,m,d,h,m,s,utc-gpst) */
                                  {2017, 1, 1, 0, 0, 0, -18},
                                  {2015, 7, 1, 0, 0, 0, -17},
                                  {2012, 7, 1, 0, 0, 0, -16},
                                  {2009, 1, 1, 0, 0, 0, -15},
                                  {2006, 1, 1, 0, 0, 0, -14},
                                  {1999, 1, 1, 0, 0, 0, -13},
                                  {1997, 7, 1, 0, 0, 0, -12},
                                  {1996, 1, 1, 0, 0, 0, -11},
                                  {1994, 7, 1, 0, 0, 0, -10},
                                  {1993, 7, 1, 0, 0, 0, -9},
                                  {1992, 7, 1, 0, 0, 0, -8},
                                  {1991, 1, 1, 0, 0, 0, -7},
                                  {1990, 1, 1, 0, 0, 0, -6},
                                  {1988, 1, 1, 0, 0, 0, -5},
                                  {1985, 7, 1, 0, 0, 0, -4},
                                  {1983, 7, 1, 0, 0, 0, -3},
                                  {1982, 7, 1, 0, 0, 0, -2},
                                  {1981, 7, 1, 0, 0, 0, -1},
                                  {0}};

/* utils */
static void time2epoch(kpl_gtime_t t, double *ep)
{
    const int mday[] = {/* # of days in a month */
                        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
                        31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int days, sec, mon, day;

    /* leap year if year%4==0 in 1901-2099 */
    days = (int)(t.time / 86400);
    sec = (int)(t.time - (time_t)days * 86400);
    for (day = days % 1461, mon = 0; mon < 48; mon++)
    {
        if (day >= mday[mon])
            day -= mday[mon];
        else
            break;
    }
    ep[0] = 1970 + days / 1461 * 4 + mon / 12;
    ep[1] = mon % 12 + 1;
    ep[2] = day + 1;
    ep[3] = sec / 3600;
    ep[4] = sec % 3600 / 60;
    ep[5] = sec % 60 + t.sec;
}
static double time2mjd(kpl_gtime_t t)
{
    double ep[6];
    time2epoch(t, ep);
    return ymd2mjd((int)ep[0], (int)ep[1], (int)ep[2]) + (ep[3] * 3600.0 + ep[4] * 60.0 + ep[5]) / 86400.0;
}
static void time2mjdsod(kpl_gtime_t t, int *mjd, double *sod)
{
    double jd = time2mjd(t);
    *mjd = (int)jd;
    *sod = (jd - *mjd) * 86400.0;
}
static double timediff(kpl_gtime_t t1, kpl_gtime_t t2)
{
    return difftime(t1.time, t2.time) + t1.sec - t2.sec;
}
static kpl_gtime_t timeadd(kpl_gtime_t t, double sec)
{
    double tt;

    t.sec += sec;
    tt = floor(t.sec);
    t.time += (time_t)tt; // by zzy
    t.sec -= tt;
    return t;
}
static kpl_gtime_t epoch2time(const double *ep)
{
    const int doy[] = {1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
    kpl_gtime_t time = {0};
    int days, sec, year = (int)ep[0], mon = (int)ep[1], day = (int)ep[2];

    if (year < 1970 || 2099 < year || mon < 1 || 12 < mon)
        return time;

    /* leap year if year%4==0 in 1901-2099 */
    days = (year - 1970) * 365 + (year - 1969) / 4 + doy[mon - 1] + day - 2 + (year % 4 == 0 && mon >= 3 ? 1 : 0);
    sec = (int)floor(ep[5]);
    time.time = (time_t)days * 86400 + (int)ep[3] * 3600 + (int)ep[4] * 60 + sec;
    time.sec = ep[5] - sec;
    return time;
}
static void time2str(kpl_gtime_t t, char *s, int n)
{
    double ep[6];

    if (n < 0)
        n = 0;
    else if (n > 12)
        n = 12;
    if (1.0 - t.sec < 0.5 / pow(10.0, n))
    {
        t.time++;
        t.sec = 0.0;
    };
    time2epoch(t, ep);
    sprintf(s, "%04.0f/%02.0f/%02.0f %02.0f:%02.0f:%0*.*f", ep[0], ep[1], ep[2],
            ep[3], ep[4], n <= 0 ? 2 : n + 3, n <= 0 ? 0 : n, ep[5]);
}
static kpl_gtime_t gpst2utc(kpl_gtime_t t)
{
    kpl_gtime_t tu;
    int i;

    for (i = 0; leaps[i][0] > 0; i++)
    {
        tu = timeadd(t, leaps[i][6]);
        if (timediff(tu, epoch2time(leaps[i])) >= 0.0)
            return tu;
    }
    return t;
}
static kpl_gtime_t utc2gpst(kpl_gtime_t t)
{
    int i;

    for (i = 0; leaps[i][0] > 0; i++)
    {
        if (timediff(t, epoch2time(leaps[i])) >= 0.0)
            return timeadd(t, -leaps[i][6]);
    }
    return t;
}
static kpl_gtime_t gpst2time(int week, double sec)
{
    kpl_gtime_t t = epoch2time(gpst0);

    if (sec < -1E9 || 1E9 < sec)
        sec = 0.0;
    t.time += (time_t)86400 * 7 * week + (int)sec;
    t.sec = sec - (int)sec;
    return t;
}
static double time2gpst(kpl_gtime_t t, int *week)
{
    kpl_gtime_t t0 = epoch2time(gpst0);
    time_t sec = t.time - t0.time;
    int w = (int)(sec / (86400 * 7));

    if (week)
        *week = w;
    return (double)(sec - (double)w * 86400 * 7) + t.sec;
}
static kpl_gtime_t timeget(void)
{
    kpl_gtime_t time;
    double ep[6] = {2024, 3, 28, 0, 0, 0};
#ifdef WIN32
    SYSTEMTIME ts;

    GetSystemTime(&ts); /* utc */
    ep[0] = ts.wYear;
    ep[1] = ts.wMonth;
    ep[2] = ts.wDay;
    ep[3] = ts.wHour;
    ep[4] = ts.wMinute;
    ep[5] = ts.wSecond + ts.wMilliseconds * 1E-3;
#else
    struct timeval tv;
    struct tm *tt;

    if (!gettimeofday(&tv, NULL) && (tt = gmtime(&tv.tv_sec)))
    {
        ep[0] = tt->tm_year + 1900;
        ep[1] = tt->tm_mon + 1;
        ep[2] = tt->tm_mday;
        ep[3] = tt->tm_hour;
        ep[4] = tt->tm_min;
        ep[5] = tt->tm_sec + tv.tv_usec * 1E-6;
    }
#endif
    if (ep_set[0] * ep_set[1] * ep_set[2] != 0.0)
        time = epoch2time(ep_set);
    else
        time = epoch2time(ep);
#ifdef CPUTIME_IN_GPST /* cputime operated in gpst */
    time = gpst2utc(time);
#endif
    return timeadd(time, timeoffset_);
}
static void adjweek(kpl_atmo_rtcm_t *rtcm, double tow)
{
    double tow_p;
    int week;

    /* if no time, get cpu time */
    if (rtcm->time.time == 0)
        rtcm->time = utc2gpst(timeget());
    tow_p = time2gpst(rtcm->time, &week);
    if (tow < tow_p - 302400.0)
        tow += 604800.0;
    else if (tow > tow_p + 302400.0)
        tow -= 604800.0;
    rtcm->time = gpst2time(week, tow);
}
static void adjday_glot(kpl_atmo_rtcm_t *rtcm, double tod)
{
    kpl_gtime_t time;
    double tow, tod_p;
    int week;

    if (rtcm->time.time == 0)
        rtcm->time = utc2gpst(timeget());
    time = timeadd(gpst2utc(rtcm->time), 10800.0); /* glonass time */
    tow = time2gpst(time, &week);
    tod_p = fmod(tow, 86400.0);
    tow -= tod_p;
    if (tod < tod_p - 43200.0)
        tod += 86400.0;
    else if (tod > tod_p + 43200.0)
        tod -= 86400.0;
    time = gpst2time(week, tow + tod);
    rtcm->time = utc2gpst(timeadd(time, -10800.0));
}
static double dot(const double *a, const double *b, int n)
{
    double c = 0.0;

    while (--n >= 0)
        c += a[n] * b[n];
    return c;
}
static double norm(const double *a, int n)
{
    return sqrt(dot(a, a, n));
}
static int compare_geod(double geod_tp[3], double geod_gd[3])
{
    for (int i = 0; i < 3; i++)
    {
        if (geod_tp[i] != geod_gd[i])
        {
            return 0;
        }
    }
    return 1;
}
static uint32_t getbitu(const uint8_t *buff, int pos, int len)
{
    uint32_t bits = 0;
    int i;
    for (i = pos; i < pos + len; i++)
        bits = (bits << 1) + ((buff[i / 8] >> (7 - i % 8)) & 1u);
    return bits;
}
static int32_t getbits(const uint8_t *buff, int pos, int len)
{
    uint32_t bits = getbitu(buff, pos, len);
    if (len <= 0 || 32 <= len || !(bits & (1u << (len - 1))))
        return (int32_t)bits;
    return (int32_t)(bits | (~0u << len)); /* extend sign */
}

/* functions */
static int decode_ssr_epoch_model(kpl_atmo_rtcm_t *rtcm, int sys, int subtype)
{
    double tod, tow;
    int i = 24 + 12;

    if (subtype == 0)
    { /* RTCM SSR */

        if (sys == SYS_GLO)
        {
            tod = getbitu(rtcm->buff, i, 17);
            i += 17;
            adjday_glot(rtcm, tod);
        }
        else
        {
            tow = getbitu(rtcm->buff, i, 20);
            i += 20;
            adjweek(rtcm, tow);
        }
    }
    else
    {
        /* IGS SSR */
        i += 3 + 8;
        tow = getbitu(rtcm->buff, i, 20);
        i += 20;
        adjweek(rtcm, tow);
    }
    return i;
}

static int decode_ssr_epoch_grid(kpl_atmo_rtcm_t *rtcm, int sys, int subtype)
{
    double tod, tow;
    int i = 24 + 12;

    if (subtype == 0)
    { /* RTCM SSR */

        if (sys == SYS_GLO)
        {
            tod = getbitu(rtcm->buff, i, 17);
            i += 17;
            adjday_glot(rtcm, tod);
        }
        else
        {
            tow = getbitu(rtcm->buff, i, 20);
            i += 20;
            adjweek(rtcm, tow);
        }
    }
    else
    {
        /* IGS SSR */
        i += 3 + 8 + 13;
        tow = getbitu(rtcm->buff, i, 20);
        i += 20;
        adjweek(rtcm, tow);
    }
    return i;
}

static int decode_ssr9_head_grid(kpl_atmo_rtcm_t *rtcm, int sys, int subtype, int *sync, int *iod, double *udint, int *hsize)
{
    double geod[3] = {0};
    char *msg, tstr[64];
    int i = 24 + 12, nsat, udi, provid = 0, solid = 0, ns, dtype;
    if (subtype == 0)
    { /* RTCM SSR */
        return -1;
    }
    i = decode_ssr_epoch_grid(rtcm, sys, subtype); // obtain time

    dtype = getbitu(rtcm->buff, i, 3); // Data Type Identification
    i += 3;
    *iod = getbitu(rtcm->buff, i, 11); // VTEC SSR IOD
    i += 11;
    udi = getbitu(rtcm->buff, i, 4); // SSR Update Interval
    *udint = ssrudint[udi];
    i += 4;
    *sync = getbitu(rtcm->buff, i, 1); // SSR Multiple Message Number
    i += 1;
    solid = getbitu(rtcm->buff, i, 3); /* solution ID */
    // s_atmo_grid.id = solid;
    i += 3;
    geod[0] = getbits(rtcm->buff, i, 31) * 1e-6; /* Reference latitude, 38 bits */
    i += 38;
    geod[1] = getbits(rtcm->buff, i, 31) * 1e-6; /* Reference longtitude, 39 bits */
    i += 39;
    geod[2] = getbits(rtcm->buff, i, 23) * 1e-2; /* Reference Height, 23 bits */
    i += 23;

    int juge_pos = 0;
    if (timediff(s_atmo_grid.time, rtcm->time))
    {
        initialize_iongrid(&s_atmo_grid);
        s_atmo_grid.time = rtcm->time;
        s_atmo_grid.id = solid;
        memcpy(s_atmo_grid.geod, geod, 3 * sizeof(double));
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            if (compare_geod(geod, s_atmo_grids[i].geod) || norm(s_atmo_grids[i].geod, 3) == 0.0)
            {
                if (!timediff(s_atmo_grids[i].time, rtcm->time))
                {
                    s_atmo_grid = s_atmo_grids[i];
                    s_atmo_grid.id = solid;
                    juge_pos = 1;
                    break;
                }
                else
                {
                    initialize_iongrid(&s_atmo_grid);
                    s_atmo_grid.time = rtcm->time;
                    s_atmo_grid.id = solid;
                    memcpy(s_atmo_grid.geod, geod, 3 * sizeof(double));
                    juge_pos = 1;
                    break;
                }
            }
        }
        if (juge_pos == 0)
        {
            printf("The fifth station appeared");
            return;
        }
    }

    nsat = getbitu(rtcm->buff, i, 6);
    i += 6;

    time2str(rtcm->time, tstr, 2);
    printf("decode_ssr2_head: time=%s sys=%d subtype=%d nsat=%d sync=%d iod=%d"
           "solid=%d geod0=%f geod1=%f geod2=%f\n",
           tstr, sys, subtype, nsat, *sync, *iod, solid, geod[0], geod[1], geod[2]);

    if (rtcm->outtype)
    {
        msg = rtcm->msgtype + strlen(rtcm->msgtype);
        sprintf(msg, " %s nsat=%2d iod=%2d udi=%2d sync=%d", tstr, nsat, *iod, udi,
                *sync);
    }
    *hsize = i;
    return nsat;
}
static int decode_ssr9_head_model(kpl_atmo_rtcm_t *rtcm, int sys, int subtype, int *sync, int *iod, double *udint, int *hsize)
{
    char *msg, tstr[64];
    int i = 24 + 12, nsat, udi, provid = 0, solid = 0, ns;
    double clat, clon;

    if (subtype == 0)
    { /* RTCM SSR */
        return -1;
    }
    i = decode_ssr_epoch_model(rtcm, sys, subtype); // obtain time (i=24+12+3+8+20)
    s_ionmodel.t0 = rtcm->time;
    udi = getbitu(rtcm->buff, i, 4); // SSR Update Interval. bit(4)
    *udint = ssrudint[udi];
    i += 4;
    *sync = getbitu(rtcm->buff, i, 1); // SSR Multiple Message Number. bit(1)
    i += 1;
    *iod = getbitu(rtcm->buff, i, 11); // STEC SSR IOD. 0: NaN, 1-1440: Time span (60s)
    i += 11;
    nsat = getbitu(rtcm->buff, i, 6); // satellite number. uint4
    i += 6;
    // model property parameter
    s_ionmodel.lat0 = getbits(rtcm->buff, i, 25) * 1e-5; // central lat. int25
    i += 25;
    s_ionmodel.lon0 = getbits(rtcm->buff, i, 26) * 1e-5; // cetral lon. int26
    i += 26;
    s_ionmodel.n = getbitu(rtcm->buff, i, 2); // degree of lat. uint2
    i += 2;
    s_ionmodel.m = getbitu(rtcm->buff, i, 2); // degree of lon. uint2
    i += 2;

    time2str(rtcm->time, tstr, 2);
    printf("decode_ssr2_head: time=%s sys=%d subtype=%d nsat=%d sync=%d iod=%d"
           "solid=%d\n",
           tstr, sys, subtype, nsat, *sync, *iod, solid);
    //    printf(4, "decode_ssr2_head: time=%s sys=%d subtype=%d nsat=%d sync=%d iod=%d"
    //              "solid=%d\n", tstr, sys, subtype, nsat, *sync, *iod, solid);

    if (rtcm->outtype)
    {
        msg = rtcm->msgtype + strlen(rtcm->msgtype);
        sprintf(msg, " %s nsat=%2d iod=%2d udi=%2d sync=%d", tstr, nsat, *iod, udi, *sync);
    }
    *hsize = i;
    return nsat;
}
static double interpb(const double *y, double a, double b)
{
    return y[0] * (1.0 - a) * (1.0 - b) + y[1] * a * (1.0 - b) + y[2] * (1.0 - a) * b + y[3] * a * b;
}
static void s_interp_atmo(const double *pos)
{
    double dlat, dlon, a, b;
    double ionsig[4], ionval[4];
    for (int i = 0; i < 4; i++)
    {
        if (s_atmo_grids[i].id == -1)
        {
            printf("(s_interp_atmo) warning: atmo_grid %d %d %d %d\n",
                   s_atmo_grids[0].id, s_atmo_grids[1].id, s_atmo_grids[2].id, s_atmo_grids[3].id);
            return;
        }
    }
    // 0 1 2 3 = lb rb lt rt
    dlat = s_atmo_grids[3].geod[0] - s_atmo_grids[0].geod[0];
    dlon = s_atmo_grids[3].geod[1] - s_atmo_grids[0].geod[1];
    a = (pos[0] - s_atmo_grids[0].geod[0]) / dlat;
    b = (pos[1] - s_atmo_grids[0].geod[1]) / dlon;
    for (int isat = 0; isat < KPL_MAXSAT; isat++)
    {
        int valid = 1;
        for (int i = 0; i < 4; i++)
        {
            ionval[i] = s_atmo_grids[i].ionval[isat];
            ionsig[i] = s_atmo_grids[i].ionsig[isat];
            if (ionsig[i] == 0.0)
                valid = 0;
        }
        s_atmo_interp.ionval[isat] = valid ? (float)interpb(ionval, a, b) : 999.0f;
        s_atmo_interp.ionsig[isat] = valid ? (float)interpb(ionsig, a, b) : 999.0f;
    }
}
void initialize_iongrid(kpl_iongrid_t *grid)
{
    grid->time.time = 0;
    grid->time.sec = 0;
    grid->id = 0;
    memset(grid->geod, 0, sizeof(grid->geod));
    memset(grid->ionval, 0, sizeof(grid->ionval));
    memset(grid->trpval, 0, sizeof(grid->trpval));
    memset(grid->ionsig, 0, sizeof(grid->ionsig));
    memset(grid->trpsig, 0, sizeof(grid->trpsig));
    grid->nsat = 0;
    grid->dtype = 0;
    grid->udint = 0;
    grid->iod = 0;
    grid->sync = 0;
    memset(grid->cprn, 0, sizeof(grid->cprn));
    memset(grid->elev, 0, sizeof(grid->elev));
}
static void s_grid_assign()
{
    if (s_atmo_grid.nsat == 0) // whether received or not
    {
        // printf("Grid not be recei/ved\n");
        return;
    }
    if (timediff(s_atmo_grid.time, s_atmo_grids[0].time) && s_atmo_grids[0].time.time != 0) // time
    {
        for (int i = 0; i < 4; i++)
        {
            initialize_iongrid(&s_atmo_grids[i]);
        }
    }
    for (int i = 0; i < 4; i++)
    {
        if (compare_geod(s_atmo_grid.geod, s_atmo_grids[i].geod) || norm(s_atmo_grids[i].geod, 3) == 0.0) // grid point
        {
            s_atmo_grids[i] = s_atmo_grid;
            s_atmo_grids[i].id = i;
            break;
        }
    }
}
/* API */
extern int KPL_decode_ssr9_grid(kpl_atmo_rtcm_t *rtcm, int sys, int subtype)
{
    double stec, sigma;
    double udint, elev, lat = 0.0, lon = 0.0, hig = 0.0;
    int i, j, type, sync, iod, sat, ura, np, offp, prn, nsat;

    type = getbitu(rtcm->buff, 24, 12);

    if ((nsat = decode_ssr9_head_grid(rtcm, sys, subtype, &sync, &iod, &udint, &i)) < 0)
    {
        printf("rtcm3 %d length error: len=%d\n", type, rtcm->len);
        return -1;
    }

    s_atmo_grid.nsat += nsat;
    s_atmo_grid.udint = udint;
    s_atmo_grid.iod = iod;
    s_atmo_grid.sync = sync;

    for (j = 0; j < nsat && i + 24 <= rtcm->len * 8; j++)
    {
        prn = getbitu(rtcm->buff, i, 6) + offp;
        i += 6;
        char cprn[KPL_LEN_PRN] = {0};
        sprintf(cprn, "%c%02d", KPL_SYS[sys], prn);
        if (!(sat = kpl_satno(sys, prn)))
        {
            printf("rtcm3 %d satellite number error: prn=%d\n", type, prn);
            continue;
        }
        memcpy(s_atmo_grid.cprn[sat], cprn, sizeof(char) * KPL_LEN_PRN);

        s_atmo_grid.elev[sat] = getbitu(rtcm->buff, i, 10) * 0.1; /* GNSS Satellite Elevation Angle, uint10, 0.1 degree */
        i += 10;
        s_atmo_grid.ionval[sat] = getbits(rtcm->buff, i, 14) * 5e-2f; /* STEC, int14, unit:0.05 TECU */
        i += 14;
        s_atmo_grid.ionsig[sat] = getbitu(rtcm->buff, i, 8) * 5e-2f; /* Sigma, uint8, unit:0.05 TECU */
        i += 8;
        i += 10;
    }
    s_grid_assign(); // save new griddata to grids
    return sync ? 0 : 9;
}
extern int KPL_decode_ssr9_model(kpl_atmo_rtcm_t *rtcm, int sys, int subtype)
{
    int i, j, type, sync, iod, sat, ura, np, offp, prn, nsat;
    double udint, elev, lat = 0.0, lon = 0.0, hig = 0.0;
    int tc, ic, fc; // int/frac part of coef
    double stec, sigma;

    type = getbitu(rtcm->buff, 24, 12);

    if ((nsat = decode_ssr9_head_model(rtcm, sys, subtype, &sync, &iod, &udint, &i)) < 0)
    {
        printf("rtcm3 %d length error: len=%d\n", type, rtcm->len);
        // trace(2, "rtcm3 %d length error: len=%d\n", type, rtcm->len);
        return -1;
    }
    s_ionmodel.nsat = nsat;
    s_ionmodel.udint = udint;
    s_ionmodel.iod = iod;
    s_ionmodel.sync = sync;
    // model coffecient
    for (j = 0; j < nsat && i + 24 <= rtcm->len * 8; ++j)
    {
        prn = getbitu(rtcm->buff, i, 6); // number part of prn. uint6
        i += 6;
        char cprn[KPL_LEN_PRN] = {0};
        sprintf(cprn, "%c%02d", KPL_SYS[sys], prn);
        memcpy(s_ionmodel.cprn[j], cprn, sizeof(char) * KPL_LEN_PRN);
        for (int ii = 0; ii < s_ionmodel.n + 1; ++ii)
        {
            for (int jj = 0; jj < s_ionmodel.m + 1; ++jj)
            {
                tc = getbitu(rtcm->buff, i, 1); // uint1
                i += 1;
                ic = getbitu(rtcm->buff, i, 10); // uint10
                i += 10;
                fc = getbitu(rtcm->buff, i, 17); // uint17
                i += 17;
                s_ionmodel.coef[j][ii * (s_ionmodel.n + 1) + jj] = tc > 0 ? ic + fc * 1e-5 : -(ic + fc * 1e-5);
                // s_ionmodel.coef[j][ii * (s_ionmodel.n + 1) + jj] = ic >= 0 ? ic + fc * 1e-5 : ic - fc * 1e-5;
            }
        }
        s_ionmodel.sig[j] = getbitu(rtcm->buff, i, 9) * 1e-2f; // uint9
        i += 9;
    }
    return sync ? 0 : 9;
}

extern float KPL_getIonGrid(int isat)
{
    float stec;
    if (s_atmo_interp.ionsig[isat] != 0)
        stec = s_atmo_interp.ionval[isat];
    else
        stec = 999.0f;
    return stec;
}
extern float KPL_getIonModel(int isat, double ipplat, double ipplon, kpl_gtime_t t)
{
    double stec = 0.0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            double dlat = ipplat - s_ionmodel.lat0;
            double dlon = ipplon - s_ionmodel.lon0;
            double dt = timediff(t, s_ionmodel.t0) / 3600.0 * 15.0; // hour angle of sun
            stec += s_ionmodel.coef[isat][i * 3 + j] * pow(dlat, i) * pow(dlon + dt, j);
        }
    }
    return (float)(stec == 0 ? 999.0f : stec);
}