#include "ssr_.h"
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#ifndef WIN32
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <math.h>
#endif

static uint32_t table_crc24q[] = {
    0x000000, 0x864CFB, 0x8AD50D, 0x0C99F6, 0x93E6E1, 0x15AA1A, 0x1933EC, 0x9F7F17,
    0xA18139, 0x27CDC2, 0x2B5434, 0xAD18CF, 0x3267D8, 0xB42B23, 0xB8B2D5, 0x3EFE2E,
    0xC54E89, 0x430272, 0x4F9B84, 0xC9D77F, 0x56A868, 0xD0E493, 0xDC7D65, 0x5A319E,
    0x64CFB0, 0xE2834B, 0xEE1ABD, 0x685646, 0xF72951, 0x7165AA, 0x7DFC5C, 0xFBB0A7,
    0x0CD1E9, 0x8A9D12, 0x8604E4, 0x00481F, 0x9F3708, 0x197BF3, 0x15E205, 0x93AEFE,
    0xAD50D0, 0x2B1C2B, 0x2785DD, 0xA1C926, 0x3EB631, 0xB8FACA, 0xB4633C, 0x322FC7,
    0xC99F60, 0x4FD39B, 0x434A6D, 0xC50696, 0x5A7981, 0xDC357A, 0xD0AC8C, 0x56E077,
    0x681E59, 0xEE52A2, 0xE2CB54, 0x6487AF, 0xFBF8B8, 0x7DB443, 0x712DB5, 0xF7614E,
    0x19A3D2, 0x9FEF29, 0x9376DF, 0x153A24, 0x8A4533, 0x0C09C8, 0x00903E, 0x86DCC5,
    0xB822EB, 0x3E6E10, 0x32F7E6, 0xB4BB1D, 0x2BC40A, 0xAD88F1, 0xA11107, 0x275DFC,
    0xDCED5B, 0x5AA1A0, 0x563856, 0xD074AD, 0x4F0BBA, 0xC94741, 0xC5DEB7, 0x43924C,
    0x7D6C62, 0xFB2099, 0xF7B96F, 0x71F594, 0xEE8A83, 0x68C678, 0x645F8E, 0xE21375,
    0x15723B, 0x933EC0, 0x9FA736, 0x19EBCD, 0x8694DA, 0x00D821, 0x0C41D7, 0x8A0D2C,
    0xB4F302, 0x32BFF9, 0x3E260F, 0xB86AF4, 0x2715E3, 0xA15918, 0xADC0EE, 0x2B8C15,
    0xD03CB2, 0x567049, 0x5AE9BF, 0xDCA544, 0x43DA53, 0xC596A8, 0xC90F5E, 0x4F43A5,
    0x71BD8B, 0xF7F170, 0xFB6886, 0x7D247D, 0xE25B6A, 0x641791, 0x688E67, 0xEEC29C,
    0x3347A4, 0xB50B5F, 0xB992A9, 0x3FDE52, 0xA0A145, 0x26EDBE, 0x2A7448, 0xAC38B3,
    0x92C69D, 0x148A66, 0x181390, 0x9E5F6B, 0x01207C, 0x876C87, 0x8BF571, 0x0DB98A,
    0xF6092D, 0x7045D6, 0x7CDC20, 0xFA90DB, 0x65EFCC, 0xE3A337, 0xEF3AC1, 0x69763A,
    0x578814, 0xD1C4EF, 0xDD5D19, 0x5B11E2, 0xC46EF5, 0x42220E, 0x4EBBF8, 0xC8F703,
    0x3F964D, 0xB9DAB6, 0xB54340, 0x330FBB, 0xAC70AC, 0x2A3C57, 0x26A5A1, 0xA0E95A,
    0x9E1774, 0x185B8F, 0x14C279, 0x928E82, 0x0DF195, 0x8BBD6E, 0x872498, 0x016863,
    0xFAD8C4, 0x7C943F, 0x700DC9, 0xF64132, 0x693E25, 0xEF72DE, 0xE3EB28, 0x65A7D3,
    0x5B59FD, 0xDD1506, 0xD18CF0, 0x57C00B, 0xC8BF1C, 0x4EF3E7, 0x426A11, 0xC426EA,
    0x2AE476, 0xACA88D, 0xA0317B, 0x267D80, 0xB90297, 0x3F4E6C, 0x33D79A, 0xB59B61,
    0x8B654F, 0x0D29B4, 0x01B042, 0x87FCB9, 0x1883AE, 0x9ECF55, 0x9256A3, 0x141A58,
    0xEFAAFF, 0x69E604, 0x657FF2, 0xE33309, 0x7C4C1E, 0xFA00E5, 0xF69913, 0x70D5E8,
    0x4E2BC6, 0xC8673D, 0xC4FECB, 0x42B230, 0xDDCD27, 0x5B81DC, 0x57182A, 0xD154D1,
    0x26359F, 0xA07964, 0xACE092, 0x2AAC69, 0xB5D37E, 0x339F85, 0x3F0673, 0xB94A88,
    0x87B4A6, 0x01F85D, 0x0D61AB, 0x8B2D50, 0x145247, 0x921EBC, 0x9E874A, 0x18CBB1,
    0xE37B16, 0x6537ED, 0x69AE1B, 0xEFE2E0, 0x709DF7, 0xF6D10C, 0xFA48FA, 0x7C0401,
    0x42FA2F, 0xC4B6D4, 0xC82F22, 0x4E63D9, 0xD11CCE, 0x575035, 0x5BC9C3, 0xDD8538};

