// by zzy
#include "tinyppp.h"
#include "IO_rtcm.h"
static FILE *feph;
static FILE *fobs;
static FILE *fssr;
static FILE *fclk;
static FILE *fsp3;
static FILE *fobx;
static int ditv = 5;
char outdir[1024] = "sav";
char *ephhead[] = {
    "     3.05           N: GNSS NAV DATA    M: Mixed            RINEX VERSION / TYPE\n",
    "TINYPPP              KPL             %4d%02d%02d 000000 UTC PGM / RUN BY / DATE\n",
    "Source: RTCM_3.3 58.49.58.149/RTCM3EPH-MGEX                 COMMENT\n",
    "                                                            END OF HEADER\n"};
char *obshead[] =
    {
        "     3.05           OBSERVATION DATA    M                   RINEX VERSION / TYPE\n",
        "TINYPPP              KPL             %4d%02d%02d 000000 UTC    PGM / RUN BY / DATE\n",
        "Default set of observation types used                       COMMENT\n",
        "RTCM_3 %28s                         COMMENT\n",
        "%10s                                                  MARKER NAME\n",
        "                                                            MARKER TYPE\n",
        "                                                            OBSERVER / AGENCY\n",
        "                                                            REC # / TYPE / VERS\n",
        "                                                            ANT # / TYPE\n",
        "%14.4f%14.4f%14.4f                  APPROX POSITION XYZ\n",
        "        0.0000        0.0000        0.0000                  ANTENNA: DELTA H/E\n",
        "  %4d    %2d    %2d    %2d    %2d   %10.7f     GPS         TIME OF FIRST OBS\n",
        "C   12 C2I L2I S2I C6I L6I S6I C5X L5X S5X C7I L7I S7I      SYS / # / OBS TYPES\n",
        "E   18 C1X L1X S1X C1C L1C S1C C5X L5X S5X C5Q L5Q S5Q C7X  SYS / # / OBS TYPES\n",
        "       L7X S7X C7Q L7Q S7Q                                  SYS / # / OBS TYPES\n",
        "G   12 C1C L1C S1C C2W L2W S2W C5X L5X S5X C5Q L5Q S5Q      SYS / # / OBS TYPES\n",
        "                                                            END OF HEADER\n"};
