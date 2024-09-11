#include "ssr_.h"
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t get_bit(unsigned char *encoded_data, int i)
{
    return (*((encoded_data) + i / 8) & (1 << (7 - i % 8))) == 0 ? 0 : 1;
}

uint64_t get_bits(unsigned char *encoded_data, int i_beg, int len)
{
    uint64_t rvl = 0;
    for (int i = 0; i < len; i++)
    {
        rvl |= (get_bit(encoded_data, i + i_beg) << (len - i - 1));
    }
    return rvl;
}

ssr_network_t *get_ssr_network(ssrctx_t *sc, int cnid)
{
    if (!sc->ssr_network[cnid])
    {
        sc->ssr_network[cnid] = calloc(1, sizeof(ssr_network_t));
    }
    sc->ssr_network[cnid]->cnid = cnid;
    return sc->ssr_network[cnid];
}
/******************************************************************************
 * Name:    crc24_pppB2b  x24+x23+x18+x17+x14+x11+x10+x7+x6+x5+x4+x3+x1+1
 * Poly:    0x864CFB
 * Init:    0x000000
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000000
 * Note:
 *****************************************************************************/
static uint32_t crc24_pppB2b(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint32_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= (uint8_t)(*data++); // crc ^=(uint8_t)(*data); data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 0x80000000)
                crc = (crc << 1) ^ 0x864CFB00;
            else
                crc <<= 1;
        }
    }
    return crc;
}

void get_bits_array(uint8_t *encoded_data, int i_beg, int len, uint8_t *output_array)
{
    for (int i = 0; i < len; i++)
    {
        output_array[i] = get_bit(encoded_data, i + i_beg);
    }
}

int64_t uint64_to_int64(uint64_t value, uint8_t originalLen)
{
    uint64_t a = 1;
    if ((a <<= --originalLen) & value)
        return (0xffffffffffffffff << originalLen) | value; // 负数
    else
        return value;
}

// 闰秒的列表，包含每次闰秒的日期和总的闰秒数（从 GPS 纪元开始）
typedef struct
{
    int year, month, day;
    int leap_seconds;
} leap_seconds_t;