static uint32_t crc24_pppB2b(uint8_t *data, uint16_t bit_len)
{
    uint32_t crc = 0;
    int n = bit_len / 8;
    int r = bit_len % 8;
    uint8_t t_byte;
    for (int i = 0; i < n; i++)
    {
        t_byte = data[i];
        crc = ((crc << 8) & 0xFFFFFF) ^ table_crc24q[(crc >> 16) ^ t_byte];
    }
    if (r)
    {
        t_byte = data[n];
        crc = ((crc << r) & 0xFFFFFF) ^ table_crc24q[(crc >> (24 - r)) ^ (t_byte >> (8 - r))];
    }
    return crc;
}

uint32_t crc_bit462(unsigned char *crcdata)
{
    uint8_t buff[58] = {0};
    memcpy(buff, crcdata, 58);
    return crc24_pppB2b(buff, 462);
}

uint64_t b_offset_bits(ssrctx_t *sc, int *i_beg, int len)
{
    uint64_t rvl = get_bits(sc->buff, *i_beg, len);
    *i_beg += len;
    return rvl;
}

static uint32_t crc_bit462_check(unsigned char *crcdata)
{
    uint32_t crc = get_bits(crcdata, 462, 24);
    return !(crc_bit462(crcdata) ^ (crc));
}

static uint32_t lshift_synchead(uint32_t preamb, unsigned char data)
{
    preamb <<= 8;
    preamb += data;
    return preamb;
}

static void shit2low_kx(uint8_t *preamb, int size, unsigned char data)
{
    for (int i = 0; i < size - 1; i++)
    {
        preamb[i] = preamb[i + 1];
    }
    preamb[size - 1] = data;
    return;
}

static int slot2isys(int index)
{
    if (index < 63 && index > -1)
        return index_string(B2B_SYS, 'C');
    else if (index < 100)
        return index_string(B2B_SYS, 'G');
    else if (index < 137)
        return index_string(B2B_SYS, 'E');
    else if (index < 174)
        return index_string(B2B_SYS, 'R');
    return -1;
}

static void slot2prn(int index, char *cprn)
{
    if (index < 63)
        sprintf(cprn, "C%02d", index + 1);
    else if (index < 100)
        sprintf(cprn, "G%02d", index - 63 + 1);
    else if (index < 137)
        sprintf(cprn, "E%02d", index - 100 + 1);
    else if (index < 174)
        sprintf(cprn, "R%02d", index - 137 + 1);
    return;
}

