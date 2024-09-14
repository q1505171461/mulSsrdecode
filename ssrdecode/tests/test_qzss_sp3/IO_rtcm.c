#include "IO_rtcm.h"
#include "ctype.h"
nav_t g_sdk_nav = {0};
static rtcm_t s_rtcm_eph;
// by zzy
static int checkeph(nav_t *nav, const eph_t *eph)
{
    int ieph;
    for (ieph = 0; ieph < nav->n; ieph++)
    {
        if (nav->eph[ieph].sat == eph->sat)
        {
            if (timediff(eph->toe, nav->eph[ieph].toe) > 599)
                return ieph;
        }
    }
    return 0;
}
static int add_eph(nav_t *nav, const eph_t *eph)
{
    eph_t *nav_eph;
    int ieph = 0;
    char cprn[4];

    satno2id(eph->sat,cprn);
    double ep[6];
    time2epoch(eph->toe, ep);
    if (ep[5] > 1e4 || (int)ep[4] % 10 > 1e4)
        return 0;
    printf("get %3s eph\n",cprn);
    if (nav->nmax <= nav->n)
    {
        nav->nmax += 1024;
        if (!(nav_eph = (eph_t *)realloc(nav->eph, sizeof(eph_t) * nav->nmax)))
        {
            trace(1, "decode_eph malloc error: n=%d\n", nav->nmax);
            free(nav->eph);
            nav->eph = NULL;
            nav->n = nav->nmax = 0;
            return 0;
        }
        nav->eph = nav_eph;
    }
    ieph = checkeph(nav, eph);
    if (!ieph)
        nav->eph[nav->n++] = *eph;
    else
        nav->eph[ieph] = *eph;
    return 1;
}
void KPL_inputEph(nav_t *nav, int psat, int offset)
{
    gtime_t td;
    eph_t *a = nav->eph + psat - 1 + offset;
    add_eph(&g_sdk_nav, nav->eph + psat - 1 + offset);
    td = (nav->eph + psat - 1 + offset)->toe;
    // outputeph(nav->eph + psat - 1 + offset, getFileptr(td, NULL, NULL, "eph"));
}

int IO_inputEphData(uint8_t data)
{
    int ret = 0;
    if (0 == (ret = input_rtcm3(&s_rtcm_eph, data)))
        return 0;
    switch (ret)
    {
    case 2:
        /* navigation ephemeris */
        KPL_inputEph(&s_rtcm_eph.nav, s_rtcm_eph.ephsat, s_rtcm_eph.ephset ? MAXSAT : 0);
        break;
    default:
        /* just return */
        break;
    }
    return ret;
}
static int kx_decodestreph(char * buff,int len,int isys)
{
    gtime_t tnow=timeadd(timeget(), 0);
    eph_t teph={0};
    int i;
    char dbuff[19];
    double data[39]={0};
    char cprn[4];
    for(i=0;i<len;i++)
    {   if(buff[i]=='e')
        continue;
        if(isalpha(buff[i]))
            buff[i]='0';
    }
    i=0;
    while(i<39)
    {
        if(sscanf(buff+i*19,"%19s",dbuff))
        {
            sscanf(dbuff,"%lf",data+i);
        }   
        i++;
    }
    if(!data[0]||data[2])
        return 0;
    if((int)data[7]%600!=0)
        return 0;
    
    // teph.flag=
    sprintf(cprn,"%c%02d",SYS[isys],(int)data[1]);// teph.svid
    teph.sat=satid2no(cprn);
    // teph.health=
    // teph.eph_type=
    teph.iodc=teph.iode=(int)data[4];
    // teph.top=
    teph.week = (int)data[6];
    if (SYS[isys] == 'C')
    {

        teph.toc = bdt2gpst(bdt2time(teph.week, data[7]));
        teph.toe = bdt2gpst(bdt2time(teph.week, data[8]));
        teph.toes=time2gpst(teph.toe,&teph.week);
    }
    else
    {
        teph.toc = (gpst2time(teph.week, data[7]));
        teph.toe = (gpst2time(teph.week, data[8]));
        teph.toes=data[7];
    }
    double ep[6];
    time2epoch(teph.toe,ep);
    if(fabs(timediff(tnow,teph.toe))>3600)
        return 0;
    teph.f0 = data[9];
    teph.f1 = data[10];
    teph.f2 = data[11];
    teph.A=data[12]*data[12];
    teph.e=data[13];
    teph.omg=data[14];
    teph.deln=data[15];
    teph.M0=data[16];
    teph.OMG0=data[17];
    teph.OMGd=data[18];
    teph.i0=data[19];
    teph.idot=data[20];
    teph.cuc=data[21];
    teph.cus=data[22];
    teph.crc=data[23];
    teph.crs=data[24];
    teph.cic=data[25];
    teph.cis=data[26];
    teph.dA=data[27];
    teph.Adot=data[28];
    teph.dn0=data[29];
    teph.ndot=data[30];
    teph.A=data[31];
    // teph.n=
    // teph.root_ecc=
    // teph.omega_t=
    // teph.omega_delta=
    // teph.Ek=
    teph.tgd[0]=data[37];
    teph.tgd[1]=data[38];
    teph.type=1;
    s_rtcm_eph.nav.eph[teph.sat-1]=teph;

}
extern int IO_inputkx_strEph( char data)
{
    uint16_t tmp, i, j;
    static uint8_t isys=-1;
    char ctmp[2]={data,'\0'};
    if (s_rtcm_eph.len == 0)
    {
        static char preamb[7] = {};
        static uint8_t flag=0;
        if(data=='N')
            flag=1;
        if(!flag)
            return 0;
        if(strlen(preamb)<7)
            {strcat(preamb,ctmp);
            return 0;}
        if(strstr(preamb,"NAV"))
        {
            s_rtcm_eph.len=7;
        }
        if(strstr(preamb,"BDS"))
            isys=index_string(SYS,'C');
        if(strstr(preamb,"GPS"))
            isys=index_string(SYS,'G');
            
        memset(preamb,0,sizeof(preamb));
        flag=0;
        return 0;
    }
    s_rtcm_eph.buff[s_rtcm_eph.len-7]=data;
    s_rtcm_eph.len++;

    if(s_rtcm_eph.len==7+19*38)
        {
            kx_decodestreph(s_rtcm_eph.buff,s_rtcm_eph.len-7,isys);
            s_rtcm_eph.len=0;
            memset(s_rtcm_eph.buff,0,sizeof(s_rtcm_eph.buff));
            return 1;
        }
    return 0;

}

extern void *pthread_brd(void *x)
{
    const char *c_path =  "qlz:qulizhong123@58.49.58.149:2101/RTCM3EPH-MGEX-GPS";
    // const char *c_path =  ":123@119.96.242.26:15026/EPHB_RTCM33";
    stream_t m_unsyncConn;
    strinit(&m_unsyncConn);
    if (!stropen(&m_unsyncConn, STR_NTRIPCLI, STR_MODE_RW, c_path))
    {
        printf("***ERROR(v_openRnx):can't reach the observation %s", c_path);
        exit(1);
    }
    init_rtcm(&s_rtcm_eph);
    strsettimeout(&m_unsyncConn, 60000, 10000); /// 60s for timeout 10s for reconnect
    unsigned char buff[1024] = {0};
    while (1)
    {
        int nread = strread(&m_unsyncConn, buff, 1024);
        for (int i = 0; i < nread; ++i)
        {
            // IO_inputkx_strEph(buff[i]);
            IO_inputEphData(buff[i]);
        }
        if (nread == 0)
            sleepms(10);
    }
}