static double leaps[MAXLEAPS + 1][7] = {/* leap seconds (y,m,d,h,m,s,utc-gpst) */
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

/* add time --------------------------------------------------------------------
 * add time to gtime_t2 struct
 * args   : gtime_t2 t        I   gtime_t2 struct
 *          double sec       I   time to add (s)
 * return : gtime_t2 struct (t+sec)
 *-----------------------------------------------------------------------------*/
extern gtime_t2 timeadd(gtime_t2 t, double sec)
{
    double tt;

    t.sec += sec;
    tt = floor(t.sec);
    t.time += (int)tt;
    t.sec -= tt;
    return t;
}

/* time difference -------------------------------------------------------------
 * difference between gtime_t2 structs
 * args   : gtime_t2 t1,t2    I   gtime_t2 structs
 * return : time difference (t1-t2) (s)
 *-----------------------------------------------------------------------------*/
extern double timediff(gtime_t2 t1, gtime_t2 t2)
{
    return difftime(t1.time, t2.time) + t1.sec - t2.sec;
}

/* utc to gpstime --------------------------------------------------------------
 * convert utc to gpstime considering leap seconds
 * args   : gtime_t2 t        I   time expressed in utc
 * return : time expressed in gpstime
 * notes  : ignore slight time offset under 100 ns
 *-----------------------------------------------------------------------------*/
extern gtime_t2 utc2gpst(gtime_t2 t)
{
    int i;

    for (i = 0; leaps[i][0] > 0; i++)
    {
        if (timediff(t, epoch2time(leaps[i])) >= 0.0)
            return timeadd(t, -leaps[i][6]);
    }
    return t;
}
struct timeval
{
    __time_t tv_sec;       /* Seconds.  */
    __suseconds_t tv_usec; /* Microseconds.  */
};
/* convert calendar day/time to time -------------------------------------------
 * convert calendar day/time to gtime_t2 struct
 * args   : double *ep       I   day/time {year,month,day,hour,min,sec}
 * return : gtime_t2 struct
 * notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
 *-----------------------------------------------------------------------------*/

extern gtime_t2 epoch2time(const double *ep)
{
    const int doy[] = {1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
    gtime_t2 time = {0};
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
static double timeoffset_ = 0.0;
extern gtime_t2 timeget(void)
{
    double ep[6] = {0};
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
    return timeadd(epoch2time(ep), timeoffset_);
}

/* time to calendar day/time ---------------------------------------------------
 * convert gtime_t2 struct to calendar day/time
 * args   : gtime_t2 t        I   gtime_t2 struct
 *          double *ep       O   day/time {year,month,day,hour,min,sec}
 * return : none
 * notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
 *-----------------------------------------------------------------------------*/
extern void time2epoch(gtime_t2 t, double *ep)
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

/* gps time to time ------------------------------------------------------------
 * convert week and tow in gps time to gtime_t2 struct
 * args   : int    week      I   week number in gps time
 *          double sec       I   time of week in gps time (s)
 * return : gtime_t2 struct
 *-----------------------------------------------------------------------------*/
extern gtime_t2 gpst2time(int week, double sec)
{
    gtime_t2 t = epoch2time(gpst0);

    if (sec < -1E9 || 1E9 < sec)
        sec = 0.0;
    t.time += 86400 * 7 * week + (int)sec;
    t.sec = sec - (int)sec;
    return t;
}
/* time to gps time ------------------------------------------------------------
 * convert gtime_t2 struct to week and tow in gps time
 * args   : gtime_t2 t        I   gtime_t2 struct
 *          int    *week     IO  week number in gps time (NULL: no output)
 * return : time of week in gps time (s)
 *-----------------------------------------------------------------------------*/
extern double time2gpst(gtime_t2 t, int *week)
{
    gtime_t2 t0 = epoch2time(gpst0);
    time_t sec = t.time - t0.time;
    int w = (int)(sec / (86400 * 7));

    if (week)
        *week = w;
    return (double)(sec - w * 86400 * 7) + t.sec;
}
/* time to string --------------------------------------------------------------
 * convert gtime_t2 struct to string
 * args   : gtime_t2 t        I   gtime_t2 struct
 *          char   *s        O   string ("yyyy/mm/dd hh:mm:ss.ssss")
 *          int    n         I   number of decimals
 * return : none
 *-----------------------------------------------------------------------------*/
extern void time2str(gtime_t2 t, char *s, int n)
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
/* bdt to gpstime --------------------------------------------------------------
 * convert bdt (beidou navigation satellite system time) to gpstime
 * args   : gtime_t2 t        I   time expressed in bdt
 * return : time expressed in gpstime
 * notes  : see gpst2bdt()
 *-----------------------------------------------------------------------------*/
gtime_t2 bdt2gpst(gtime_t2 t)
{
    return timeadd(t, 14.0);
}
void prn2str(int prn, char *prnstr)
{
    int bds_offset = 0;
    switch (prn / 40)
    {
    case 0:
        prnstr[0] = 'G';
        break;
    case 1:
        prnstr[0] = 'R';
        break;
    case 2:
        prnstr[0] = 'E';
        break;
    case 3:
        prnstr[0] = 'C';
        break;
    case 4:
        prnstr[0] = 'J';
        break;
    case 5:
        prnstr[0] = 'S';
        break;
    case 6:
        prnstr[0] = 'C';
        bds_offset = 40;
        break;
    default:
        prnstr[0] = ' ';
        break;
    }
    sprintf(prnstr + 1, "%02d", prn % 40 + 1 + bds_offset);
}
static char *obscodes[] = {
    /* observation code strings */

    "", "1C", "1P", "1W", "1Y", "1M", "1N", "1S", "1L", "1E",   /*  0- 9 */
    "1A", "1B", "1X", "1Z", "2C", "2D", "2S", "2L", "2X", "2P", /* 10-19 */
    "2W", "2Y", "2M", "2N", "5I", "5Q", "5X", "7I", "7Q", "7X", /* 20-29 */
    "6A", "6B", "6C", "6X", "6Z", "6S", "6L", "8L", "8Q", "8X", /* 30-39 */
    "2I", "2Q", "6I", "6Q", "3I", "3Q", "3X", "1I", "1Q", "5A", /* 40-49 */
    "5B", "5C", "9A", "9B", "9C", "9X", "1D", "5D", "5P", "5Z", /* 50-59 */
    "6E", "7D", "7P", "7Z", "8D", "8P", "4A", "4B", "4X", ""    /* 60-69 */
};

uint8_t obs2code3(const char *obs)
{
    for (int i = 1; *obscodes[i]; i++)
    {
        if (strcmp(obscodes[i], obs))
            continue;
        return (uint8_t)i;
    }
    return CODE_NONE;
}
static char *code2obs(uint8_t code)
{
    if (code <= CODE_NONE || MAXCODE < code)
        return "";
    return obscodes[code];
}

#define URA_INVALID -1.0
double ura2dist(uint8_t ura)
{
    /* Converts user range accuracy (URA) code to accuracy in distance [mm] */
    double dist = 0.0;

    if ((ura & 0x3F) == 0x00)
    { // ura.b == '000000': undefined or unknown
        dist = URA_INVALID;
    }
    else if ((ura & 0x3F) == 0x3F)
    { // ura.b == '111111': URA more than 5466.5 mm
        dist = 5466.5;
    }
    else
    {
        uint8_t cls = (ura >> 3) & 0x07; // ura[4:7].u
        uint8_t val = ura & 0x07;        // ura[0:4].u
        dist = pow(3, cls) * (1 + val / 4.0) - 1;
    }
    return dist;
}

/**
 * print ssr;
 */
void print_ssr(ssrctx_t *sc)
{
    char prnstr[5] = "";
    int line = 0;

    int satnum = 0;
    for (int i = 0; i < MAXSSRSAT; ++i)
    {
        if (!sc->mask[i])
            continue;
        if (!sc->ssr_epoch[i].t0[1].time)
            continue;
        satnum++;
    }

    for (int i = 0; i < MAXSSRSAT; ++i)
    {
        if (!sc->mask[i])
            continue;
        if (!sc->ssr_epoch[i].t0[1].time)
            continue; // 不含clk

        if (!line)
        {
            char s[100] = "";
            time2str(sc->ssr_epoch[i].t0[1], s, 0);
            printf("%s   PRN: %d   IODSSR: %2d   NUM:%3d\n", s, sc->prn, sc->IODSSR, satnum); // head
        }
        /* prn */
        prn2str(i, prnstr);
        printf("%s", prnstr);

        /* orb */
        if (sc->ssr_epoch[i].t0[0].time)
        {
            printf(" orb:%9.4f%9.4f%9.4f  iode: %3d", sc->ssr_epoch[i].deph[0], sc->ssr_epoch[i].deph[1], sc->ssr_epoch[i].deph[2], sc->ssr_epoch[i].iode);
        }

        /* clk */
        if (sc->ssr_epoch[i].t0[1].time)
            printf(" clk:%9.4f", sc->ssr_epoch[i].dclk[0]);

        /* ura */
        if (sc->ssr_epoch[i].t0[3].time)
        printf(" ura:%8.2f", sc->ssr_epoch[i].ura);

        /* pbias */
        if (sc->ssr_epoch[i].t0[5].time)
        {
            printf(" pbias:");
            for (int j = 0; j < MAXCODE; ++j)
            {
                if (sc->ssr_epoch[i].f_pbias[j])
                    printf("%s %9.4f ", code2obs(j + 1), sc->ssr_epoch[i].pbias[j]);
            }
        }

        /* cbias */
        if (sc->ssr_epoch[i].t0[4].time)
        {
            printf(" cbias:");
            for (int j = 0; j < MAXCODE; ++j)
            {
                if (sc->ssr_epoch[i].f_cbias[j])
                    printf("%s %9.4f ", code2obs(j + 1), sc->ssr_epoch[i].cbias[j]);
            }
        }
        printf("\n");
        line++;
    }
}

// kpl
extern int index_string(const char *src, char key)
{
    int len = strlen(src);
    int i;
    for (i = 0; i < len; i++)
    {
        if (src[i] == key)
            break;
    }
    if (i == len)
        return -1;
    else
        return i;
}