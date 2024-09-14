#include "tinyppp.h"
#include <dirent.h>
#include <errno.h>
/* compare ephemeris ---------------------------------------------------------*/
static int cmpeph(const void *p1, const void *p2)
{
    eph_t *q1 = (eph_t *)p1, *q2 = (eph_t *)p2;
    return q1->ttr.time != q2->ttr.time ? (int)(q1->ttr.time - q2->ttr.time) : (q1->toe.time != q2->toe.time ? (int)(q1->toe.time - q2->toe.time) : q1->sat - q2->sat);
}
/* sort and unique ephemeris -------------------------------------------------*/
static void uniqeph(nav_t *nav)
{
    eph_t *nav_eph;
    int i, j;

    trace(3, "uniqeph: n=%d\n", nav->n);

    if (nav->n <= 0)
        return;

    qsort(nav->eph, nav->n, sizeof(eph_t), cmpeph);

    for (i = 1, j = 0; i < nav->n; i++)
    {
        if (nav->eph[i].sat != nav->eph[j].sat ||
            nav->eph[i].iode != nav->eph[j].iode)
        {
            nav->eph[++j] = nav->eph[i];
        }
    }
    nav->n = j + 1;

    if (!(nav_eph = (eph_t *)realloc(nav->eph, sizeof(eph_t) * nav->n)))
    {
        trace(1, "uniqeph malloc error n=%d\n", nav->n);
        free(nav->eph);
        nav->eph = NULL;
        nav->n = nav->nmax = 0;
        return;
    }
    nav->eph = nav_eph;
    nav->nmax = nav->n;

    trace(4, "uniqeph: n=%d\n", nav->n);
}
/* compare glonass ephemeris -------------------------------------------------*/
static int cmpgeph(const void *p1, const void *p2)
{
    geph_t *q1 = (geph_t *)p1, *q2 = (geph_t *)p2;
    return q1->tof.time != q2->tof.time ? (int)(q1->tof.time - q2->tof.time) : (q1->toe.time != q2->toe.time ? (int)(q1->toe.time - q2->toe.time) : q1->sat - q2->sat);
}
/* sort and unique glonass ephemeris -----------------------------------------*/
static void uniqgeph(nav_t *nav)
{
    geph_t *nav_geph;
    int i, j;

    trace(3, "uniqgeph: ng=%d\n", nav->ng);

    if (nav->ng <= 0)
        return;

    qsort(nav->geph, nav->ng, sizeof(geph_t), cmpgeph);

    for (i = j = 0; i < nav->ng; i++)
    {
        if (nav->geph[i].sat != nav->geph[j].sat ||
            nav->geph[i].toe.time != nav->geph[j].toe.time ||
            nav->geph[i].svh != nav->geph[j].svh)
        {
            nav->geph[++j] = nav->geph[i];
        }
    }
    nav->ng = j + 1;

    if (!(nav_geph = (geph_t *)realloc(nav->geph, sizeof(geph_t) * nav->ng)))
    {
        trace(1, "uniqgeph malloc error ng=%d\n", nav->ng);
        free(nav->geph);
        nav->geph = NULL;
        nav->ng = nav->ngmax = 0;
        return;
    }
    nav->geph = nav_geph;
    nav->ngmax = nav->ng;

    trace(4, "uniqgeph: ng=%d\n", nav->ng);
}
/* compare sbas ephemeris ----------------------------------------------------*/
static int cmpseph(const void *p1, const void *p2)
{
    seph_t *q1 = (seph_t *)p1, *q2 = (seph_t *)p2;
    return q1->tof.time != q2->tof.time ? (int)(q1->tof.time - q2->tof.time) : (q1->t0.time != q2->t0.time ? (int)(q1->t0.time - q2->t0.time) : q1->sat - q2->sat);
}
/* sort and unique sbas ephemeris --------------------------------------------*/
static void uniqseph(nav_t *nav)
{
    seph_t *nav_seph;
    int i, j;

    trace(3, "uniqseph: ns=%d\n", nav->ns);

    if (nav->ns <= 0)
        return;

    qsort(nav->seph, nav->ns, sizeof(seph_t), cmpseph);

    for (i = j = 0; i < nav->ns; i++)
    {
        if (nav->seph[i].sat != nav->seph[j].sat ||
            nav->seph[i].t0.time != nav->seph[j].t0.time)
        {
            nav->seph[++j] = nav->seph[i];
        }
    }
    nav->ns = j + 1;

    if (!(nav_seph = (seph_t *)realloc(nav->seph, sizeof(seph_t) * nav->ns)))
    {
        trace(1, "uniqseph malloc error ns=%d\n", nav->ns);
        free(nav->seph);
        nav->seph = NULL;
        nav->ns = nav->nsmax = 0;
        return;
    }
    nav->seph = nav_seph;
    nav->nsmax = nav->ns;

    trace(4, "uniqseph: ns=%d\n", nav->ns);
}
/* unique ephemerides ----------------------------------------------------------
 * unique ephemerides in navigation data and update carrier wave length
 * args   : nav_t *nav    IO     navigation data
 * return : number of epochs
 *-----------------------------------------------------------------------------*/
