#include <stdio.h>
#include "rtklib.h"
#include "ssr.h"

/* show message --------------------------------------------------------------*/
// extern int showmsg(char *format, ...)
// {
//     va_list arg;
//     va_start(arg, format);
//     vfprintf(stderr, format, arg);
//     va_end(arg);
//     fprintf(stderr, "\r");
//     return 0;
// }

// extern void settspan(gtime_t ts, gtime_t te) {}

// extern void settime(gtime_t time) {}

void ntripinit(char *args, stream_t *m_unsyncConn)
{
    int stype = STR_NONE;
    char *str = (char *)args;
    unsigned char buff[1024] = {0}, strin[256] = {0};
    if (!strncmp(str, "tcpcli:", 7))
    {
        stype = STR_TCPCLI;
        strcpy(strin, str + 9);
    }
    if (!strncmp(str, "ntripcli:", 9))
    {
        stype = STR_NTRIPCLI;
        strcpy(strin, str + 11);
    }
    if (!strncmp(str, "ntripsvr:", 9))
    {
        stype = STR_NTRIPSVR;
        strcpy(strin, str + 11);
    }
    if (!strncmp(str, "serial:", 7))
    {
        stype = STR_SERIAL;
        strcpy(strin, str + 9);
    }
    strinit(m_unsyncConn);
    printf("SSR connection got [%s]\n", strin);
    if (!stropen(m_unsyncConn, stype, STR_MODE_RW, strin))
    {
        printf("***ERROR(v_openRnx):can't reach the observation %s", args);
        exit(1);
    }
    strsettimeout(m_unsyncConn, 60000, 10000); /// 60s for timeout 10s for reconnect
}

void print_data(const unsigned char *data, size_t length) {
    int is_string = 1;

    // 检查数据是否是可打印字符串
    for (size_t i = 0; i < length; i++) {
        if (!isprint(data[i]) && data[i] != '\n' && data[i] != '\t' && data[i] != '\r') {
            is_string = 0;
            break;
        }
    }

    if (is_string) {
        // 打印为字符串
        printf("%s\n", data);
    } else {
        // 按十六进制格式打印二进制数据
        for (size_t i = 0; i < length; i++) {
            printf("%02x ", data[i]);
        }
        printf("\n");
    }
}
void *decode_test()
{
    stream_t ntripconn_r;
    ntripinit("ntripcli://wszcs001:123456@ssr.kplgnss.com:8070/KPLB2B", &ntripconn_r);
    ssrctx_t bcnx = {0};
    uint8_t buff[4024];
    while (1)
    {
        int nread = strread(&ntripconn_r, buff, 1024);
        for (int i = 0; i < nread; ++i)
        {
            if (1 == input_kplssr(&bcnx, *(buff + i))){
                // b2b2str(&bcnx); //context->ssr_epoch
                print_ssr(&bcnx);
            };
        }
        
        if (nread == 0)
            sleepms(100);
        else{
            // printf("len:%d\n", nread);
            // print_data(buff, nread);
        }
    }
}

void *decode_fromfile_test()
{
    FILE *input_file;
    // 打开二进制文件
    input_file = fopen("wpppb2b.bin", "rb");

    // 检查文件是否成功打开
    if (input_file == NULL) {
        perror("Error opening file");
    }

    ssrctx_t bcnx = {0};
    uint8_t buffer[1024];
    while (1)
    {
        size_t nread = fread(buffer, sizeof(char), sizeof(buffer), input_file);
        for (int i = 0; i < nread; ++i)
        {
            if (1 == input_kplssr(&bcnx, *(buffer + i))){
                print_ssr(&bcnx);
            };
        }
        if (nread == 0)
            break;
    }
}

int main()
{
    traceopen("");
    // tracelevel(5);
    // decode_fromfile_test();
    decode_test();
    // pthread_t my_thread; // 用于存储新线程的标识符
    // // // 创建新线程
    // if (pthread_create(&my_thread, NULL, decode_test, NULL) != 0) {
    //     fprintf(stderr, "Failed to create thread\n");
    //     return 1;
    // }

    // output_b2b("192.168.0.132", 56379, "SSR_COM_AFIF", "ntripsvr://:0000@119.96.165.202:8500/PPPB2B");
    return 0;
}
