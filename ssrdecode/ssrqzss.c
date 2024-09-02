#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "correct.h"
#include "ssr.h"

int code_index[6][16] = {{1, 2, 3, 7, 8, 0, 16, 17, 18, 19, 20, 24, 25, 26, 0, 0},
                         {1, 2, 14, 19, 66, 67, 68, 0, 0, 0, 44, 45, 46, 0, 0, 0},
                         {11, 1, 12, 24, 25, 26, 27, 28, 29, 37, 38, 39, 0, 0, 0, 0},
                         {40, 41, 18, 42, 43, 33, 27, 28, 29, 0, 0, 0, 0, 0, 0, 0},
                         {1, 7, 8, 12, 16, 17, 18, 24, 25, 26, 0, 0, 0, 0, 0, 0},
                         {1, 24, 25, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

typedef struct
{
    float lat;
    float lon;
} LatLon;

typedef struct
{
    const char *name;  // 地区名称
    int grid_count;    // 格点数量
    LatLon points[32]; // 格点位置，假设最多有32个点
} CLASGrid;

CLASGrid clasGrid[] = {
    {"ISHIGAKI", 8, {{24.75, 125.37}, {24.83, 125.17}, {24.64, 124.69}, {24.54, 124.30}, {24.34, 124.17}, {24.06, 123.80}, {24.43, 123.79}, {24.45, 122.94}}},
    {"OKINAWA", 11, {{26.42, 126.87}, {26.15, 127.53}, {26.69, 127.53}, {26.69, 128.18}, {27.23, 128.18}, {27.23, 128.84}, {27.77, 128.51}, {27.77, 129.17}, {28.30, 129.17}, {28.30, 129.83}, {25.83, 131.23}}},
    {"KYUSYU", 32, {{33.16, 129.50}, {33.70, 129.50}, {34.23, 129.50}, {34.77, 129.50}, {34.23, 128.84}, {32.62, 128.84}, {33.16, 128.84}, {31.81, 129.50}, {31.00, 130.49}, {31.00, 131.14}, {30.46, 130.49}, {30.46, 131.14}, {31.54, 130.16}, {32.08, 130.16}, {32.62, 130.16}, {33.16, 130.16}, {33.70, 130.16}, {31.54, 130.82}, {32.08, 130.82}, {32.62, 130.82}, {33.16, 130.82}, {33.70, 130.82}, {31.54, 131.47}, {32.08, 131.47}, {32.62, 131.47}, {33.16, 131.47}, {33.70, 131.47}, {28.84, 128.84}, {28.84, 129.50}, {29.38, 129.50}, {29.92, 129.50}, {29.92, 130.16}}},
    {"SHIKOKU", 15, {{32.62, 132.13}, {33.16, 132.13}, {33.70, 132.13}, {32.62, 132.79}, {33.16, 132.79}, {33.70, 132.79}, {34.23, 132.79}, {33.16, 133.45}, {33.70, 133.45}, {34.23, 133.45}, {33.16, 134.11}, {33.70, 134.11}, {34.23, 134.11}, {33.70, 134.76}, {34.23, 134.76}}},
    {"CHUGOKU", 15, {{34.23, 130.82}, {34.23, 131.47}, {34.77, 131.47}, {34.23, 132.13}, {34.77, 132.13}, {34.77, 132.79}, {35.31, 132.79}, {34.77, 133.45}, {35.31, 133.45}, {35.85, 132.79}, {35.85, 133.45}, {36.39, 133.45}, {34.77, 134.11}, {35.31, 134.11}, {35.85, 134.11}}},
    {"KANSAI", 27, {{34.77, 134.76}, {35.31, 134.76}, {35.85, 134.76}, {33.70, 135.42}, {34.23, 135.42}, {34.77, 135.42}, {35.31, 135.42}, {35.85, 135.42}, {33.70, 136.08}, {34.23, 136.08}, {34.77, 136.08}, {35.31, 136.08}, {35.85, 136.08}, {36.39, 136.08}, {34.23, 136.74}, {34.77, 136.74}, {35.31, 136.74}, {35.85, 136.74}, {36.39, 136.74}, {36.93, 136.74}, {37.47, 136.74}, {34.77, 137.40}, {35.31, 137.40}, {35.85, 137.40}, {36.39, 137.40}, {36.93, 137.40}, {37.47, 137.40}}},
    {"KANTO", 22, {{34.77, 138.05}, {35.31, 138.05}, {35.85, 138.05}, {36.39, 138.05}, {34.77, 138.71}, {35.31, 138.71}, {35.85, 138.71}, {36.39, 138.71}, {34.23, 139.04}, {34.23, 139.70}, {34.77, 139.37}, {35.31, 139.37}, {35.85, 139.37}, {36.39, 139.37}, {34.77, 140.03}, {35.31, 140.03}, {35.85, 140.03}, {36.39, 140.03}, {35.31, 140.69}, {35.85, 140.69}, {36.39, 140.69}, {33.11, 139.79}}},
    {"TOHOKU-SOUTH", 20, {{36.93, 138.05}, {36.93, 138.71}, {37.47, 138.71}, {37.74, 138.05}, {38.28, 138.05}, {38.01, 138.71}, {36.93, 139.37}, {37.47, 139.37}, {38.01, 139.37}, {38.55, 139.37}, {36.93, 140.03}, {37.47, 140.03}, {38.01, 140.03}, {38.55, 140.03}, {36.93, 140.69}, {37.47, 140.69}, {38.01, 140.69}, {38.55, 140.69}, {37.47, 141.34}, {38.55, 141.34}}},
    {"TOHOKU-NORTH", 18, {{39.09, 140.03}, {39.62, 140.03}, {40.16, 140.03}, {40.70, 140.03}, {41.24, 140.03}, {39.09, 140.69}, {39.62, 140.69}, {40.16, 140.69}, {40.70, 140.69}, {41.24, 140.69}, {39.09, 141.34}, {39.62, 141.34}, {40.16, 141.34}, {40.70, 141.34}, {41.24, 141.34}, {39.09, 142.00}, {39.62, 142.00}, {40.16, 142.00}}},
    {"HOKKAIDO-WEST", 23, {{42.32, 139.37}, {41.78, 140.03}, {42.32, 140.03}, {42.86, 140.03}, {41.78, 140.69}, {42.32, 140.69}, {42.86, 140.69}, {43.40, 140.69}, {41.78, 141.34}, {42.32, 141.34}, {42.86, 141.34}, {43.40, 141.34}, {43.94, 141.34}, {42.32, 142.00}, {42.86, 142.00}, {43.40, 142.00}, {43.94, 142.00}, {42.32, 142.66}, {42.86, 142.66}, {43.40, 142.66}, {41.78, 143.32}, {42.32, 143.32}, {42.86, 143.32}}},
    {"HOKKAIDO-EAST", 19, {{45.28, 141.34}, {44.48, 142.00}, {45.01, 142.00}, {45.55, 142.00}, {43.94, 142.66}, {44.48, 142.66}, {45.01, 142.66}, {43.40, 143.32}, {43.94, 143.32}, {44.48, 143.32}, {42.86, 143.98}, {43.40, 143.98}, {43.94, 143.98}, {42.86, 144.63}, {43.40, 144.63}, {43.94, 144.63}, {43.40, 145.29}, {43.94, 145.29}, {44.48, 145.29}}},
    {"OGASAWARA", 2, {{27.07, 142.20}, {26.64, 142.16}}},
    {"HOKKAIDO-ISLAND", 13, {{43.40, 145.95}, {43.94, 145.95}, {44.48, 145.95}, {43.94, 146.61}, {44.48, 146.61}, {45.01, 146.61}, {44.48, 147.27}, {45.01, 147.27}, {45.01, 147.92}, {45.55, 147.92}, {45.01, 148.58}, {45.55, 148.58}, {45.55, 149.24}}},
    {"ISLAND (TAKESHIMA)", 1, {{37.24, 131.87}}},
    {"ISLAND (KITA-DAITO)", 1, {{25.96, 131.31}}},
    {"ISLAND (UOTSURI)", 1, {{25.73, 123.54}}},
    {"ISLAND (IOU)", 1, {{24.77, 141.34}}},
    {"ISLAND (MINAMI-TORISHIMA)", 1, {{24.28, 153.99}}},
    {"ISLAND (OKINOSHIMA)", 1, {{20.44, 136.09}}}};

void reset_qzss_buff(ssrctx_t *sc)
{
    sc->qbuff_beg = 0;
    sc->qbuff_end = 0;
}

int prn2sys(int prn)
{
    return prn / 40;
}

int is_galileo(int prn)
{
    if (prn >= 80 && prn < 120)
        return 1;
    return 0;
}

void initctx(ssrctx_t *sc)
{
    sc->gnss_hourly_epoch_t = 0;
    sc->last_iodssr = 0;
    sc->cellmask_n = 0;
    memset(sc->mask_sig, 0, sizeof(sc->mask_sig));
    memset(sc->mask, 0, sizeof(sc->mask));
}

int is_bufflen_enough(ssrctx_t *sc, int needlen)
{
    if (sc->qbuff_end - sc->qbuff_beg < needlen)
    {
        sc->next_need_len = needlen;
        return 0;
    }
    sc->next_need_len = 37;
    return 1;
}

uint8_t matrixMultiply(uint8_t mat1[8], uint8_t vec)
{
    uint8_t result = 0;
    for (int i = 0; i < 8; i++)
    {
        uint8_t row_value = mat1[i];
        uint8_t sum = 0;
        uint8_t temp_vec = vec;
        for (int j = 0; j < 8; j++)
        {
            sum ^= (row_value & 0x01) & (temp_vec & 0x01);
            row_value >>= 1;
            temp_vec >>= 1;
        }
        if (sum)
        {
            result |= (1 << (7 - i));
        }
    }
    return result;
}

void rebase(uint8_t *encoded_msg)
{
    uint8_t mat1[8] = {
        0b10011011,
        0b11011101,
        0b00111110,
        0b00011100,
        0b00110111,
        0b10110011,
        0b01100000,
        0b10010100};

    for (int i = 0; i < 255; i++)
    {
        *(encoded_msg + i) = matrixMultiply(mat1, *(encoded_msg + i));
    }
}
void rebasex(uint8_t *encoded_msg)
{
    uint8_t mat1[8] = {
        0b11111110,
        0b01101001,
        0b01101011,
        0b00001101,
        0b11101111,
        0b11110010,
        0b01011011,
        0b11000111};

    for (int i = 0; i < 223; i++)
    {
        *(encoded_msg + i) = matrixMultiply(mat1, *(encoded_msg + i));
    }
}

void print_type12(ssrctx_t *sc, int cnid)
{
    ssr_network_t *sn = get_ssr_network(sc, cnid);
    char msg1[10240 * 5];
    snprintf(msg1, sizeof(msg1), "Trop NID=%d (%s)", cnid, clasGrid[cnid - 1].name);
    if (sn->tavail & 1)
    {
        char temp_msg[256];
        snprintf(temp_msg, sizeof(temp_msg), " qual=%.3f[mm]", sn->ura);
        strcat(msg1, temp_msg);
        snprintf(temp_msg, sizeof(temp_msg), " t00=%.3f[m]", sn->t00);
        strcat(msg1, temp_msg);
        if (1 <= sn->tctype)
        {
            if (sn->t01 != -64 && sn->t10 != -64)
            {
                snprintf(temp_msg, sizeof(temp_msg), " t01=%.3f[m/deg] t10=%.3f[m/deg]", sn->t01, sn->t10);
                strcat(msg1, temp_msg);
            }
        }
        if (2 <= sn->tavail)
        {
            if (sn->t11 != -64)
            {
                snprintf(temp_msg, sizeof(temp_msg), " t11=%.3f[m/deg^2]", sn->t11);
                strcat(msg1, temp_msg);
            }
        }
    }

    if (sn->tavail >> 1 & 1)
    {
        char temp_msg[256];
        snprintf(temp_msg, sizeof(temp_msg), " offset=%.3f[m]", sn->tro);
        strcat(msg1, temp_msg);
        strcat(msg1, "\nTrop  Lat.   Lon. residual[m]");
        for (int grid = 0; grid < sn->ngrid; grid++)
        {
            float lat = clasGrid[cnid - 1].points[grid].lat;
            float lon = clasGrid[cnid - 1].points[grid].lon;
            snprintf(temp_msg, sizeof(temp_msg), "\nTrop %5.2f %6.2f     %.3f", lat, lon, sn->tr[grid]);
            strcat(msg1, temp_msg);
        }
    }
    if (sn->savail & 1)
    {
        for (int i = 0; i < MAXSSRSAT; i++)
        {
            if (!sn->mask_st12[i])
                continue;
            char temp_msg[256];
            char prnstr[4];
            prn2str(i, prnstr);
            snprintf(temp_msg, sizeof(temp_msg), "\nSTEC %s  Lat.   Lon. residual[TECU] qual=%.3f[TECU]", prnstr, sn->ssr_epoch[i].stura);
            strcat(msg1, temp_msg);
            snprintf(temp_msg, sizeof(temp_msg), " c00=%.3f[TECU]", sn->ssr_epoch[i].c00);
            strcat(msg1, temp_msg);
            if (1 <= sn->ssr_epoch[i].sctype)
            {
                snprintf(temp_msg, sizeof(temp_msg), " c01=%.3f[TECU/deg] c10=%.3f[TECU/deg]", sn->ssr_epoch[i].c01, sn->ssr_epoch[i].c10);
                strcat(msg1, temp_msg);
            }
            if (2 <= sn->ssr_epoch[i].sctype)
            {
                snprintf(temp_msg, sizeof(temp_msg), " c11=%.3f[TECU/deg^2]", sn->ssr_epoch[i].c11);
                strcat(msg1, temp_msg);
            }
            if (3 <= sn->ssr_epoch[i].sctype)
            {
                snprintf(temp_msg, sizeof(temp_msg), " c02=%.3f[TECU/deg^2] c20=%.3f[TECU/deg^2]", sn->ssr_epoch[i].c02, sn->ssr_epoch[i].c20);
                strcat(msg1, temp_msg);
            }
            for (int grid = 0; grid < sn->ngrid; grid++)
            {
                float lat = clasGrid[cnid - 1].points[grid].lat;
                float lon = clasGrid[cnid - 1].points[grid].lon;
                snprintf(temp_msg, sizeof(temp_msg), "\nSTEC %s %5.2f %6.2f         %.3f", prnstr, lat, lon, sn->ssr_epoch[i].sr[grid]);
                strcat(msg1, temp_msg);
            }
        }
    }
    printf("%s\n", msg1);
}

int n_mask(uint64_t n)
{
    int count = 0;
    while (n)
    {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

int iodssr_not_match(ssrctx_t *sc)
{
    sc->qzss_wait_new_subframe = 1;
    return 0;
}

gtime_t gpsts2gtime(double sec)
{
    int gps_week;
    gtime_t current_time = utc2gpst(timeget());
    double gps_seconds = time2gpst(current_time, &gps_week);
    if (gps_seconds > sec)
        return gpst2time(gps_week, sec);
    return gpst2time(gps_week - 1, sec);
}

double diff_time(double gps_epo_sec, double gnss_hours_sec)
{
    gtime_t gps_time = gpsts2gtime(gps_epo_sec);
    double ep[6] = {0};
    time2epoch(gps_time, ep);
    double delta_sec = gnss_hours_sec - (ep[5] + ep[4] * 60);
    if (delta_sec < 0)
        return delta_sec + 3600;
    return delta_sec;
}

uint8_t *get_cell_mask(ssrctx_t *sc, int *i_beg, int len, int cell_mask_flag)
{
    uint8_t *rvl = (uint8_t *)calloc(1, len);
    if (!cell_mask_flag)
    {
        for (int i = 0; i < len; ++i)
        {
            rvl[i] = 1;
        }
        return rvl;
    }
    get_bits_array(sc->qbuff, sc->qbuff_beg + *i_beg, len, rvl);
    *i_beg = *i_beg + len;
    return rvl;
}
gtime_t get_t0(ssrctx_t *sc)
{
    double diff_t = diff_time(sc->gps_epoch_t, sc->gnss_hourly_epoch_t);
    return timeadd(gpsts2gtime(sc->gps_epoch_t), diff_t);
}
uint64_t q_get_bits(ssrctx_t *sc, int i_beg, int len)
{
    return get_bits(sc->qbuff, sc->qbuff_beg + i_beg, len);
}

uint64_t q_offset_bits(ssrctx_t *sc, int *i_beg, int len)
{
    uint64_t rvl = get_bits(sc->qbuff, sc->qbuff_beg + *i_beg, len);
    *i_beg += len;
    return rvl;
}

int set_svmask(ssrctx_t *sc, int i_beg, int *svmask)
{
    int svmask_index = 0;
    for (int i = 0; i < MAXSSRSAT; ++i)
    {
        if (sc->mask[i] == 0)
            continue;
        svmask[svmask_index++] = q_offset_bits(sc, &i_beg, 1);
    }
    return svmask_index;
}
int set_snmask(ssrctx_t *sc, int *snmask, int i_beg)
{
    int snmask_index = 0;
    for (int i = 0; i < MAXSSRSAT; ++i)
    {
        snmask[i] = 0;
        if (sc->mask[i] == 0)
            continue;
        snmask_index++;
        if (q_offset_bits(sc, &i_beg, 1))
            snmask[i] = 1;
    }
    return snmask_index;
}

int prn_sig2codeindex(int prn, int sig)
{
    return code_index[prn2sys(prn)][sig] - 1;
}

void decode_qzssr_head(ssrctx_t *sc)
{
    sc->message_number = q_get_bits(sc, 0, 12);
    sc->message_sub_type_id = q_get_bits(sc, 12, 4);
    sc->gnss_hourly_epoch_t = q_get_bits(sc, 16, 12);
    sc->ssr_update_interval = q_get_bits(sc, 28, 4);
    sc->mul_msg_indicator = q_get_bits(sc, 32, 1);
    sc->last_iodssr = q_get_bits(sc, 33, 4);
    if (sc->message_number != 4073)
        sc->qzss_wait_new_subframe = 1;
}

int decode_qzssr_type1(ssrctx_t *sc)
{
    if (sc->qbuff_beg != 0)
    {
        sc->qzss_wait_new_subframe = 1;
        return 0;
    }
    initctx(sc);
    sc->lend++;
    sc->lvalid = 1;
    sc->message_number = q_get_bits(sc, 0, 12);
    if (sc->message_number != 4073)
    {
        sc->qzss_wait_new_subframe = 1;
        return 0;
    }
    sc->message_sub_type_id = q_get_bits(sc, 12, 4);
    sc->gps_epoch_t = q_get_bits(sc, 16, 20);
    sc->ssr_update_interval = q_get_bits(sc, 36, 4);
    sc->mul_msg_indicator = q_get_bits(sc, 40, 1);
    sc->IODSSR = q_get_bits(sc, 41, 4);
    sc->last_iodssr = sc->IODSSR;
    sc->n_gnss = q_get_bits(sc, 45, 4);
    int offset = 49;
    sc->cellmask_n = 0;
    for (int j = 0; j < sc->n_gnss; ++j)
    {
        int gnss_id = q_offset_bits(sc, &offset, 4);
        uint64_t sat_mask = q_offset_bits(sc, &offset, 40);
        for (int i = 0; i < 40; ++i)
        {
            sc->mask[gnss_id * 40 + i] = (sat_mask >> 39 - i) & 1;
        }
        uint64_t sig_mask = q_offset_bits(sc, &offset, 16);
        for (int i = 0; i < QZSSSIGNUM; ++i)
        {
            sc->mask_sig[gnss_id * QZSSSIGNUM + i] = (sig_mask >> (15 - i)) & 1;
        }
        uint64_t n_cell_mask = n_mask(sat_mask) * n_mask(sig_mask);
        uint8_t cell_mask_flag = q_offset_bits(sc, &offset, 1);
        for (int i = 0; i < n_cell_mask; ++i)
        {
            if (cell_mask_flag)
                sc->gnss_cell_mask[sc->cellmask_n++] = q_offset_bits(sc, &offset, 1);
            else
                sc->gnss_cell_mask[sc->cellmask_n++] = 1;
        }
    }
    sc->next_need_len = 37;
    sc->qbuff_beg = offset;
    return 1;
}

int decode_qzssr_type2(ssrctx_t *sc)
{
    decode_qzssr_head(sc);
    int offset = 37;
    for (int i = 0; i < MAXSSRSAT; ++i)
    {
        if (sc->mask[i] == 0)
            continue;
        int iode = q_offset_bits(sc, &offset, is_galileo(i) ? 10 : 8);
        double delta_radial = uint64_to_int64(q_offset_bits(sc, &offset, 15), 15) * 0.0016;
        double delta_along = uint64_to_int64(q_offset_bits(sc, &offset, 13), 13) * 0.0064;
        double delta_cross = uint64_to_int64(q_offset_bits(sc, &offset, 13), 13) * 0.0064;
        if (sc->IODSSR != sc->last_iodssr)
            return iodssr_not_match(sc);
        sc->ssr_epoch[i].t0[0] = timeadd(gpsts2gtime(sc->gps_epoch_t), diff_time(sc->gps_epoch_t, sc->gnss_hourly_epoch_t));
        sc->ssr_epoch[i].iode = iode;
        sc->ssr_epoch[i].deph[0] = delta_radial;
        sc->ssr_epoch[i].deph[1] = delta_along;
        sc->ssr_epoch[i].deph[2] = delta_cross;
    }
    sc->qbuff_beg += offset;
    sc->lend++;
    return 2;
}

int decode_qzssr_type3(ssrctx_t *sc)
{
    decode_qzssr_head(sc);
    int offset = 37;
    for (int i = 0; i < MAXSSRSAT; ++i)
    {
        sc->ssr_epoch[i].update = 0;
        if (sc->mask[i] == 0)
            continue;
        double clk = uint64_to_int64(q_offset_bits(sc, &offset, 15), 15) * 0.0016;
        if (sc->IODSSR != sc->last_iodssr || sc->lvalid == 0)
            return iodssr_not_match(sc);
        double diff_t = diff_time(sc->gps_epoch_t, sc->gnss_hourly_epoch_t);
        sc->ssr_epoch[i].t0[1] = timeadd(gpsts2gtime(sc->gps_epoch_t), diff_t);
        sc->ssr_epoch[i].dclk[0] = clk;
        sc->ssr_epoch[i].update = 1;
    }
    sc->qbuff_beg += offset;
    sc->lend++;
    return 3;
}

int decode_qzssr_type4(ssrctx_t *sc)
{
    decode_qzssr_head(sc);
    int offset = 37;
    int cell_mask_index = 0;
    for (int i = 0; i < MAXSSRSAT; ++i)
    {
        if (sc->mask[i] == 0)
            continue;
        for (int j = 0; j < QZSSSIGNUM; ++j)
        {
            if (!sc->mask_sig[prn2sys(i) * QZSSSIGNUM + j])
                continue;
            if (!sc->gnss_cell_mask[cell_mask_index++])
                continue;
            if (!is_bufflen_enough(sc, offset + 11))
                return 0;
            double cbias = uint64_to_int64(q_offset_bits(sc, &offset, 11), 11) * 0.02;
            if (sc->IODSSR != sc->last_iodssr)
                return iodssr_not_match(sc);
            sc->ssr_epoch[i].t0[4] = get_t0(sc);
            if (prn_sig2codeindex(i, j) >= 0)
                sc->ssr_epoch[i].cbias[prn_sig2codeindex(i, j)] = cbias;
        }
    }
    sc->qbuff_beg += offset;
    sc->lend++;
    return 4;
}

int decode_qzssr_type5(ssrctx_t *sc)
{
    /* pbias: 暂时没有发 */
    decode_qzssr_head(sc);
    sc->qzss_wait_new_subframe = 1;
    return 0;
}

int decode_qzssr_type6(ssrctx_t *sc)
{
    decode_qzssr_head(sc);
    int offset = 37;
    if (!is_bufflen_enough(sc, offset + 8))
        return 0;
    int cbias_existing_flag = q_offset_bits(sc, &offset, 1);
    int pbias_existing_flag = q_offset_bits(sc, &offset, 1);
    int network_correction = q_offset_bits(sc, &offset, 1);
    int cnid = q_offset_bits(sc, &offset, 5);
    ssr_network_t *sn = get_ssr_network(sc, cnid);
    if (!is_bufflen_enough(sc, offset + set_snmask(sc, sn->mask_st6, offset)))
        return 0;
    offset += set_snmask(sc, sn->mask_st6, offset);
    int cell_mask_index = 0;
    for (int i = 0; i < MAXSSRSAT; ++i)
    {
        if (!sc->mask[i])
            continue;
        for (int j = 0; j < QZSSSIGNUM; ++j)
        {
            if (!sc->mask_sig[prn2sys(i) * QZSSSIGNUM + j])
                continue;
            if (!sc->gnss_cell_mask[cell_mask_index++])
                continue;
            if (!sn->mask_st6[i])
                continue;
            if (cbias_existing_flag)
            {
                if (!is_bufflen_enough(sc, offset + 11))
                    return 0;
                double cbias = uint64_to_int64(q_offset_bits(sc, &offset, 11), 11) * 0.02;
                if (sc->IODSSR != sc->last_iodssr)
                    return iodssr_not_match(sc);
                if (prn_sig2codeindex(i, j) >= 0)
                    sc->ssr_epoch[i].cbias[prn_sig2codeindex(i, j)] = cbias;
            }
            if (pbias_existing_flag)
            {
                if (!is_bufflen_enough(sc, offset + 17))
                    return 0;
                double pbias = uint64_to_int64(q_offset_bits(sc, &offset, 15), 15) * 0.001;
                int phase_discontinuity_ndicator1 = q_offset_bits(sc, &offset, 2);
                if (sc->IODSSR != sc->last_iodssr)
                    return iodssr_not_match(sc);
                if (prn_sig2codeindex(i, j) >= 0)
                    sc->ssr_epoch[i].pbias[prn_sig2codeindex(i, j)] = pbias;
            }
        }
    }
    sc->qbuff_beg += offset;
    sc->lend++;
    return 6;
}

int decode_qzssr_type7(ssrctx_t *sc)
{
    decode_qzssr_head(sc);
    int offset = 37;
    for (int i = 0; i < MAXSSRSAT; ++i)
    {
        if (sc->mask[i] == 0)
            continue;
        double ura = q_offset_bits(sc, &offset, 6);
        if (sc->IODSSR != sc->last_iodssr)
            return iodssr_not_match(sc);
        double diff_t = diff_time(sc->gps_epoch_t, sc->gnss_hourly_epoch_t);
        sc->ssr_epoch[i].t0[3] = timeadd(gpsts2gtime(sc->gps_epoch_t), diff_t);
        sc->ssr_epoch[i].ura = ura2dist(ura);
    }
    sc->qbuff_beg += offset;
    sc->lend++;
    return 7;
}

int decode_qzssr_type8(ssrctx_t *sc)
{
    sc->qzss_wait_new_subframe = 1;
    decode_qzssr_head(sc);
    return 0;
}

int decode_qzssr_type9(ssrctx_t *sc)
{
    sc->qzss_wait_new_subframe = 1;
    decode_qzssr_head(sc);
    return 0;
}

int decode_qzssr_type10(ssrctx_t *sc)
{
    sc->qzss_wait_new_subframe = 1;
    return 0;
}

int decode_qzssr_type11(ssrctx_t *sc)
{
    decode_qzssr_head(sc);
    int offset = 37;
    int l = 45;
    if (!is_bufflen_enough(sc, offset + 8))
        return 0;
    int orb_existing_flag = q_offset_bits(sc, &offset, 1);
    int clk_existing_flag = q_offset_bits(sc, &offset, 1);
    int network_correction = q_offset_bits(sc, &offset, 1);
    int cnid = q_offset_bits(sc, &offset, 5);

    ssr_network_t *sn = get_ssr_network(sc, cnid);
    sn->networkcorr = network_correction;
    if (!is_bufflen_enough(sc, offset + set_snmask(sc, sn->mask_st11, offset)))
        return 0;
    offset += set_snmask(sc, sn->mask_st11, offset);
    for (int i = 0; i < MAXSSRSAT; ++i)
    {
        if (!sn->mask_st11[i])
            continue;
        if (orb_existing_flag)
        {
            if (sc->IODSSR != sc->last_iodssr)
                return iodssr_not_match(sc);
            if (!is_bufflen_enough(sc, offset + prn2sys(i) == 2 ? 51 : 49))
                return 0;
            sn->ssr_epoch[i].t0[0] = timeadd(gpsts2gtime(sc->gps_epoch_t), diff_time(sc->gps_epoch_t, sc->gnss_hourly_epoch_t));
            sn->ssr_epoch[i].iode = q_offset_bits(sc, &offset, prn2sys(i) == 2 ? 10 : 8);
            sn->ssr_epoch[i].deph[0] = uint64_to_int64(q_offset_bits(sc, &offset, 15), 15) * 0.0016;
            sn->ssr_epoch[i].deph[1] = uint64_to_int64(q_offset_bits(sc, &offset, 13), 13) * 0.0064;
            sn->ssr_epoch[i].deph[2] = uint64_to_int64(q_offset_bits(sc, &offset, 13), 13) * 0.0064;
        }
        if (clk_existing_flag)
        {
            if (sc->IODSSR != sc->last_iodssr)
                return iodssr_not_match(sc);
            if (!is_bufflen_enough(sc, offset + 15))
                return 0;
            sn->ssr_epoch[i].dclk[0] = uint64_to_int64(q_offset_bits(sc, &offset, 15), 15) * 0.0016;
        }
    }
    sc->qbuff_beg += offset;
    sc->lend++;
    return 11;
}

int decode_qzssr_type12(ssrctx_t *sc)
{
    decode_qzssr_head(sc);
    int offset = 37;
    if (!is_bufflen_enough(sc, offset + 2 + 2 + 5 + 6))
        return 0;
    uint8_t tavail;
    uint8_t savail;
    tavail = q_offset_bits(sc, &offset, 2); // 对流层校正可用性
    savail = q_offset_bits(sc, &offset, 2); // STEC 校正可用性

    uint8_t cnid = q_offset_bits(sc, &offset, 5);  // compact network ID
    uint8_t ngrid = q_offset_bits(sc, &offset, 6); // grid number

    if (cnid < 1 || cnid > 19)
    {
        fprintf(stderr, "invalid compact network ID: %d\n", cnid);
        sc->qzss_wait_new_subframe = 1;
        return 0;
    }

    if (clasGrid[cnid - 1].grid_count != ngrid)
    {
        fprintf(stderr, "cnid=%d, ngrid=%d != %d\n", cnid, ngrid, clasGrid[cnid - 1].grid_count);
        sc->qzss_wait_new_subframe = 1;
        return 0;
    }

    ssr_network_t *snc = get_ssr_network(sc, cnid);
    snc->tavail = tavail;
    snc->savail = savail;
    if (snc->tavail & 1)
    {
        if (!is_bufflen_enough(sc, offset + 6 + 2 + 9))
            return 0;
        snc->ura = ura2dist(q_offset_bits(sc, &offset, 6));                   // 对流层质量指示
        snc->tctype = q_offset_bits(sc, &offset, 2);                          // 对流层校正类型
        snc->t00 = uint64_to_int64(q_offset_bits(sc, &offset, 9), 9) * 0.004; // 对流层多项式系数

        if (1 <= snc->tctype)
        {
            if (!is_bufflen_enough(sc, offset + 14))
                return 0;
            snc->t01 = uint64_to_int64(q_offset_bits(sc, &offset, 7), 7) * 0.002;
            snc->t10 = uint64_to_int64(q_offset_bits(sc, &offset, 7), 7) * 0.002;
        }
        if (2 <= snc->tctype)
        {
            if (!is_bufflen_enough(sc, offset + 7))
                return 0;
            snc->t11 = uint64_to_int64(q_offset_bits(sc, &offset, 7), 7) * 0.001;
        }
    }

    if (snc->tavail >> 1 & 1)
    {
        if (!is_bufflen_enough(sc, offset + 5))
            return 0;
        uint8_t trs = q_offset_bits(sc, &offset, 1);       // 对流层残差大小
        double tro = q_offset_bits(sc, &offset, 4) * 0.02; // 对流层残差偏移
        int bw = trs ? 8 : 6;
        snc->tro = tro;
        snc->ngrid = ngrid;

        for (int grid = 0; grid < snc->ngrid; grid++)
        {
            if (!is_bufflen_enough(sc, offset + bw))
                return 0;
            snc->tr[grid] = uint64_to_int64(q_offset_bits(sc, &offset, bw), bw) * 0.004; // 对流层残差
        }
    }

    if (snc->savail & 1)
    {
        if (!is_bufflen_enough(sc, offset + set_snmask(sc, snc->mask_st12, offset)))
            return 0;
        offset += set_snmask(sc, snc->mask_st12, offset);
        for (int i = 0; i < MAXSSRSAT; i++)
        {
            if (!snc->mask_st12[i])
                continue;
            if (!is_bufflen_enough(sc, offset + 6 + 2 + 14))
                return 0;
            snc->ssr_epoch[i].stura = ura2dist(q_offset_bits(sc, &offset, 6));                  // STEC 质量指示
            snc->ssr_epoch[i].sctype = q_offset_bits(sc, &offset, 2);                           // STEC 校正类型
            snc->ssr_epoch[i].c00 = uint64_to_int64(q_offset_bits(sc, &offset, 14), 14) * 0.05; // STEC 多项式系数
            if (1 <= snc->ssr_epoch[i].sctype)
            {
                if (!is_bufflen_enough(sc, offset + 12 + 12))
                    return 0;
                snc->ssr_epoch[i].c01 = uint64_to_int64(q_offset_bits(sc, &offset, 12), 12) * 0.02;
                snc->ssr_epoch[i].c10 = uint64_to_int64(q_offset_bits(sc, &offset, 12), 12) * 0.02;
            }

            if (2 <= snc->ssr_epoch[i].sctype)
            {
                if (!is_bufflen_enough(sc, offset + 10))
                    return 0;
                snc->ssr_epoch[i].c11 = uint64_to_int64(q_offset_bits(sc, &offset, 10), 10) * 0.02;
            }
            if (3 <= snc->ssr_epoch[i].sctype)
            {
                if (!is_bufflen_enough(sc, offset + 16))
                    return 0;
                snc->ssr_epoch[i].c02 = uint64_to_int64(q_offset_bits(sc, &offset, 8), 8) * 0.005;
                snc->ssr_epoch[i].c20 = uint64_to_int64(q_offset_bits(sc, &offset, 8), 8) * 0.005;
            }
            uint8_t srs = q_offset_bits(sc, &offset, 2);
            int bw = (srs == 0) ? 4 : (srs == 1) ? 4
                                  : (srs == 2)   ? 5
                                                 : 7;
            double lsb = (srs == 0)   ? 0.04
                         : (srs == 1) ? 0.12
                         : (srs == 2) ? 0.16
                                      : 0.24;
            for (int grid = 0; grid < snc->ngrid; grid++)
            {
                if (!is_bufflen_enough(sc, offset + bw))
                    return 0;
                snc->ssr_epoch[i].sr[grid] = uint64_to_int64(q_offset_bits(sc, &offset, bw), bw) * lsb;
            }
        }
    }
    sc->qbuff_beg += offset;
    sc->lend++;
    // print_type12(sc, cnid);
    return 12;
}

int decode_qzssr_type_other(ssrctx_t *sc)
{
    sc->qzss_wait_new_subframe = 1;
    return 0;
}

uint32_t heapup_preamb(uint32_t preamb, unsigned char data)
{
    preamb <<= 8;
    preamb += data;
    return preamb;
}

void copy_bits(unsigned char *target, int target_bit_start, unsigned char *source, int source_bit_start, int bit_count)
{
    for (int i = 0; i < bit_count; i++)
    {
        int target_bit_position = target_bit_start + i;
        int source_bit_position = source_bit_start + i;
        unsigned char source_bit = (source[source_bit_position / 8] >> (7 - (source_bit_position % 8))) & 1;
        target[target_bit_position / 8] &= ~(1 << (7 - (target_bit_position % 8)));
        target[target_bit_position / 8] |= (source_bit << (7 - (target_bit_position % 8)));
    }
}

void update_qbuff(ssrctx_t *sc)
{
    if (sc->qzss_wait_new_subframe && !sc->subframe_indicator)
        return;
    if (sc->subframe_indicator)
    {
        reset_qzss_buff(sc);
        sc->sub_n = 0;
        sc->qzss_wait_new_subframe = 0;
    }
    sc->sub_n++;
    copy_bits(sc->qbuff, sc->qbuff_end, sc->buff, 89, 1695);
    sc->qbuff_end += 1695;
}

uint8_t get_msg_type(ssrctx_t *sc)
{
    return get_bits(sc->qbuff, sc->qbuff_beg + 12, 4);
}

int count_errors(const uint8_t *original_data, const uint8_t *decoded_data)
{
    int error_count = 0;
    for (int i = 0; i < 223; i++)
    {
        if (original_data[i] != decoded_data[i])
        {
            error_count++;
        }
    }
    return error_count;
}

correct_reed_solomon *rs = 0;
int reed_solomon_decode(ssrctx_t *sc)
{
    if (!rs)
        rs = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds, 112, 11, 32);
    unsigned char encoded_msg[255] = {0};
    memcpy(encoded_msg + 9, sc->buff + 4, 246);
    rebase(encoded_msg);
    ssize_t decoded_length = correct_reed_solomon_decode(rs, encoded_msg, 255, sc->buff);
    if (decoded_length != 223)
        memcpy(sc->buff, encoded_msg, 223);
    int errcnt = count_errors(encoded_msg, sc->buff);
    rebasex(sc->buff);
    return decoded_length != -1 ? errcnt : -1;
}

static ssrctx_t *get_ssr_ctx_prn(ssrctx_t *sc)
{
    static ssrctx_t *ssr_ctxs[MAXSSRSAT] = {0};
    int prn = get_bits(sc->buff, 72, 8);
    if (!ssr_ctxs[prn])
    {
        ssr_ctxs[prn] = calloc(1, sizeof(ssrctx_t));
        ssr_ctxs[prn]->next_need_len = 37;
        ssr_ctxs[prn]->qzss_wait_new_subframe = 1;
    }
    memcpy(ssr_ctxs[prn]->buff, sc->buff, 250);
    ssr_ctxs[prn]->prn = prn;
    ssr_ctxs[prn]->subframe_indicator = get_bits(sc->buff, 87, 1);
    return ssr_ctxs[prn];
}

void printf_qzss(ssrctx_t *sc)
{
    printf("msg_type: %5d subframe_indicator: %5d ", sc->message_sub_type_id, sc->subframe_indicator);
    printf("message_number: %5d QZSSPRN: %5d ", sc->message_number, sc->prn);
    printf("gnss_hourly_epoch_t: %5d ", sc->gnss_hourly_epoch_t);
    printf("gps_epoch_t: %5d ssr_update_interval: %5d ", sc->gps_epoch_t, sc->ssr_update_interval);
    printf("mul_msg_indicator: %5d IODSSR: %5d last_iodssr: %5d ", sc->mul_msg_indicator, sc->IODSSR, sc->last_iodssr);
    printf("n_gnss: %5d lvalid: %5d lend: %5d\n", sc->n_gnss, sc->lvalid, sc->lend);
}

int switch_msg(ssrctx_t *sc)
{
    int type = get_msg_type(sc);
    if (!sc->lvalid && type != 1)
    {
        reset_qzss_buff(sc);
        return 0;
    }
    switch (type)
    {
    case 1:
        return decode_qzssr_type1(sc);
    case 2:
        return decode_qzssr_type2(sc);
    case 3:
        return decode_qzssr_type3(sc);
    case 4:
        return decode_qzssr_type4(sc);
    case 5:
        return decode_qzssr_type5(sc);
    case 6:
        return decode_qzssr_type6(sc);
    case 7:
        return decode_qzssr_type7(sc);
    case 8:
        return decode_qzssr_type8(sc);
    case 9:
        return decode_qzssr_type9(sc);
    case 10:
        return decode_qzssr_type10(sc);
    case 11:
        return decode_qzssr_type11(sc);
    case 12:
        return decode_qzssr_type12(sc);
    default:
        return decode_qzssr_type_other(sc);
    }
}
int get_alert_flag(ssrctx_t *sc)
{
    return get_bits(sc->buff, 88, 1);
}
int get_prn(ssrctx_t *sc)
{
    return get_bits(sc->buff, 72, 8);
}

int decode_qzssr(ssrctx_t *user_ssr_ctx)
{
    int update = 0;
    int err_cnt = reed_solomon_decode(user_ssr_ctx);
    ssrctx_t *sc = get_ssr_ctx_prn(user_ssr_ctx);

    static int cnt_all = 0, cnt_err = 0;
    cnt_all++;
    printf("n: %4d prn: %3d bad_c: %4d err: %1d err_cnt: %2d sub_flag: %d\n", cnt_all, sc->prn, cnt_err, err_cnt == -1 ? 1 : 0, err_cnt, sc->subframe_indicator);
    if (err_cnt == -1)
    {
        cnt_err++;
        sc->qzss_wait_new_subframe = 1;
        return 0;
    }
    // if (sc->prn != 195)
    //     return 0;
    // if (get_alert_flag(sc))
    //     return 0;

    update_qbuff(sc);

    while (sc->qbuff_end - sc->qbuff_beg > sc->next_need_len && !sc->qzss_wait_new_subframe)
    {
        int type;
        if (type = switch_msg(sc))
        {
            memcpy(user_ssr_ctx, sc, sizeof(ssrctx_t));
            if (type == 2 || type == 3 || type == 4 || type == 6 || type == 12)
                update = 1;
            printf_qzss(sc);
        }
    }
    return update;
}

extern int input_qzssr(ssrctx_t *sc, unsigned char data)
{
    static uint32_t preamb = 0x0;
    if (sc->nbyte == 0)
    {
        preamb = heapup_preamb(preamb, data);
        if (preamb != QZSSPREAMB)
            return 0;
        sc->nbyte = 4;
        *((uint32_t *)sc->buff) = 0x1DFCCF1A;
        return 0;
    }
    sc->buff[sc->nbyte++] = data;
    if (sc->nbyte == 250)
    {
        sc->nbyte = 0;
        return decode_qzssr(sc);
    }
    return 0;
}