extern void uniqnav(nav_t *nav)
{
    trace(3, "uniqnav: neph=%d ngeph=%d nseph=%d\n", nav->n, nav->ng, nav->ns);

    /* unique ephemeris */
    uniqeph(nav);
    uniqgeph(nav);
    uniqseph(nav);
}
/* compare observation data -------------------------------------------------*/
static int cmpobs(const void *p1, const void *p2)
{
    obsd_t *q1 = (obsd_t *)p1, *q2 = (obsd_t *)p2;
    double tt = timediff(q1->time, q2->time);
    if (fabs(tt) > DTTOL)
        return tt < 0 ? -1 : 1;
    if (q1->rcv != q2->rcv)
        return (int)q1->rcv - (int)q2->rcv;
    return (int)q1->sat - (int)q2->sat;
}
/* sort and unique observation data --------------------------------------------
 * sort and unique observation data by time, rcv, sat
 * args   : obs_t *obs    IO     observation data
 * return : number of epochs
 *-----------------------------------------------------------------------------*/
extern int sortobs(obs_t *obs)
{
    int i, j, n;

    trace(3, "sortobs: nobs=%d\n", obs->n);

    if (obs->n <= 0)
        return 0;

    qsort(obs->data, obs->n, sizeof(obsd_t), cmpobs);

    /* delete duplicated data */
    for (i = j = 0; i < obs->n; i++)
    {
        if (obs->data[i].sat != obs->data[j].sat ||
            obs->data[i].rcv != obs->data[j].rcv ||
            timediff(obs->data[i].time, obs->data[j].time) != 0.0)
        {
            obs->data[++j] = obs->data[i];
        }
    }
    obs->n = j + 1;

    for (i = n = 0; i < obs->n; i = j, n++)
    {
        for (j = i + 1; j < obs->n; j++)
        {
            if (timediff(obs->data[j].time, obs->data[i].time) > DTTOL)
                break;
        }
    }
    return n;
}
/* screen by time --------------------------------------------------------------
 * screening by time start, time end, and time interval
 * args   : gtime_t time  I      time
 *          gtime_t ts    I      time start (ts.time==0:no screening by ts)
 *          gtime_t te    I      time end   (te.time==0:no screening by te)
 *          double  tint  I      time interval (s) (0.0:no screen by tint)
 * return : 1:on condition, 0:not on condition
 *-----------------------------------------------------------------------------*/
extern int screent(gtime_t time, gtime_t ts, gtime_t te, double tint)
{
    return (tint <= 0.0 || fmod(time2gpst(time, NULL) + DTTOL, tint) <= DTTOL * 2.0) &&
           (ts.time == 0 || timediff(time, ts) >= -DTTOL) &&
           (te.time == 0 || timediff(time, te) < DTTOL);
}
/* read/save navigation data ---------------------------------------------------
 * save or load navigation data
 * args   : char    file  I      file path
 *          nav_t   nav   O/I    navigation data
 * return : status (1:ok,0:no file)
 *-----------------------------------------------------------------------------*/