static const char *tracking_modes[NUM_SYSTEMS][NUM_MODES] = {
    {"2I", "1S", "1L", "XX", "5D", "5P", "XX", "7I", "7Q", "XX", "XX", "XX", "6I", "XX", "XX", "XX"},
    {"1C", "1P", "XX", "XX", "1L", "1X", "XX", "2L", "2X", "XX", "XX", "5I", "5Q", "5X", "XX", "XX"},
    {"1C", "1P", "2C", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX"},
    {"XX", "1B", "XX", "XX", "5Q", "5I", "XX", "7I", "7Q", "XX", "XX", "6C", "XX", "XX", "XX", "XX"}};

static gtime_t adjdoy(gtime_t t_now, double sod)
{
    double ep[6] = {0};
    /* if no time, get cpu time */
    if (t_now.time == 0)
        t_now = utc2gpst(timeget());
    /* current time in PC */
    time2epoch(t_now, ep);
    ep[2] = 27;
    double sod_n = ep[3] * 3600.0 + ep[4] * 60.0 + ep[5];

    /* current receive */
    ep[3] = floor(sod / 3600);
    ep[4] = floor((sod - ep[3] * 3600.0) / 60.0);
    ep[5] = fmod(sod, 60.0);
    gtime_t t_r = epoch2time(ep);
    /* compare the two time */
    if (sod_n - sod > 43200.0)
    {
        /* if PC >> SSR, ex. sod_n is 83900, sod is 0.05 s on the next day   */
        t_r = timeadd(t_r, 86400.0);
    }
    else if (sod_n - sod < -43200)
    {
        /* if PC << SSR, ex, sod_n is 0.05, sod is 83900 s on the previous day  */
        t_r = timeadd(t_r, -86400);
    }
    return t_r;
}

// // output check
// int outeph(FILE *ssrfile, ssrctx_t *ssrc)
// {
//     char epoch_header[1024] = {'\0'}, msg_tmp[1024] = {'\0'}, cprn[4] = {'\0'};
//     int iprn, nc = 0;
//     double ep[6];
//     char *msg = (char *)malloc(sizeof(char) * 1024 * nprn_);
//     memset(msg, 0, sizeof(char) * 1024 * nprn_);

//     for (iprn = 0; iprn < nprn_; iprn++)
//     {
//         if ((ssrc->ssr_epoch[iprn].deph[1] + ssrc->ssr_epoch[iprn].deph[1] + ssrc->ssr_epoch[iprn].deph[2]) == 0)
//             continue;
//         satno2id(iprn, cprn);
//         // if (strstr(cprn, "G04"))continue;
//         sprintf(msg_tmp, "%-4s%11d%11.4f%11.4f%11.4f%11.4f%11.4f%11.4f\n", cprn, ssrc->ssr_epoch[iprn].iode,
//                 ssrc->ssr_epoch[iprn].deph[0], ssrc->ssr_epoch[iprn].deph[1], ssrc->ssr_epoch[iprn].deph[2], 0.0,
//                 0.0, 0.0);
//         strcat(msg, msg_tmp);
//         msg_tmp[0] = '\0';
//         nc++;
//         time2epoch(ssrc->ssr_epoch[iprn].t0[0], ep);
//     }

//     sprintf(epoch_header, "> ORBIT %4d %02d %02d %02d %02d %4.1f %1d %2d CLK01\n",
//             (int)ep[0], (int)ep[1], (int)ep[2], (int)ep[3], (int)ep[4], ep[5], ssrc->ssr_epoch[iprn].iod[0], nc);
//     fprintf(ssrfile, "%s%s", epoch_header, msg);
//     return 1;
// };
// int outclk(FILE *ssrfile, ssrctx_t *ssrc)
// {
//     char epoch_header[1024] = {'\0'}, msg_tmp[1024] = {'\0'}, cprn[4] = {'\0'};
//     int iprn, nc = 0;
//     double ep[6];
//     char *msg = (char *)malloc(sizeof(char) * 1024 * nprn_);
//     memset(msg, 0, sizeof(char) * 1024 * nprn_);

//     for (iprn = 0; iprn < nprn_; iprn++)
//     {

//         if ((ssrc->ssr_epoch[iprn].dclk[0]) == 0)
//             continue;
//         satno2id(iprn, cprn);
//         sprintf(msg_tmp, "%-4s%11d%11.4f%11.4f%11.4f\n", cprn, ssrc->ssr_epoch[iprn].iode,
//                 ssrc->ssr_epoch[iprn].dclk[0],
//                 0.0, 0.0);
//         strcat(msg, msg_tmp);
//         msg_tmp[0] = '\0';
//         time2epoch(ssrc->ssr_epoch[iprn].t0[1], ep);
//         nc++;
//     }
//     sprintf(epoch_header, "> CLOCK %4d %02d %02d %02d %02d %4.1f %1d %d CLK01\n",
//             (int)ep[0], (int)ep[1], (int)ep[2], (int)ep[3], (int)ep[4], ep[5], ssrc->ssr_epoch[iprn].iod[1], nc);
//     fprintf(ssrfile, "%s%s", epoch_header, msg);
//     return 1;
// }
// int outcodesbias(FILE *biasfile, ssrctx_t *ssrc)
// {
//     char cprn[4] = {'\0'};
//     int isys, iprn, code;
//     char ctmp[1024] = {'\0'};
//     double ep[6];
//     size_t size;
//     char biascode[NUM_SYSTEMS][16][12] = {{"L1 C/A", "L1 P", "", "", "L1C(P)", "L1C(D+P)", "", "L2C(L)", "L2C(M+L)", "", "", "L5I", "L5Q", "L5I+Q", "", ""},
//                                           {"", "E1 B", "E1 C", "", "E5a Q", "E5a I", "", "E5b I", "E5b Q", "", "", "E6 C", "", "", "", ""},
//                                           {"B1I", "B1C(d)", "B1C(p)", "-", "B2a(D)", "B2a(P)", "-", "B2b-I", "B2b-I", "-", "", "-", "B3I", "-", "-", "-"},
//                                           {"B1I", "B1C(d)", "B1C(p)", "-", "B2a(D)", "B2a(P)", "-", "B2b-I", "B2b-I", "-", "", "-", "B3I", "-", "-", "-"}};
//     for (isys = 0; isys < NUM_SYSTEMS; isys++)
//     {
//         fprintf(biasfile, "%12s", " ");
//         for (int icode = 0; icode < 16; icode++)
//         {
//             fprintf(biasfile, "%12s", biascode[isys][icode]);
//         }
//         fprintf(biasfile, "\n");
//     }

//     for (int iprn = 0; iprn < nprn_; iprn++)
//     {
//         satno2id(iprn, cprn);
//         isys = index_sys(iprn + 1);
//         fprintf(biasfile, "%12s", cprn);
//         for (int icode = 0; icode < 16; icode++)
//         {
//             code = obs2code(tracking_modes[isys][icode]);
//             if (code != CODE_NONE && ssrc->ssr_epoch[iprn].cbias[code - 1] != 0)
//                 fprintf(biasfile, "%12.3f", ssrc->ssr_epoch[iprn].cbias[code - 1]);
//             else
//                 fprintf(biasfile, "%12s", "--");
//         }
//         fprintf(biasfile, "\n");
//     }
// }
static int mask2qzss(uint16_t i)
{
    if (i < 40)
        return i + 120;
    if (i < 63)
        return i + 200;
    if (i < 100)
        return i - 63;
    if (i < 137)
        return i - 20;
    if (i < 174)
        return i - 97;
    return -1;
}

static void decode_b2bssr_type1(ssrctx_t *sc)
{
    gtime_t t_now = timeget();
    uint32_t bdt = get_bits(sc->buff, 6, 17);
    sc->BDT = adjdoy(t_now, bdt);
    sc->IODSSR = get_bits(sc->buff, 27, 2);
    sc->IODP = get_bits(sc->buff, 29, 4);
    sc->n_sat = 0;
    memset(sc->mask, 0, sizeof(sc->mask));
    for (int i = 0; i < 255; i++)
    {
        sc->maskb2b[i] = get_bits(sc->buff, 33 + i, 1);
        int a = mask2qzss(i);
        if (mask2qzss(i) > -1)
            sc->mask[mask2qzss(i)] = sc->maskb2b[i];
        sc->n_sat += sc->maskb2b[i] ? 1 : 0;
    }
    sc->lvalid = 1;
}

static int decode_b2bssr_type2(ssrctx_t *sc)
{
    int offset = 6;
    uint32_t bdt = b_offset_bits(sc, &offset, 17);
    if (!sc->lvalid)
    {
        printf("warning: no valid mask  decoding clk corr at sod(bdt) %d\n", bdt);
        return 0;
    }
    offset += 4;
    uint8_t iodssr = b_offset_bits(sc, &offset, 2);
    if (iodssr != sc->IODSSR)
        return 0;
    for (int i = 0; i < 6; i++)
    {
        uint16_t satslot = b_offset_bits(sc, &offset, 9);
        if (!sc->maskb2b[satslot - 1])
            return 0;
        int isat;
        if ((isat = mask2qzss(satslot - 1)) < 0)
            return 0;
        sc->ssr_epoch[isat].iode = b_offset_bits(sc, &offset, 10);
        sc->ssr_epoch[isat].iodcrc = b_offset_bits(sc, &offset, 3) + 1;
        uint16_t radialCorr = b_offset_bits(sc, &offset, 15);
        uint16_t tangentialCorr = b_offset_bits(sc, &offset, 13);
        uint16_t normalCorr = b_offset_bits(sc, &offset, 13);
        uint8_t ura = b_offset_bits(sc, &offset, 6);
        if (radialCorr + tangentialCorr + normalCorr == 0)
            continue;
        sc->ssr_epoch[isat].deph[0] = uint64_to_int64(radialCorr, 15) * 0.0016;
        sc->ssr_epoch[isat].deph[1] = uint64_to_int64(tangentialCorr, 13) * 0.0064;
        sc->ssr_epoch[isat].deph[2] = uint64_to_int64(normalCorr, 13) * 0.0064;
        sc->ssr_epoch[isat].iod[0] = iodssr;
        sc->ssr_epoch[isat].t0[0] = bdt2gpst(adjdoy(sc->BDT, bdt));
        sc->ssr_epoch[isat].ura = ura2dist(ura);
    }
}

static int decode_b2bssr_type3(ssrctx_t *sc)
{
    unsigned char *encoded_data = sc->buff;
    int offset = 6;
    int i, isat;
    uint8_t n_cbias;
    uint32_t bdt = b_offset_bits(sc, &offset, 17);
    if (!sc->lvalid)
    {
        printf("warning: no valid mask  decoding codebias at sod(bdt) %d\n", bdt);
        return 0;
    }
    offset += 4;
    uint8_t iodssr = b_offset_bits(sc, &offset, 2);
    uint8_t num_sat = b_offset_bits(sc, &offset, 5);
    if (iodssr != sc->IODSSR)
        return 0;
    for (i = 0; i < num_sat; ++i)
    {
        int satslot = b_offset_bits(sc, &offset, 9);
        n_cbias = b_offset_bits(sc, &offset, 4);
        int isys = slot2isys(satslot - 1);
        if ((isat = mask2qzss(satslot - 1)) < 0)
            return 0;
        sc->ssr_epoch[isat].t0[4] = bdt2gpst(adjdoy(sc->BDT, bdt));
        for (int k = 0; k < n_cbias && isys != -1; k++)
        {
            int i_c_type = b_offset_bits(sc, &offset, 4);
            uint16_t codebias_v = b_offset_bits(sc, &offset, 12);
            int code = obs2code3(tracking_modes[isys][i_c_type]);
            if (code == CODE_NONE)
                continue;
            sc->ssr_epoch[isat].iod[4] = iodssr;
            sc->ssr_epoch[isat].cbias[code - 1] = uint64_to_int64(codebias_v, 12) * 0.017;
            sc->ssr_epoch[isat].f_cbias[code - 1] = 1;
        }
    }
    return 3;
}

static int decode_b2bssr_type4(ssrctx_t *sc)
{
    unsigned char *encoded_data = sc->buff;
    int offset = 6;
    uint32_t bdt = b_offset_bits(sc, &offset, 17);
    if (!sc->lvalid)
    {
        printf("warning: no valid mask  decoding orb corr at sod(bdt) %d\n", bdt);
        return 0;
    }
    offset += 4;
    uint8_t iodssr = b_offset_bits(sc, &offset, 2);
    uint8_t iodp = b_offset_bits(sc, &offset, 4);
    uint8_t subtype = b_offset_bits(sc, &offset, 5);
    if (iodssr != sc->IODSSR || iodp != sc->IODP)
        return 0;
    int n_sat = -1;
    for (int i = 0; i < 255; ++i)
    {
        if (!sc->maskb2b[i])
            continue;
        n_sat++;
        if (n_sat < subtype * 23)
            continue;
        if (n_sat >= subtype * 23 + 23)
            break;
        int isat = mask2qzss(i);
        uint8_t iodcorr = b_offset_bits(sc, &offset, 3) + 1;
        double clk = uint64_to_int64(b_offset_bits(sc, &offset, 3), 15) * 0.0016;
        if (!sc->ssr_epoch[isat].iodcrc)
            sc->ssr_epoch[isat].iodcrc = iodcorr;
        if (iodcorr != sc->ssr_epoch[isat].iodcrc)
            continue;
        sc->ssr_epoch[isat].dclk[0] = clk;
        sc->ssr_epoch[isat].t0[1] = bdt2gpst(adjdoy(sc->BDT, bdt));
    }
}
static ssrctx_t *get_ssr_ctx_prn(ssrctx_t *sc)
{
    static ssrctx_t *ssr_ctxs[MAXSSRSAT] = {0};
    if (!ssr_ctxs[sc->prn])
    {
        ssr_ctxs[sc->prn] = calloc(1, sizeof(ssrctx_t));
        ssr_ctxs[sc->prn]->prn = sc->prn;
    }
    ssrctx_t *rvl = ssr_ctxs[sc->prn];
    memcpy(rvl->buff, sc->buff, 70);
    return rvl;
}

int decode_b2b(ssrctx_t *user_ssr_ctx, int prn)
{
    // if(prn!=60)return 0;
    user_ssr_ctx->prn = prn;
    if (!crc_bit462_check(user_ssr_ctx->buff))
    {
        printf("error: msg of  C%02d CRC failed!\n", prn);
        return 0;
    }
    ssrctx_t *sc = get_ssr_ctx_prn(user_ssr_ctx);
    int type = get_bits(user_ssr_ctx->buff, 0, 6);
    switch (type)
    {
    case 1:
        decode_b2bssr_type1(sc);
        break;
    case 2:
        decode_b2bssr_type2(sc);
        break;
    case 3:
        decode_b2bssr_type3(sc);
        break;
    case 4:
        decode_b2bssr_type4(sc);
        break;
    default:
        break;
    }
    memcpy(user_ssr_ctx, sc, sizeof(ssrctx_t));
    return type;
}

// kx post hex data process
extern int inputb2bbyte_kx(ssrctx_t *sc)
{
    return 0;
}

void u32tbyte_kx(uint32_t data, uint8_t *buff, int idx)
{
    for (int i = 0; i < 4; i++)
        (buff[i + idx] = data >> ((3 - i % 4) * 8)) & 0xFF;
}

extern int input_b2bssr(ssrctx_t *sc, unsigned char data)
{
    uint16_t tmp, i, j;
    if (sc->nbyte == 0)
    {
        static uint16_t preamb = 0x0;
        preamb = lshift_synchead(preamb, data);
        if (preamb != B2bPREAMB)
            return 0;
        // *((uint32_t *)ssrContext->buff) = B2bPREAMB;
        sc->nbyte = 2; /* 固定长度 */
        return 0;
    }
    // TBD unknown format
    // TBD LDPC? 1000btis/ 512bit?
    sc->buff[sc->nbyte++] = data;
    if (sc->nbyte == 60) // 486/8
    {
        decode_b2b(sc, 1);
        sc->nbyte = 0;
        memset(sc->buff, 0, SSRCTX_BUFFLEN);
        return 1;
    }
    return 0;
}
