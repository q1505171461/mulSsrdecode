#include "tCore.h"
extern int index_string(const char *src, char key)
    {
        int len = strlen(src);
        int i;
        for (i = 0; i < len; i++)
        {
            if (src[i] == key)
                break;
        }
        if (i == len)
            return -1;
        else
            return i;
}