extern int readnav(const char *file, nav_t *nav)
{
    FILE *fp;
    eph_t eph0 = {0};
    geph_t geph0 = {0};
    char buff[4096], *p;
    long toe_time, tof_time, toc_time, ttr_time;
    int i, sat, prn;

    trace(3, "loadnav: file=%s\n", file);

    if (!(fp = fopen(file, "r")))
        return 0;

    while (fgets(buff, sizeof(buff), fp))
    {
        if (!strncmp(buff, "IONUTC", 6))
        {
            for (i = 0; i < 8; i++)
                nav->ion_gps[i] = 0.0;
            for (i = 0; i < 8; i++)
                nav->utc_gps[i] = 0.0;
            (void)sscanf(buff, "IONUTC,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
                         &nav->ion_gps[0], &nav->ion_gps[1], &nav->ion_gps[2], &nav->ion_gps[3],
                         &nav->ion_gps[4], &nav->ion_gps[5], &nav->ion_gps[6], &nav->ion_gps[7],
                         &nav->utc_gps[0], &nav->utc_gps[1], &nav->utc_gps[2], &nav->utc_gps[3],
                         &nav->utc_gps[4]);
            continue;
        }
        if ((p = strchr(buff, ',')))
            *p = '\0';
        else
            continue;
        if (!(sat = satid2no(buff)))
            continue;
        if (satsys(sat, &prn) == SYS_GLO)
        {
            nav->geph[prn - 1] = geph0;
            nav->geph[prn - 1].sat = sat;
            toe_time = tof_time = 0;
            (void)sscanf(p + 1,
                         "%d,%d,%d,%d,%d,%ld,%ld,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,"
                         "%lf,%lf,%lf,%lf",
                         &nav->geph[prn - 1].iode, &nav->geph[prn - 1].frq, &nav->geph[prn - 1].svh,
                         &nav->geph[prn - 1].sva, &nav->geph[prn - 1].age,
                         &toe_time, &tof_time,
                         &nav->geph[prn - 1].pos[0], &nav->geph[prn - 1].pos[1], &nav->geph[prn - 1].pos[2],
                         &nav->geph[prn - 1].vel[0], &nav->geph[prn - 1].vel[1], &nav->geph[prn - 1].vel[2],
                         &nav->geph[prn - 1].acc[0], &nav->geph[prn - 1].acc[1], &nav->geph[prn - 1].acc[2],
                         &nav->geph[prn - 1].taun, &nav->geph[prn - 1].gamn, &nav->geph[prn - 1].dtaun);
            nav->geph[prn - 1].toe.time = toe_time;
            nav->geph[prn - 1].tof.time = tof_time;
        }
        else
        {
            nav->eph[sat - 1] = eph0;
            nav->eph[sat - 1].sat = sat;
            toe_time = toc_time = ttr_time = 0;
            (void)sscanf(p + 1,
                         "%d,%d,%d,%d,%ld,%ld,%ld,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,"
                         "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%d",
                         &nav->eph[sat - 1].iode, &nav->eph[sat - 1].iodc, &nav->eph[sat - 1].sva,
                         &nav->eph[sat - 1].svh,
                         &toe_time, &toc_time, &ttr_time,
                         &nav->eph[sat - 1].A, &nav->eph[sat - 1].e, &nav->eph[sat - 1].i0,
                         &nav->eph[sat - 1].OMG0, &nav->eph[sat - 1].omg, &nav->eph[sat - 1].M0,
                         &nav->eph[sat - 1].deln, &nav->eph[sat - 1].OMGd, &nav->eph[sat - 1].idot,
                         &nav->eph[sat - 1].crc, &nav->eph[sat - 1].crs, &nav->eph[sat - 1].cuc,
                         &nav->eph[sat - 1].cus, &nav->eph[sat - 1].cic, &nav->eph[sat - 1].cis,
                         &nav->eph[sat - 1].toes, &nav->eph[sat - 1].fit, &nav->eph[sat - 1].f0,
                         &nav->eph[sat - 1].f1, &nav->eph[sat - 1].f2, &nav->eph[sat - 1].tgd[0],
                         &nav->eph[sat - 1].code, &nav->eph[sat - 1].flag);
            nav->eph[sat - 1].toe.time = toe_time;
            nav->eph[sat - 1].toc.time = toc_time;
            nav->eph[sat - 1].ttr.time = ttr_time;
        }
    }
    fclose(fp);
    return 1;
}
extern int savenav(const char *file, const nav_t *nav)
{
    FILE *fp;
    int i;
    char id[32];

    trace(3, "savenav: file=%s\n", file);

    if (!(fp = fopen(file, "w")))
        return 0;

    for (i = 0; i < MAXSAT; i++)
    {
        if (nav->eph[i].ttr.time == 0)
            continue;
        satno2id(nav->eph[i].sat, id);
        fprintf(fp,
                "%s,%d,%d,%d,%d,%d,%d,%d,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,"
                "%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,"
                "%.14E,%.14E,%.14E,%.14E,%.14E,%d,%d\n",
                id, nav->eph[i].iode, nav->eph[i].iodc, nav->eph[i].sva,
                nav->eph[i].svh, (int)nav->eph[i].toe.time,
                (int)nav->eph[i].toc.time, (int)nav->eph[i].ttr.time,
                nav->eph[i].A, nav->eph[i].e, nav->eph[i].i0, nav->eph[i].OMG0,
                nav->eph[i].omg, nav->eph[i].M0, nav->eph[i].deln, nav->eph[i].OMGd,
                nav->eph[i].idot, nav->eph[i].crc, nav->eph[i].crs, nav->eph[i].cuc,
                nav->eph[i].cus, nav->eph[i].cic, nav->eph[i].cis, nav->eph[i].toes,
                nav->eph[i].fit, nav->eph[i].f0, nav->eph[i].f1, nav->eph[i].f2,
                nav->eph[i].tgd[0], nav->eph[i].code, nav->eph[i].flag);
    }
    for (i = 0; i < MAXPRNGLO; i++)
    {
        if (nav->geph[i].tof.time == 0)
            continue;
        satno2id(nav->geph[i].sat, id);
        fprintf(fp,
                "%s,%d,%d,%d,%d,%d,%d,%d,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,"
                "%.14E,%.14E,%.14E,%.14E,%.14E,%.14E\n",
                id, nav->geph[i].iode, nav->geph[i].frq, nav->geph[i].svh,
                nav->geph[i].sva, nav->geph[i].age, (int)nav->geph[i].toe.time,
                (int)nav->geph[i].tof.time,
                nav->geph[i].pos[0], nav->geph[i].pos[1], nav->geph[i].pos[2],
                nav->geph[i].vel[0], nav->geph[i].vel[1], nav->geph[i].vel[2],
                nav->geph[i].acc[0], nav->geph[i].acc[1], nav->geph[i].acc[2],
                nav->geph[i].taun, nav->geph[i].gamn, nav->geph[i].dtaun);
    }
    /*fprintf(fp,"IONUTC,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,%.14E,"
               "%.14E,%.14E,%.14E,%.0f",
            nav->ion_gps[0],nav->ion_gps[1],nav->ion_gps[2],nav->ion_gps[3],
            nav->ion_gps[4],nav->ion_gps[5],nav->ion_gps[6],nav->ion_gps[7],
            nav->utc_gps[0],nav->utc_gps[1],nav->utc_gps[2],nav->utc_gps[3],
            nav->utc_gps[4]);*/

    fclose(fp);
    return 1;
}
/* free observation data -------------------------------------------------------
 * free memory for observation data
 * args   : obs_t *obs    IO     observation data
 * return : none
 *-----------------------------------------------------------------------------*/
