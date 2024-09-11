#include "ssr_.h"
#include <stdio.h>
#include <math.h>

static const char *tracking_modes[NUM_SYSTEMS][NUM_MODES] = {
    {"2I", "5P", "5X", "6I", "7I", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX"},
    {"1C", "1W", "2L", "2S", "2W", "2X", "5Q", "5X", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX"},
    {"1I", "1C", "1W", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX"},
    {"1C", "1X", "5Q", "5X", "XX", "XX", "7Q", "7X", "XX", "XX", "XX", "XX", "XX", "XX", "XX", "XX"},
};
static const char *ptracking_modes[NUM_SYSTEMS][8] = {
    {"2I", "5P", "6I", "7I", "XX", "XX", "XX", "XX"},
    {"1C", "2W", "5X", "XX", "XX", "XX", "XX", "XX"},
    {"1I", "2W", "XX", "XX", "XX", "XX", "XX", "XX"},
    {"1X", "2X", "5Q", "7Q", "XX", "XX", "XX", "XX"},
};

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

uint64_t k_offset_bits(ssrctx_t *sc, int *i_beg, int len)
{
    uint64_t rvl = get_bits(sc->buff, *i_beg, len);
    *i_beg += len;
    return rvl;
}
static char *slot2prn(int index, char *cprn)
{
    if (index < 63)
        sprintf(cprn, "C%02d", index + 1);
    else if (index < 100)
        sprintf(cprn, "G%02d", index - 63 + 1);
    else if (index < 137)
        sprintf(cprn, "E%02d", index - 100 + 1);
    else if (index < 174)
        sprintf(cprn, "R%02d", index - 137 + 1);
    return cprn;
}
static int slot2offset(int index)
{
    if (index < 63 && index > -1)
        return 16 * 0;
    else if (index < 100)
        return 16 * 1;
    else if (index < 137)
        return 16 * 3;
    else if (index < 174)
        return 16 * 2;
    return -16;
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
/* adjust weekly rollover of GPS time ----------------------------------------*/
static gtime_t2 adjdoy(gtime_t2 t_now, double sod)
{
    double ep[6] = {0};
    /* if no time, get cpu time */
    if (t_now.time == 0)
        t_now = utc2gpst(timeget());
    /* current time in PC */
    time2epoch(t_now, ep);
    double sod_n = ep[3] * 3600.0 + ep[4] * 60.0 + ep[5];

    /* current receive */
    ep[3] = floor(sod / 3600);
    ep[4] = floor((sod - ep[3] * 3600.0) / 60.0);
    ep[5] = fmod(sod, 60.0);
    gtime_t2 t_r = epoch2time(ep);
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

static uint32_t crc_bit462(unsigned char *crcdata)
{
    uint8_t buff[KPLSSR_STRUCT_SIZE - 3] = {0};
    memcpy(buff, crcdata, KPLSSR_STRUCT_SIZE - 4);
    buff[KPLSSR_STRUCT_SIZE - 4] = crcdata[KPLSSR_STRUCT_SIZE - 4] >> 4 << 4;
    return crc24_pppB2b(buff, KPLSSR_STRUCT_SIZE - 3);
}

static uint32_t crc_bit462_check(unsigned char *crcdata)
{
    uint32_t crc = get_bits(crcdata, 476, 24);
    return !(crc_bit462(crcdata) ^ (crc << 8));
}

static void decoding_type1(ssrctx_t *sc, int bit_begin)
{
    gtime_t2 t_now = timeget();
    bit_begin += 6;
    uint32_t bdt = k_offset_bits(sc, &bit_begin, 17);
    bit_begin += 4;
    sc->IODSSR = k_offset_bits(sc, &bit_begin, 2);
    sc->IODP = k_offset_bits(sc, &bit_begin, 4);
    sc->BDT = adjdoy(t_now, bdt);
    memset(sc->mask, 0, sizeof(sc->mask));
    for (int i = 0; i < 255; i++)
    {
        sc->maskb2b[i] = k_offset_bits(sc, &bit_begin, 1);
        sc->n_sat += sc->maskb2b[i] ? 1 : 0;
        if (mask2qzss(i)> -1)
            sc->mask[mask2qzss(i)] = sc->maskb2b[i];
    }
    sc->lvalid = 1;
}


static int decoding_type3(ssrctx_t *sc, int bit_begin)
{
    unsigned char *encoded_data = sc->buff;
    int satn = sc->n_sat, i, j;
    if (!sc->lvalid)
    {
        printf("error: sc not valid!\n");
        return 0;
    }
    uint32_t bdt = get_bits(encoded_data, bit_begin + 6, 17);
    uint8_t iodssr = get_bits(encoded_data, bit_begin + 27, 2);
    uint8_t num_sat = get_bits(encoded_data, bit_begin + 29, 5);
    uint16_t satslot = get_bits(encoded_data, bit_begin + 34, 9);
    if (iodssr != sc->IODSSR)
    {
        printf("error: IODSSR");
        return 0;
    }
    bit_begin += 34 + 9;
    for (i = 0, j = satslot - 1; i < num_sat; ++i, ++j)
    {
        for (; j < 255; ++j)
        {
            if (sc->maskb2b[j])
                break;
        }
        int isys = slot2isys(j);
        uint8_t n_cbias = k_offset_bits(sc, &bit_begin, 4) + 1;
        for (int k = 0; k < n_cbias && isys != -1; k++)
        {
            char cprn[8] = {0};
            slot2prn(j, cprn);
            int i_c_type = k_offset_bits(sc, &bit_begin, 4);
            int code = obs2code3(tracking_modes[isys][i_c_type]);
            uint16_t codebias_v = k_offset_bits(sc, &bit_begin, 12);
            // printf("saving code bias for %s %d %d %s\n", cprn, j, i_c_type, code2obs(code));
            if (!code)
                continue;
            sc->ssr_epoch[mask2qzss(j)].cbias[code - 1] = uint64_to_int64(codebias_v, 12) * 0.017;
            sc->ssr_epoch[mask2qzss(j)].f_cbias[code - 1] = 1;
            sc->ssr_epoch[mask2qzss(j)].t0[4] = adjdoy(sc->BDT, bdt);
        }
    }
    return num_sat;
}

static int decoding_type8(ssrctx_t *sc, int bit_begin)
{
    // 6 17 2 12 11 12 11 7 7 5 18 3 18 3 ...
    unsigned char *encoded_data = sc->buff;
    int satn = sc->n_sat;
    if (!sc->lvalid)
    {
        printf("error: sc not valid!\n");
        return 0;
    }
    uint32_t bdt = get_bits(encoded_data, bit_begin + 6, 17);
    gtime_t2 t0 = adjdoy(timeget(), bdt);
    int n_area = get_bits(encoded_data, bit_begin + 23, 2);
    bit_begin += 25;
    for (int i = 0; i < n_area; ++i)
    {
        int j = get_bits(encoded_data, bit_begin + 53, 7);
        ssr_network_t *sn = get_ssr_network(sc, j);
        sn->cnid = j;
        sn->t0 = t0;
        sn->rect[0] = uint64_to_int64(get_bits(encoded_data, bit_begin + 0, 12), 12) / 10.;
        sn->rect[1] = uint64_to_int64(get_bits(encoded_data, bit_begin + 12, 11), 11) / 10.;
        sn->rect[2]  = uint64_to_int64(get_bits(encoded_data, bit_begin + 23, 12), 12) / 10.;
        sn->rect[3] = uint64_to_int64(get_bits(encoded_data, bit_begin + 35, 11), 11) / 10.;
        sn->sigma = get_bits(encoded_data, bit_begin + 46, 7) / 1000.;
        sn->coeff_num = get_bits(encoded_data, bit_begin + 60, 5);
        for (int k = 0; k < sn->coeff_num; k++)
        {
            int ex = -1 * get_bits(encoded_data, bit_begin + 83 + k * 21, 3);
            // sn->mcoeff[k] = uint64_to_int64(get_bits(encoded_data, bit_begin + 65 + k * 21, 18), 18) / 10000. * pow(10, ex);
        }
        bit_begin += 65 + 21 * sn->coeff_num;
    }
    return n_area;
}

static void decoding_type6(ssrctx_t *sc, int bitbegin)
{
    // MesTypeID
    unsigned char *encoded_data = sc->buff;
    int satn = sc->n_sat, j, i, n_used_bits;
    if (!sc->lvalid)
    {
        printf("error: sc not valid!\n");
        return;
    }
    int NumC = get_bits(encoded_data, bitbegin + 6, 5);
    int NumO = get_bits(encoded_data, bitbegin + 11, 3);
    // printf("TYPE 6: %02d(orbit) %02d(clk)\n", NumO, NumC);
    if (NumC > 0)
    {
        uint32_t bdt = get_bits(encoded_data, bitbegin + 14, 17);
        // printf("----t: %d %d %d\n\n", bdt/3600, bdt%3600/60, bdt%60);
        uint8_t iodssr = get_bits(encoded_data, bitbegin + 35, 2);
        uint8_t iodp = get_bits(encoded_data, bitbegin + 37, 4);
        uint8_t satslot = get_bits(encoded_data, bitbegin + 41, 9);
        n_used_bits = 64;
        if (iodssr != sc->IODSSR)
        {
            printf("error: IODSSR");
            return;
        }
        if (iodp != sc->IODP)
        {
            printf("error: IODP ");
            return;
        }
        for (i = 0, j = satslot - 1; i < NumC; ++i, ++j)
        {
            uint8_t iodcorr = get_bits(encoded_data, n_used_bits, 3);
            uint16_t corr = get_bits(encoded_data, n_used_bits + 3, 15);
            uint8_t n_pbias = get_bits(encoded_data, n_used_bits + 18, 3) + 1;
            n_pbias = n_pbias % 8;
            for (; j < 255; ++j)
            {
                if (sc->maskb2b[j])
                    break;
            }
            int q_satslot = mask2qzss(j);
            sc->ssr_epoch[q_satslot].t0[1] = adjdoy(sc->BDT, bdt);
            sc->ssr_epoch[q_satslot].dclk[0] = uint64_to_int64(corr, 15) * 0.0016;
            sc->ssr_epoch[q_satslot].IODCorr = iodcorr;
            sc->ssr_epoch[q_satslot].iod[1] = iodp;
            sc->ssr_epoch[q_satslot].iod[4] = iodp;
            for (int k = 0; k < n_pbias; k++)
            {
                uint8_t pbias_type = get_bits(encoded_data, n_used_bits + 21 + k * 15, 3);
                uint16_t pbias_v = get_bits(encoded_data, n_used_bits + 24 + k * 15, 12);
                int isys = slot2isys(j);
                int code = obs2code3(ptracking_modes[isys][pbias_type]);
                if (code == 0)
                    continue;
                sc->ssr_epoch[q_satslot].pbias[code - 1] = uint64_to_int64(pbias_v, 12) * 0.017;
                sc->ssr_epoch[q_satslot].f_pbias[code - 1] = 1;
                sc->ssr_epoch[q_satslot].t0[5] = sc->ssr_epoch[q_satslot].t0[1];
            }
            n_used_bits += 21 + n_pbias * 15;
        }
    }

    if (NumO > 0)
    {
        int index_code = bitbegin + 14 + 36 * (NumC > 0 ? 1 : 0) + 18 * NumC;
        uint32_t bdt = get_bits(encoded_data, index_code, 17);
        uint8_t iodssr = get_bits(encoded_data, index_code + 21, 2);

        if (iodssr != sc->IODSSR)
        {
            printf("error: IODSSR");
            return;
        }

        for (int i = 0; i < NumO; i++)
        {
            uint16_t satslot = get_bits(encoded_data, index_code + 69 * i + 23, 9);
            uint16_t IODN = get_bits(encoded_data, index_code + 69 * i + 32, 10);
            uint16_t IODCorr = get_bits(encoded_data, index_code + 69 * i + 42, 3);
            uint16_t radialCorr = get_bits(encoded_data, index_code + 69 * i + 45, 15);
            uint16_t tangentialCorr = get_bits(encoded_data, index_code + 69 * i + 60, 13);
            uint16_t normalCorr = get_bits(encoded_data, index_code + 69 * i + 73, 13);
            uint8_t URAL_CLASS = get_bits(encoded_data, index_code + 69 * i + 86, 3);
            uint8_t URAL_VALUE = get_bits(encoded_data, index_code + 69 * i + 89, 3);

            int q_satslot = mask2qzss(satslot-1);
            sc->ssr_epoch[q_satslot].t0[0] = adjdoy(sc->BDT, bdt);
            sc->ssr_epoch[q_satslot].IODCorr = IODCorr;
            sc->ssr_epoch[q_satslot].iode = IODN;
            sc->ssr_epoch[q_satslot].deph[0] = uint64_to_int64(radialCorr, 15) * 0.0016;
            sc->ssr_epoch[q_satslot].deph[1] = uint64_to_int64(tangentialCorr, 13) * 0.0064;
            sc->ssr_epoch[q_satslot].deph[2] = uint64_to_int64(normalCorr, 13) * 0.0064;
        }
    }
}

int decode_ssr(ssrctx_t *sc)
{
    int bitbegin = 14;
    uint8_t p_symbol = get_bits(sc->buff, 11, 3), b_sync = 1;
    sc->lend = (p_symbol & b_sync) ? 0 : 1;
    uint64_t code_type = get_bits(sc->buff, bitbegin, 6);
    if (!crc_bit462_check(sc->buff))
    {
        printf("error: CRC检验失败。");
        return 0;
    }
    char *tm[100];
    time2str(timeget(), tm, 2);
    printf("%s recive message %d.\n", tm, code_type);
    switch (code_type)
    {
    case 1:
        decoding_type1(sc, bitbegin);
        break;
    case 3:
        decoding_type3(sc, bitbegin);
        break;
    case 6:
        decoding_type6(sc, bitbegin);
        break;
    case 8:
        decoding_type8(sc, bitbegin);
        break;
    default:
        break;
    }
    return sc->lend && sc->lvalid;
}

extern int input_kplssr(ssrctx_t *ssrContext, unsigned char data)
{
    if (ssrContext->nbyte == 0)
    {
        if (data != B2bPREAMB)
            return 0;
        ssrContext->buff[ssrContext->nbyte++] = data;
        return 0;
    }
    ssrContext->buff[ssrContext->nbyte++] = data;
    if (ssrContext->nbyte < KPLSSR_STRUCT_SIZE)
        return 0;
    ssrContext->nbyte = 0;
    return decode_ssr(ssrContext);
}
