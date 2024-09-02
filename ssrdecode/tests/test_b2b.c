#include <stdio.h>
#include "ssr.h"

int loadb2bpost_kx(char *line, char *buff)
{
    char hexstr[8];
    uint32_t data;
    if (!strstr(line, "$PPPB2B"))
        return 0;
    int prn;
    sscanf(line + 8, "%2d", &prn);
    for (int i = 0; i < 16; i++)
    {
        sscanf(line + 15 + i * 9, "%8[^,]", hexstr);
        u32tbyte_kx(strtoul(hexstr, NULL, 16), buff, i * 4);
    }
    return prn;
}

int b2bdecodepost_kx(char *b2bfile_kx)
{
    FILE *fb2bpost;
    if ((fb2bpost = fopen(b2bfile_kx, "r")) == NULL)
    {
        printf("open file %s failed!\n", b2bfile_kx);
        return 1;
    }

    char line[1024];
    ssrctx_t ssr_ctx = {0};
    while (fgets(line, 1024, fb2bpost))
    {
        int prn;
        if (prn = loadb2bpost_kx(line, ssr_ctx.buff))
        {
            decode_b2b(&ssr_ctx, prn);
            print_ssr(&ssr_ctx);
        }
    }
    fclose(fb2bpost);
}

int main()
{
    char *file = "res/workspace/0829_COM1.log";
    b2bdecodepost_kx(file);
}