extern void freeobs(obs_t *obs)
{
    free(obs->data);
    obs->data = NULL;
    obs->n = obs->nmax = 0;
}
/* free navigation data ---------------------------------------------------------
 * free memory for navigation data
 * args   : nav_t *nav    IO     navigation data
 *          int   opt     I      option (or of followings)
 *                               (0x01: gps/qzs ephmeris, 0x02: glonass ephemeris,
 *                                0x04: sbas ephemeris,   0x08: precise ephemeris,
 *                                0x10: precise clock     0x20: almanac,
 *                                0x40: tec data)
 * return : none
 *-----------------------------------------------------------------------------*/
extern void freenav(nav_t *nav, int opt)
{
    if (opt & 0x01)
    {
        free(nav->eph);
        nav->eph = NULL;
        nav->n = nav->nmax = 0;
    }
    if (opt & 0x02)
    {
        free(nav->geph);
        nav->geph = NULL;
        nav->ng = nav->ngmax = 0;
    }
    if (opt & 0x04)
    {
        free(nav->seph);
        nav->seph = NULL;
        nav->ns = nav->nsmax = 0;
    }
    if (opt & 0x08)
    {
        free(nav->peph);
        nav->peph = NULL;
        nav->ne = nav->nemax = 0;
    }
    if (opt & 0x10)
    {
        free(nav->pclk);
        nav->pclk = NULL;
        nav->nc = nav->ncmax = 0;
    }
}