char *obxhead[] = {"+SATELLITE/ID_AND_DESCRIPTION\n",
                   " G01 BLOCK IIF\n",
                   " G02 BLOCK IIR-B\n",
                   " G03 BLOCK IIF\n",
                   " G04 BLOCK IIIA\n",
                   " G05 BLOCK IIR-M\n",
                   " G06 BLOCK IIF\n",
                   " G07 BLOCK IIR-M\n",
                   " G08 BLOCK IIF\n",
                   " G09 BLOCK IIF\n",
                   " G10 BLOCK IIF\n",
                   " G11 BLOCK IIIA\n",
                   " G12 BLOCK IIR-M\n",
                   " G13 BLOCK IIR-A\n",
                   " G14 BLOCK IIIA\n",
                   " G15 BLOCK IIR-M\n",
                   " G16 BLOCK IIR-A\n",
                   " G17 BLOCK IIR-M\n",
                   " G18 BLOCK IIIA\n",
                   " G19 BLOCK IIR-B\n",
                   " G20 BLOCK IIR-A\n",
                   " G21 BLOCK IIR-A\n",
                   " G22 BLOCK IIR-A\n",
                   " G23 BLOCK IIIA\n",
                   " G24 BLOCK IIF\n",
                   " G25 BLOCK IIF\n",
                   " G26 BLOCK IIF\n",
                   " G27 BLOCK IIF\n",
                   " G28 BLOCK IIIA\n",
                   " G29 BLOCK IIR-M\n",
                   " G30 BLOCK IIF\n",
                   " G31 BLOCK IIR-M\n",
                   " G32 BLOCK IIF\n",
                   " C06 BEIDOU-2I\n",
                   " C07 BEIDOU-2I\n",
                   " C08 BEIDOU-2I\n",
                   " C09 BEIDOU-2I\n",
                   " C10 BEIDOU-2I\n",
                   " C11 BEIDOU-2M\n",
                   " C12 BEIDOU-2M\n",
                   " C13 BEIDOU-2I\n",
                   " C14 BEIDOU-2M\n",
                   " C16 BEIDOU-2I\n",
                   " C19 BEIDOU-3M-CAST\n",
                   " C20 BEIDOU-3M-CAST\n",
                   " C21 BEIDOU-3M-CAST\n",
                   " C22 BEIDOU-3M-CAST\n",
                   " C23 BEIDOU-3M-CAST\n",
                   " C24 BEIDOU-3M-CAST\n",
                   " C25 BEIDOU-3M-SECM\n",
                   " C26 BEIDOU-3M-SECM\n",
                   " C27 BEIDOU-3M-SECM\n",
                   " C28 BEIDOU-3M-SECM\n",
                   " C29 BEIDOU-3M-SECM\n",
                   " C30 BEIDOU-3M-SECM\n",
                   " C32 BEIDOU-3M-CAST\n",
                   " C33 BEIDOU-3M-CAST\n",
                   " C34 BEIDOU-3M-SECM\n",
                   " C35 BEIDOU-3M-SECM\n",
                   " C36 BEIDOU-3M-CAST\n",
                   " C37 BEIDOU-3M-CAST\n",
                   " C38 BEIDOU-3I\n",
                   " C39 BEIDOU-3I\n",
                   " C40 BEIDOU-3I\n",
                   " C41 BEIDOU-3M-CAST\n",
                   " C42 BEIDOU-3M-CAST\n",
                   " C43 BEIDOU-3M-SECM\n",
                   " C44 BEIDOU-3M-SECM\n",
                   " C45 BEIDOU-3M-CAST\n",
                   " C46 BEIDOU-3M-CAST\n",
                   " E01 GALILEO-2\n",
                   " E02 GALILEO-2\n",
                   " E03 GALILEO-2\n",
                   " E04 GALILEO-2\n",
                   " E05 GALILEO-2\n",
                   " E07 GALILEO-2\n",
                   " E08 GALILEO-2\n",
                   " E09 GALILEO-2\n",
                   " E11 GALILEO-1\n",
                   " E12 GALILEO-1\n",
                   " E13 GALILEO-2\n",
                   " E14 GALILEO-2\n",
                   " E15 GALILEO-2\n",
                   " E18 GALILEO-2\n",
                   " E19 GALILEO-1\n",
                   " E20 GALILEO-1\n",
                   " E21 GALILEO-2\n",
                   " E22 GALILEO-2\n",
                   " E24 GALILEO-2\n",
                   " E25 GALILEO-2\n",
                   " E26 GALILEO-2\n",
                   " E27 GALILEO-2\n",
                   " E30 GALILEO-2\n",
                   " E31 GALILEO-2\n",
                   " E33 GALILEO-2\n",
                   " E36 GALILEO-2\n",
                   "-SATELLITE/ID_AND_DESCRIPTION\n",
                   "+EPHEMERIS/DAT\n"};
static const uint8_t ssr_sig_gps[MAXCODE] = {CODE_L1C, CODE_L2W, CODE_L5X, CODE_L5Q};
static const uint8_t ssr_sig_gal[MAXCODE] = {CODE_L1X, CODE_L1C, CODE_L5X, CODE_L5Q, CODE_L7X, CODE_L7Q};
static const uint8_t ssr_sig_qzs[MAXCODE] = {CODE_L1C, CODE_L2X, CODE_L5Q};
static const uint8_t ssr_sig_cmp[MAXCODE] = {CODE_L2I, CODE_L6I, CODE_L5X, CODE_L7I};
static const uint8_t *headcodes[MAXSYS] = {ssr_sig_gps, ssr_sig_gal, ssr_sig_cmp, ssr_sig_cmp, ssr_sig_qzs};

