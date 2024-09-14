#ifndef IO_RTKLIB_IO_RTCM_H_
#define IO_RTKLIB_IO_RTCM_H_
#include "tinyppp.h"

extern nav_t g_sdk_nav;

extern void SDK_init(sta_t *, prcopt_t *popt, solopt_t *sopt, filopt_t *fopt);
extern void SDK_terminate();
extern void SDK_restart();
extern void SDK_setintv(double intv);
extern void SDK_setpath(const char *path);
extern void SDK_retrieve(const char *, char *, int len);
extern int IO_inputObsData(uint8_t data);
extern int IO_inputEphData(uint8_t data);
extern int IO_inputSsrData(uint8_t data);
extern int IO_inputAtmData(uint8_t data);
extern void *pthread_brd(void *x);
extern void *pthread_cal(void *x);
#endif