/* execute command -------------------------------------------------------------
 * execute command cbuf by operating system shell
 * args   : char   *cmd      I   command cbuf
 * return : execution status (0:ok,0>:error)
 *-----------------------------------------------------------------------------*/
extern int execcmd(const char *cmd)
{
#ifdef WIN32
    PROCESS_INFORMATION info;
    STARTUPINFO si = {0};
    DWORD stat;
    char cmds[1024];

    trace(3, "execcmd: cmd=%s\n", cmd);

    si.cb = sizeof(si);
    sprintf(cmds, "cmd /c %s", cmd);
    if (!CreateProcess(NULL, (LPTSTR)cmds, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL,
                       NULL, &si, &info))
        return -1;
    WaitForSingleObject(info.hProcess, INFINITE);
    if (!GetExitCodeProcess(info.hProcess, &stat))
        stat = -1;
    CloseHandle(info.hProcess);
    CloseHandle(info.hThread);
    return (int)stat;
#else
    trace(3, "execcmd: cmd=%s\n", cmd);

    return system(cmd);
#endif
}

#ifndef IO_FILE
/* expand file path ------------------------------------------------------------
 * expand file path with wild-card (*) in file
 * args   : char   *path     I   file path to expand (captal insensitive)
 *          char   *paths    O   expanded file paths
 *          int    nmax      I   max number of expanded file paths
 * return : number of expanded file paths
 * notes  : the order of expanded files is alphabetical order
 *-----------------------------------------------------------------------------*/
extern int expath(const char *path, char *paths[], int nmax)
{
    int i, j, n = 0;
    char tmp[1024];
#ifdef WIN32
    WIN32_FIND_DATA file;
    HANDLE h;
    char dir[1024] = "", *p;

    trace(3, "expath  : path=%s nmax=%d\n", path, nmax);

    if ((p = strrchr(path, '\\')))
    {
        strncpy(dir, path, p - path + 1);
        dir[p - path + 1] = '\0';
    }
    if ((h = FindFirstFile((LPCTSTR)path, &file)) == INVALID_HANDLE_VALUE)
    {
        strcpy(paths[0], path);
        return 1;
    }
    sprintf(paths[n++], "%s%s", dir, file.cFileName);
    while (FindNextFile(h, &file) && n < nmax)
    {
        if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        sprintf(paths[n++], "%s%s", dir, file.cFileName);
    }
    FindClose(h);
#else
    struct dirent *d;
    DIR *dp;
    const char *file = path;
    char dir[1024] = "", s1[1024], s2[1024], *p, *q, *r;

    trace(3, "expath  : path=%s nmax=%d\n", path, nmax);

    if ((p = strrchr(path, '/')) || (p = strrchr(path, '\\')))
    {
        file = p + 1;
        strncpy(dir, path, p - path + 1);
        dir[p - path + 1] = '\0';
    }
    if (!(dp = opendir(*dir ? dir : ".")))
        return 0;
    while ((d = readdir(dp)))
    {
        if (*(d->d_name) == '.')
            continue;
        sprintf(s1, "^%s$", d->d_name);
        sprintf(s2, "^%s$", file);
        for (p = s1; *p; p++)
            *p = (char)tolower((int)*p);
        for (p = s2; *p; p++)
            *p = (char)tolower((int)*p);

        for (p = s1, q = strtok_r(s2, "*", &r); q; q = strtok_r(NULL, "*", &r))
        {
            if ((p = strstr(p, q)))
                p += strlen(q);
            else
                break;
        }
        if (p && n < nmax)
            sprintf(paths[n++], "%s%s", dir, d->d_name);
    }
    closedir(dp);
#endif
    /* sort paths in alphabetical order */
    for (i = 0; i < n - 1; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (strcmp(paths[i], paths[j]) > 0)
            {
                strcpy(tmp, paths[i]);
                strcpy(paths[i], paths[j]);
                strcpy(paths[j], tmp);
            }
        }
    }
    for (i = 0; i < n; i++)
        trace(3, "expath  : file=%s\n", paths[i]);

    return n;
}