typedef struct ssrclk
{
    int wk;
    double sow;
    int iode[MAXSAT];
    int iod[MAXSAT];
    double c[MAXSAT][3];
} SSRCLK;
typedef struct SSRorb
{
    int wk;
    int iode[MAXSAT];
    double sow;
    int iod[MAXSAT];
    double dx[MAXSAT][6];
} SSRORB;
typedef struct SSRyaw_pbias
{
    int iode;
    int wk;
    double sow;
    int iod[MAXSAT];
    double yaw[MAXSAT][2];
    double pbias[MAXSAT][MAXCODE];
} SSRYAW_PBIAS;

typedef struct SSRcbias
{
    int iode;
    int wk;
    double sow;
    int iod[MAXSAT];
    double cbias[MAXSAT][MAXCODE];
} SSRCBIAS;
typedef struct SSRifpb
{
    int iode;
    int wk;
    double sow;
    int iod[MAXSAT];
    double ifpb[MAXSAT];
} SSRCIFPB;

static void m_wtclkheader(gtime_t td, FILE *fp)
{
    double ep[6];
    time2epoch(td, ep);
    char *line[] = {
        "     3.00           C                                       RINEX VERSION / TYPE\n",
        "     5.00                                                   INTERVAL\n",
        "PANDA-EXTCLK        WHU                 %04d%02d%02d            PGM / RUN BY / DATE\n",
        "G                   IGS20.ATX                               SYS / PCVS APPLIED\n",
        "R                   IGS20.ATX                               SYS / PCVS APPLIED\n",
        "C                   IGS20.ATX                               SYS / PCVS APPLIED\n",
        "E                   IGS20.ATX                               SYS / PCVS APPLIED\n",
        "    2    AR    AS                                           # / TYPES OF DATA\n",
        "WHU WUHAN UNIVERSITY                                        ANALYSIS CENTER\n",
        "  115                                                       # OF SOLN SATS\n",
        "G01 G02 G03 G05 G06 G07 G08 G09 G10 G11 G12 G13 G14 G15 G16 PRN LIST\n",
        "G17 G18 G19 G20 G21 G22 G24 G25 G26 G27 G28 G29 G30 G31 G32 PRN LIST\n",
        "R01 R02 R03 R04 R05 R07 R08 R09 R11 R12 R13 R14 R15 R16 R17 PRN LIST\n",
        "R18 R19 R20 R21 R22 R23 R24 C01 C02 C03 C06 C07 C08 C09 C10 PRN LIST\n",
        "C11 C12 C13 C14 C16 C19 C20 C21 C22 C23 C24 C25 C26 C27 C28 PRN LIST\n",
        "C29 C30 C32 C33 C34 C35 C36 C37 C38 C39 C40 C41 C42 C43 C44 PRN LIST\n",
        "C45 C46 C59 E01 E02 E03 E04 E05 E07 E08 E09 E11 E12 E13 E15 PRN LIST\n",
        "E19 E21 E24 E25 E26 E27 E30 E31 E33 E36                     PRN LIST\n",
        "                                                            END OF HEADER\n"};
    for (int i = 0; i < 19; i++)
    {
        if (strstr(line[i], "PGM / RUN BY / DATE"))

            fprintf(fp, line[i], (int)ep[0], (int)ep[1], (int)ep[2]);
        else
            fprintf(fp, line[i]);
    }

    fflush(fp);
}

