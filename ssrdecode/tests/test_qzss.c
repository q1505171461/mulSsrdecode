#include <stdio.h>
#include "correct.h"
#include "ssr.h"

void print_binary(unsigned char byte)
{
    for (int i = 7; i >= 0; i--)
    {
        printf("%c", (byte & (1 << i)) ? '1' : '0');
    }
}

int main()
{
    FILE *file;
    file = fopen("res/qzssl6_com1.log", "rb");
    file = fopen("res/9-12qzss/com15_3rdCOM.txt", "rb");
    file = fopen("res/9-12gs/com_10 20240912-1", "rb");
    if (file == NULL)
        return 1;
    unsigned char data;
    ssrctx_t ssr_ctx = {0};
    while (fread(&data, 1, 1, file) == 1)
    {
        if (input_qzssr(&ssr_ctx, data))
            print_ssr(&ssr_ctx);
    }
}