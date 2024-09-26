#include <stdio.h>
#include "correct.h"
// #include "ssr.h"
#include "rtklib.h"
#include "ssr.h"



void print_binary(unsigned char byte)
{
    for (int i = 7; i >= 0; i--)
    {
        printf("%c", (byte & (1 << i)) ? '1' : '0');
    }
}
void print_rtcmssr(rtcm_t *sc)
{
    char prnstr[5] = "";
    int line = 0;

    int satnum = 0;
    for (int i = 0; i < MAXSAT; ++i)
    {
        if (!sc->ssr[i].t0[1].time)
            continue;
        satnum++;
    }

    for (int i = 0; i < MAXSAT; ++i)
    {
        if (!sc->ssr[i].t0[1].time)
            continue;

        if (!line)
        {
            char s[100] = "";
            time2str(sc->ssr[i].t0[1], s, 0);
            printf("%s   PRN: %d   IODSSR: %2d   NUM:%3d\n", s, 1, 1, satnum); // head
        }
        /* prn */
        satno2id(i,prnstr);
        printf("%s", prnstr);

        /* orb */
        if (sc->ssr[i].t0[0].time)
        {
            printf(" orb:%9.4f%9.4f%9.4f  iode: %3d", sc->ssr[i].deph[0], sc->ssr[i].deph[1], sc->ssr[i].deph[2], sc->ssr[i].iode);
        }

        /* clk */
        if (sc->ssr[i].t0[1].time)
            printf(" clk:%9.4f", sc->ssr[i].dclk[0]);

        /* ura */
        if (sc->ssr[i].t0[3].time)
        printf(" ura:%8.2f", sc->ssr[i].ura);

        /* pbias */
        if (sc->ssr[i].t0[5].time)
        {
            printf(" pbias:");
            for (int j = 0; j < MAXCODE; ++j)
            {
                if (sc->ssr[i].pbias[j])
                    printf("%s %9.4f ", code2obs(j + 1), sc->ssr[i].pbias[j]);
            }
        }

        /* cbias */
        if (sc->ssr[i].t0[4].time)
        {
            printf(" cbias:");
            for (int j = 0; j < MAXCODE; ++j)
            {
                if (sc->ssr[i].cbias[j])
                    printf("%s %9.4f ", code2obs(j + 1), sc->ssr[i].cbias[j]);
            }
        }
        printf("\n");
        line++;
    }
}

int main()
{
    FILE *file;
    file = fopen("res/ssr_003502", "rb");
    if (file == NULL)
        return 1;
    unsigned char data;
    rtcm_t rtcm_ctx = {0};
    while (fread(&data, 1, 1, file) == 1)
    {
        int a = 0;
        if (a=input_rtcm3(&rtcm_ctx, data)){
            printf("%d\n", a);
            print_rtcmssr(&rtcm_ctx);
        }
    }
}