#endif
/* generate local directory recursively --------------------------------------*/
static int mkdir_r(const char *dir)
{
    char pdir[1024], *p;

#ifdef WIN32
    HANDLE h;
    WIN32_FIND_DATA data;

    if (!*dir || !strcmp(dir + 1, ":\\"))
        return 1;

    sprintf(pdir, "%.1023s", dir);
    if ((p = strrchr(pdir, FILEPATHSEP)))
    {
        *p = '\0';
        h = FindFirstFile(pdir, &data);
        if (h == INVALID_HANDLE_VALUE)
        {
            if (!mkdir_r(pdir))
                return 0;
        }
        else
            FindClose(h);
    }
    if (CreateDirectory(dir, NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return 1;
    }
#else
    FILE *fp;

    if (!*dir)
        return 1;

    sprintf(pdir, "%.1023s", dir);
    if ((p = strrchr(pdir, FILEPATHSEP)))
    {
        *p = '\0';
        if (!(fp = fopen(pdir, "r")))
        {
            if (!mkdir_r(pdir))
                return 0;
        }
        else
            fclose(fp);
    }
    if (!mkdir(dir, 0777) || errno == EEXIST)
        return 1;
#endif
    trace(2, "directory generation error: dir=%s\n", dir);
    return 0;
}
/* create directory ------------------------------------------------------------
 * create directory if not exists
 * args   : char   *path     I   file path to be saved
 * return : none
 * notes  : recursively.
 *-----------------------------------------------------------------------------*/
extern void createdir(const char *path)
{
    char buff[1024], *p;

    tracet(3, "createdir: path=%s\n", path);

    strcpy(buff, path);
    if (!(p = strrchr(buff, FILEPATHSEP)))
        return;
    *p = '\0';

    mkdir_r(buff);
}
/* replace string ------------------------------------------------------------*/
static int repstr(char *str, const char *pat, const char *rep)
{
    int len = (int)strlen(pat);
    char buff[1024] = {'\0'}, *p, *q, *r;

    for (p = str, r = buff; *p; p = q + len)
    {
        if (!(q = strstr(p, pat)))
            break;
        strncpy(r, p, q - p);
        r += q - p;
        r += sprintf(r, "%s", rep);
    }
    if (p <= str)
        return 0;
    strcpy(r, p);
    strcpy(str, buff);
    return 1;
}
/* replace keywords in file path -----------------------------------------------
 * replace keywords in file path with date, time, rover and base station id
 * args   : char   *path     I   file path (see below)
 *          char   *rpath    O   file path in which keywords replaced (see below)
 *          gtime_t time     I   time (gpst)  (time.time==0: not replaced)
 *          char   *rov      I   rover id string        ("": not replaced)
 *          char   *base     I   base station id string ("": not replaced)
 * return : status (1:keywords replaced, 0:no valid keyword in the path,
 *                  -1:no valid time)
 * notes  : the following keywords in path are replaced by date, time and name
 *              %Y -> yyyy : year (4 digits) (1900-2099)
 *              %y -> yy   : year (2 digits) (00-99)
 *              %m -> mm   : month           (01-12)
 *              %d -> dd   : day of month    (01-31)
 *              %h -> hh   : hours           (00-23)
 *              %M -> mm   : minutes         (00-59)
 *              %S -> ss   : seconds         (00-59)
 *              %n -> ddd  : day of year     (001-366)
 *              %W -> wwww : gps week        (0001-9999)
 *              %D -> d    : day of gps week (0-6)
 *              %H -> h    : hour code       (a=0,b=1,c=2,...,x=23)
 *              %ha-> hh   : 3 hours         (00,03,06,...,21)
 *              %hb-> hh   : 6 hours         (00,06,12,18)
 *              %hc-> hh   : 12 hours        (00,12)
 *              %t -> mm   : 15 minutes      (00,15,30,45)
 *              %r -> rrrr : rover id
 *              %b -> bbbb : base station id
 *-----------------------------------------------------------------------------*/
extern int reppath(const char *path, char *rpath, gtime_t time, const char *rov, const char *base)
{
    double ep[6], ep0[6] = {2000, 1, 1, 0, 0, 0};
    int week, dow, doy, stat = 0;
    char rep[64];

    strcpy(rpath, path);

    if (!strstr(rpath, "%"))
        return 0;
    if (*rov)
        stat |= repstr(rpath, "%r", rov);
    if (*base)
        stat |= repstr(rpath, "%b", base);
    if (time.time != 0)
    {
        time2epoch(time, ep);
        ep0[0] = ep[0];
        dow = (int)floor(time2gpst(time, &week) / 86400.0);
        doy = (int)floor(timediff(time, epoch2time(ep0)) / 86400.0) + 1;
        sprintf(rep, "%02d", ((int)ep[3] / 3) * 3);
        stat |= repstr(rpath, "%ha", rep);
        sprintf(rep, "%02d", ((int)ep[3] / 6) * 6);
        stat |= repstr(rpath, "%hb", rep);
        sprintf(rep, "%02d", ((int)ep[3] / 12) * 12);
        stat |= repstr(rpath, "%hc", rep);
        sprintf(rep, "%04.0f", ep[0]);
        stat |= repstr(rpath, "%Y", rep);
        sprintf(rep, "%02.0f", fmod(ep[0], 100.0));
        stat |= repstr(rpath, "%y", rep);
        sprintf(rep, "%02.0f", ep[1]);
        stat |= repstr(rpath, "%m", rep);
        sprintf(rep, "%02.0f", ep[2]);
        stat |= repstr(rpath, "%d", rep);
        sprintf(rep, "%02.0f", ep[3]);
        stat |= repstr(rpath, "%h", rep);
        sprintf(rep, "%02.0f", ep[4]);
        stat |= repstr(rpath, "%M", rep);
        sprintf(rep, "%02.0f", floor(ep[5]));
        stat |= repstr(rpath, "%S", rep);
        sprintf(rep, "%03d", doy);
        stat |= repstr(rpath, "%n", rep);
        sprintf(rep, "%04d", week);
        stat |= repstr(rpath, "%W", rep);
        sprintf(rep, "%d", dow);
        stat |= repstr(rpath, "%D", rep);
        sprintf(rep, "%c", 'a' + (int)ep[3]);
        stat |= repstr(rpath, "%H", rep);
        sprintf(rep, "%02d", ((int)ep[4] / 15) * 15);
        stat |= repstr(rpath, "%t", rep);
    }
    else if (strstr(rpath, "%ha") || strstr(rpath, "%hb") || strstr(rpath, "%hc") ||
             strstr(rpath, "%Y") || strstr(rpath, "%y") || strstr(rpath, "%m") ||
             strstr(rpath, "%d") || strstr(rpath, "%h") || strstr(rpath, "%M") ||
             strstr(rpath, "%S") || strstr(rpath, "%n") || strstr(rpath, "%W") ||
             strstr(rpath, "%D") || strstr(rpath, "%H") || strstr(rpath, "%t"))
    {
        return -1; /* no valid time */
    }
    return stat;
}
/* replace keywords in file path and generate multiple paths -------------------
 * replace keywords in file path with date, time, rover and base station id
 * generate multiple keywords-replaced paths
 * args   : char   *path     I   file path (see below)
 *          char   *rpath[]  O   file paths in which keywords replaced
 *          int    nmax      I   max number of output file paths
 *          gtime_t ts       I   time start (gpst)
 *          gtime_t te       I   time end   (gpst)
 *          char   *rov      I   rover id string        ("": not replaced)
 *          char   *base     I   base station id string ("": not replaced)
 * return : number of replaced file paths
 * notes  : see reppath() for replacements of keywords.
 *          minimum interval of time replaced is 900s.
 *-----------------------------------------------------------------------------*/
extern int reppaths(const char *path, char *rpath[], int nmax, gtime_t ts, gtime_t te, const char *rov, const char *base)
{
    gtime_t time;
    double tow, tint = 86400.0;
    int i, n = 0, week;

    trace(3, "reppaths: path =%s nmax=%d rov=%s base=%s\n", path, nmax, rov, base);

    if (ts.time == 0 || te.time == 0 || timediff(ts, te) > 0.0)
        return 0;

    if (strstr(path, "%S") || strstr(path, "%M") || strstr(path, "%t"))
        tint = 900.0;
    else if (strstr(path, "%h") || strstr(path, "%H"))
        tint = 3600.0;

    tow = time2gpst(ts, &week);
    time = gpst2time(week, floor(tow / tint) * tint);

    while (timediff(time, te) <= 0.0 && n < nmax)
    {
        reppath(path, rpath[n], time, rov, base);
        if (n == 0 || strcmp(rpath[n], rpath[n - 1]))
            n++;
        time = timeadd(time, tint);
    }
    for (i = 0; i < n; i++)
        trace(3, "reppaths: rpath=%s\n", rpath[i]);
    return n;
}

/* uncompress file -------------------------------------------------------------
 * uncompress (uncompress/unzip/uncompact hatanaka-compression/tar) file
 * args   : char   *file     I   input file
 *          char   *uncfile  O   uncompressed file
 * return : status (-1:error,0:not compressed file,1:uncompress completed)
 * note   : creates uncompressed file in tempolary directory
 *          gzip, tar and crx2rnx commands have to be installed in commands path
 *-----------------------------------------------------------------------------*/
extern int rtk_uncompress(const char *file, char *uncfile)
{
    int stat = 0;
    char *p, cmd[64 + 2048] = "", tmpfile[1024] = "", buff[1024], *fname, *dir = "";

    trace(3, "rtk_uncompress: file=%s\n", file);

    strcpy(tmpfile, file);
    if (!(p = strrchr(tmpfile, '.')))
        return 0;

    /* uncompress by gzip */
    if (!strcmp(p, ".z") || !strcmp(p, ".Z") ||
        !strcmp(p, ".gz") || !strcmp(p, ".GZ") ||
        !strcmp(p, ".zip") || !strcmp(p, ".ZIP"))
    {
        strcpy(uncfile, tmpfile);
        uncfile[p - tmpfile] = '\0';
        sprintf(cmd, "gzip -f -d -c \"%s\" > \"%s\"", tmpfile, uncfile);

        if (execcmd(cmd))
        {
            remove(uncfile);
            return -1;
        }
        strcpy(tmpfile, uncfile);
        stat = 1;
    }
    /* extract tar file */
    if ((p = strrchr(tmpfile, '.')) && !strcmp(p, ".tar"))
    {
        strcpy(uncfile, tmpfile);
        uncfile[p - tmpfile] = '\0';
        strcpy(buff, tmpfile);
        fname = buff;
#ifdef WIN32
        if ((p = strrchr(buff, '\\')))
        {
            *p = '\0';
            dir = fname;
            fname = p + 1;
        }
        sprintf(cmd, "set PATH=%%CD%%;%%PATH%% & cd /D \"%s\" & tar -xf \"%s\"",
                dir, fname);
#else
        if ((p = strrchr(buff, '/')))
        {
            *p = '\0';
            dir = fname;
            fname = p + 1;
        }
        sprintf(cmd, "tar -C \"%s\" -xf \"%s\"", dir, tmpfile);
#endif
        if (execcmd(cmd))
        {
            if (stat)
                remove(tmpfile);
            return -1;
        }
        if (stat)
            remove(tmpfile);
        stat = 1;
    }
    /* extract hatanaka-compressed file by cnx2rnx */
    else if ((p = strrchr(tmpfile, '.')) &&
             ((strlen(p) > 3 && (*(p + 3) == 'd' || *(p + 3) == 'D')) ||
              !strcmp(p, ".crx") || !strcmp(p, ".CRX")))
    {
        strcpy(uncfile, tmpfile);
        uncfile[p - tmpfile + 3] = *(p + 3) == 'D' ? 'O' : 'o';
        sprintf(cmd, "crx2rnx < \"%s\" > \"%s\"", tmpfile, uncfile);

        if (execcmd(cmd))
        {
            remove(uncfile);
            if (stat)
                remove(tmpfile);
            return -1;
        }
        if (stat)
            remove(tmpfile);
        stat = 1;
    }
    trace(3, "rtk_uncompress: stat=%d\n", stat);
    return stat;
}