static void m_wtsp3header(gtime_t td, FILE *fp, int dintv)
{
    char *line[] = {
        "+  104   G01G02G03G04G05G06G07G08G09G10G11G12G13G14G15G16G17\n",
        "+        G18G19G20G21G22G23G24G25G26G27G28G29G30G31G32C01C02\n",
        "+        C03C04C05C06C07C08C09C10C11C12C13C14C15C16C17C18C19\n",
        "+        C20C21C22C23C24C25C26C27C28C29C30C31C32C33C34C35C36\n",
        "+        C37C38C39C40C41C42C43C44C45C46E01E02E03E04E05E07E08\n",
        "+        E09E11E12E13E15E19E21E24E25E19E21E24E25E26E27E30E31\n",
        "+        E33E36  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n",
        "%c M  cc GPS ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc\n",
        "%c cc cc ccc ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc\n",
        "%f  1.2500000  1.025000000  0.00000000000  0.000000000000000\n",
        "%f  0.0000000  0.000000000  0.00000000000  0.000000000000000\n",
        "%i    0    0    0    0      0      0      0      0         0\n",
        "%i    0    0    0    0      0      0      0      0         0\n"};
    int gweek;
    double ep[6], sod, gsow, mjd;
    time2epoch(td, ep);
    gsow = time2gpst(td, &gweek);
    mjd = time2mjd(td);
    fprintf(fp, "#dP%04d%3d%3d%3d%3d%12.8lf      96   u+U IGS14 FIT  WHU\n", (int)ep[0], (int)ep[1], (int)ep[2],
            (int)ep[3], (int)ep[4], ep[5]);
    fprintf(fp, "## %4d%16.8lf%15.8lf %d%16.8lf\n", gweek, gsow, (double)dintv, (int)mjd, (mjd - (int)mjd) * 86400);
    for (int i = 0; i < 13; i++)
        fprintf(fp, "%s", line[i]);
    fflush(fp);
}
extern void outputeph(eph_t *eph, FILE *fp)
{
    double epo[6];
    char cprn[4];
    double sec;
    int week, toes;
    gtime_t time;
    satno2id(eph->sat, cprn);

    if (cprn[0] == 'C')
        time2epoch(gpst2bdt(eph->toe), epo);
    else
        time2epoch(eph->toe, epo);

    if (eph->sat > 0)
    {
        switch (cprn[0])
        {
        case 'G':
            fprintf(fp, "%3s %4d %02d %02d %02d %02d %02d%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n",
                    cprn, (int)epo[0], (int)epo[1], (int)epo[2], (int)epo[3], (int)epo[4], (int)epo[5], eph->f0, eph->f1, eph->f2,
                    (double)eph->iode, eph->crs, eph->deln, eph->M0,
                    eph->cuc, eph->e, eph->cus, sqrt(eph->A),
                    eph->toes, eph->cic, eph->OMG0, eph->cis,
                    eph->i0, eph->crc, eph->omg, eph->OMGd,
                    (double)eph->idot, 0.0, (double)eph->week, 0.0,
                    (double)eph->sva, (double)eph->svh, eph->tgd[0], (double)eph->iodc,
                    0.0, 0.0, 0.0, 0);
            // printf("%3s %4d %02d %02d %02d %02d %02d%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n",
            //        cprn, (int)epo[0], (int)epo[1], (int)epo[2], (int)epo[3], (int)epo[4], (int)epo[5], eph->f0, eph->f1, eph->f2, (double)eph->iode, eph->crs, eph->deln, eph->M0, eph->cuc, eph->e, eph->cus, sqrt(eph->A), eph->toes, eph->cic, eph->OMG0, eph->cis, eph->i0, eph->crc, eph->omg, eph->OMGd, (double)eph->idot, (double)eph->code, (double)eph->week, 0.0, (double)eph->sva, eph->svh, eph->tgd[0], (double)eph->iodc, 0.0, (double)eph->fit, 0.0, 0);

            fflush(fp);
            break;
        case 'E':
            fprintf(fp, "%3s %4d %02d %02d %02d %02d %02d%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n",
                    cprn, (int)epo[0], (int)epo[1], (int)epo[2], (int)epo[3], (int)epo[4], (int)epo[5], eph->f0, eph->f1, eph->f2,
                    (double)eph->iode, eph->crs, eph->deln, eph->M0,
                    eph->cuc, eph->e, eph->cus, sqrt(eph->A),
                    eph->toes, eph->cic, eph->OMG0, eph->cis,
                    eph->i0, eph->crc, eph->omg, eph->OMGd,
                    (double)eph->idot, (double)eph->code, (double)eph->week, 0.0,
                    (double)eph->sva, (double)eph->svh, eph->tgd[0], (double)eph->tgd[1], (double)eph->toes, 0.0, 0.0, 0.0);
            // printf("%3s %4d %02d %02d %02d %02d %02d%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n",
            //        cprn, (int)epo[0], (int)epo[1], (int)epo[2], (int)epo[3], (int)epo[4], (int)epo[5], eph->f0, eph->f1, eph->f2, (double)eph->iode, eph->crs, eph->deln, eph->M0, eph->cuc, eph->e, eph->cus, sqrt(eph->A), eph->toes, eph->cic, eph->OMG0, eph->cis, eph->i0, eph->crc, eph->omg, eph->OMGd, (double)eph->idot, (double)eph->code, (double)eph->week, 0.0, (double)eph->sva, eph->svh, eph->tgd[0], (double)eph->tgd[1], 0.0, (double)eph->iodc, 0.0, 0.0);

            // fflush(fp);
            break;
        case 'C':
            fprintf(fp, "%3s %4d %02d %02d %02d %02d %02d%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n",
                    cprn, (int)epo[0], (int)epo[1], (int)epo[2], (int)epo[3], (int)epo[4], (int)epo[5], eph->f0, eph->f1, eph->f2,
                    (double)eph->iode, eph->crs, eph->deln, eph->M0,
                    eph->cuc, eph->e, eph->cus, sqrt(eph->A),
                    eph->toes - 14, eph->cic, eph->OMG0, eph->cis,
                    eph->i0, eph->crc, eph->omg, eph->OMGd,
                    (double)eph->idot, 0.0, (double)eph->week, 0.0,
                    (double)eph->sva, (double)eph->svh, eph->tgd[0], (double)eph->tgd[1],
                    0.0, (double)eph->iodc, 0.0, 0);
            // printf("%3s %4d %02d %02d %02d %02d %02d%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n",
            //        cprn, (int)epo[0], (int)epo[1], (int)epo[2], (int)epo[3], (int)epo[4], epo[5], eph->f0, eph->f1, eph->f2, (double)eph->iode, eph->crs, eph->deln, eph->M0, eph->cuc, eph->e, eph->cus, sqrt(eph->A), eph->toe, eph->cic, eph->OMG0, eph->cis, eph->i0, eph->crc, eph->omg, eph->OMGd, (double)eph->idot, 0.0, (double)eph->week, 0.0, eph->sva, eph->svh, eph->tgd[0], (double)eph->tgd[1], 0.0, eph->iodc, 0.0, 0);
            // printf("%3s %4d %02d %02d %02d %02d %02d%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n    %19.12e%19.12e%19.12e%19.12e\n",
            //        cprn, (int)epo[0], (int)epo[1], (int)epo[2], (int)epo[3], (int)epo[4], (int)epo[5], eph->f0, eph->f1, eph->f2, (double)eph->iode, eph->crs, eph->deln, eph->M0, eph->cuc, eph->e, eph->cus, sqrt(eph->A), eph->toes, eph->cic, eph->OMG0, eph->cis, eph->i0, eph->crc, eph->omg, eph->OMGd, (double)eph->idot, 0.0, (double)eph->week, 0.0, (double)eph->sva, eph->svh, eph->tgd[0], (double)eph->tgd[1], 0.0, (double)eph->iodc, 0.0, 0);

            // fflush(fp);
            break;
        default:
            break;
        }
    }
}
static void outputclk(gtime_t td, int sat, nav_t *nav, int ditv)
{
    int lorb, svh;
    double ep[6] = {}, rs[6], dts[2], var;
    char cprn[4];
    time2epoch(td, ep);
    satno2id(sat, cprn);
    if (((int)ep[5]) % ditv == 0)
    {
        // if ((lorb = ephpos(td, td, sat, nav, -1, rs, dts, &var, &svh)))
        if ((lorb = satpos_ssr(td, td, sat, nav, 0, rs, dts, &var, &svh)))
            fprintf(getFileptr(td, NULL, NULL, "clk"), "AS %3s%6d%3d%3d%3d%3d%10.6lf%3d%22.12e\n", cprn, (int)ep[0], (int)ep[1], (int)ep[2],
                    (int)ep[3], (int)ep[4], ep[5], 1, dts[0]);
    }
    fflush(getFileptr(td, NULL, NULL, "clk"));
}
static int outputsp3(gtime_t td, int sat, nav_t *nav, double *rs, int ditv)
{
    int lorb, svh;
    static char sstring[2048] = {};
    static char slines[2048] = {};
    static gtime_t lt = {};
    static int nsats = 0;
    double ep[6], xsat[6], dts[2], var;
    char cprn[4];

    satno2id(sat, cprn);
    if (timediff(td, lt) > 0)
    {
        if (lt.time > 0)
        {
            time2epoch(lt, ep);
            if (((int)ep[5]) % ditv == 0)
            {
                if (nsats > 0)
                {
                    sprintf(sstring, "*  %4d%3d%3d%3d%3d%12.8lf\n%s", (int)ep[0], (int)ep[1], (int)ep[2], (int)ep[3], (int)ep[4], ep[5], slines);
                    fprintf(getFileptr(td, NULL, NULL, "sp3"), sstring);
                    fflush(getFileptr(td, NULL, NULL, "sp3"));
                }
                memset(sstring, 0, sizeof(sstring));
                memset(slines, 0, sizeof(slines));
                nsats = 0;
            }
        }
        lt = td;
    }
    time2epoch(td, ep);
    if (((int)ep[5]) % ditv == 0)
        if (lorb = satpos_ssr(td, td, sat, nav, 0, xsat, dts, &var, &svh))
        // fprintf(getFileptr(td, NULL, NULL, "sp3"), "%4d%3d%3d%3d%3d%12.8lf P%s%14.6lf%14.6lf%14.6lf%14.6lf\n" , (int)ep[0], (int)ep[1], (int)ep[2], (int)ep[3], (int)ep[4], ep[5], cprn, xsat[0] / 1000, xsat[1] / 1000, xsat[2] / 1000, 999.0);
        // fflush(getFileptr(td, NULL, NULL, "sp3"));
        {
            sprintf(slines, "%sP%s%14.6lf%14.6lf%14.6lf%14.6lf\n", slines, cprn, xsat[0] / 1000, xsat[1] / 1000, xsat[2] / 1000, 999.0);
            for (int i = 0; i < 6; i++)
            {
                rs[i] = xsat[i];
            }
            nsats++;
        }
    return lorb;
}
extern FILE *getFileptr(gtime_t td, filopt_t *fopt, prcopt_t *opt, char *type)
{
    FILE *fp = NULL;
    int week, doy, yr, i, j, k;
    double sow, ep[6];
    char filename[1024], stmp[1024] = {};
    static int tdoy[6] = {-1, -1, -1, -1, -1, -1}; // eph obs ssr clk sp3 obx
    gtime_t tnow = timeget();
    if (fabs(timediff(td, tnow)) > 3600)
        td = tnow;
    sow = time2gpst(td, &week);
    doy = time2doy(td);
    time2epoch(td, ep);
    yr = (int)(ep[0]) % 2000;
    // 4 types tdoy==0 feph==null tdoy==doy tdoy<doy
    if (strstr(type, "eph"))
    {
        if (tdoy[0] < doy)
        {
            if (tdoy[0] < 0)
            {
                feph = NULL;
            }
            else
            {
                fclose(feph);
                feph = NULL;
            }
        }
        if (feph == NULL)
        {
            sprintf(filename, "%s/brdm%03d0.%02dp", outdir, doy, yr);
            feph = fopen(filename, "w");
            if (feph == NULL)
            {
                trace(3, "open output eph file faild\n");
                exit(1);
            }
            for (i = 0; i < 4; i++)
            {

                if (strstr(ephhead[i], "PGM / RUN BY / DATE"))
                    fprintf(feph, ephhead[i], (int)ep[0], (int)ep[1], (int)ep[2]);
                else
                    fprintf(feph, ephhead[i]);
            }
            tdoy[0] = doy;
        }
        fp = feph;
    }
    else if (strstr(type, "obs"))
    {
        if (tdoy[1] < doy)
        {
            if (tdoy[1] < 0)
            {
                fobs = NULL;
            }
            else
            {
                fclose(fobs);
                fobs = NULL;
            }
        }
        if (fobs == NULL)
        {

            sprintf(filename, "%s/%4s%03d0.%02do", outdir, "whu", doy, yr);
            fobs = fopen(filename, "w");
            if (fobs == NULL)
            {
                trace(3, "open output obs file faild\n");
                exit(1);
            }
            int num = sizeof(obshead) / sizeof(obshead[0]);
            for (i = 0; i < num; i++)
            {
                if (strstr(obshead[i], "PGM / RUN BY / DATE"))
                    fprintf(fobs, obshead[i], (int)ep[0], (int)ep[1], (int)ep[2]);
                else if (strstr(obshead[i], "APPROX POSITION XYZ"))
                    fprintf(fobs, obshead[i], opt->ru[0], opt->ru[1], opt->ru[2]);
                else if (strstr(obshead[i], "TIME OF FIRST OBS"))
                    fprintf(fobs, obshead[i], (int)ep[0], (int)ep[1], (int)ep[2], (int)ep[3], (int)ep[4], ep[5]);
                else if (strstr(obshead[i], "RTCM_3"))
                    fprintf(fobs, obshead[i], "test" + 1);
                else if (strstr(obshead[i], "MARKER NAME"))
                    fprintf(fobs, obshead[i], "test");
                else
                    fprintf(fobs, obshead[i]);
            }
            tdoy[1] = doy;
        }
        fp = fobs;
    }
    else if (strstr(type, "clk"))
    {
        if (tdoy[3] < doy)
        {
            if (tdoy[3] < 0)
            {
                fclk = NULL;
            }
            else
            {
                fclose(fclk);
                fclk = NULL;
            }
        }

        if (fclk == NULL)
        {
            // sprintf(filename, "whu%04d%01d.clk", week, (int)sow / 86400);
            sprintf(filename, "%s/whu%04d%01d.clk", outdir, week, (int)sow / 86400);
            fclk = fopen(filename, "w");
            if (fclk == NULL)
            {
                trace(3, "open output eph file faild\n");
                exit(1);
            }
            m_wtclkheader(td, fclk);
            tdoy[3] = doy;
        }
        fp = fclk;
    }
    else if (strstr(type, "sp3"))
    {
        if (tdoy[4] < doy)
        {
            if (tdoy[4] < 0)
            {
                fsp3 = NULL;
            }
            else
            {
                fclose(fsp3);
                fsp3 = NULL;
            }
        }
        if (fsp3 == NULL)
        {
            sprintf(filename, "%s/whu%04d%01d.sp3", outdir, week, (int)sow / 86400);
            fsp3 = fopen(filename, "w");
            if (fsp3 == NULL)
            {
                trace(3, "open output eph file faild\n");
                exit(1);
            }
            m_wtsp3header(td, fsp3, 5);
            tdoy[4] = doy;
        }
        fp = fsp3;
    }
    else if (strstr(type, "obx"))
    {
        if (tdoy[5] < doy)
        {
            if (tdoy[5] < 0)
            {
                fobx = NULL;
            }
            else
            {
                fclose(fobx);
                fobx = NULL;
            }
        }
        if (fobx == NULL)
        {
            sprintf(filename, "%s/whu%04d%01d.obx", outdir, week, (int)sow / 86400);
            fobx = fopen(filename, "w");
            if (fobx == NULL)
            {
                trace(3, "open output eph file faild\n");
                exit(1);
            }
            int num = sizeof(obxhead) / sizeof(obxhead[0]);
            for (i = 0; i < num; i++)
            {
                fprintf(fobx, obxhead[i]);
            }
            tdoy[5] = doy;
        }
        fp = fobx;
    }
    return fp;
}

extern void *pthread_cal(void *x)
{
    gtime_t tnow;
    double ep[6], rs[6];
    int isat;
    while (1)
    {
        tnow = timeadd(timeget(),0);
        time2epoch(tnow, ep);

        if (((int)ep[5] % 5) == 0)
        {
            // printf("output ephclk\n");
            for (isat = 0; isat < nprn_; isat++)
            {
                outputclk(tnow, isat + 1, &g_sdk_nav,5);
                outputsp3(tnow, isat + 1, &g_sdk_nav, rs,5);
                // if (outputsp3(td, isat + 1, nav, rs))
                // continue;
                // outputobx(td, isat + 1, nav, rs);
                memset(rs, 0, sizeof(rs));
            }
            sleepms(4500);
        }
    }
}