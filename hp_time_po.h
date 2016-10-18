/*
 * hp_time_po.h
 *
 *  Created on: 2016�~9��23��
 *      Author: poa116
 */

#ifndef MSP432_DATA_LOGGER_V1_1_HP_TIME_PO_H_
#define MSP432_DATA_LOGGER_V1_1_HP_TIME_PO_H_

#include <time.h>
#include "stdint.h"


typedef signed long long hptime_t;


/* A single byte flag type */

typedef uint8_t flag;



/* Error code for routines that normally return a high precision time.
 * The time value corresponds to '1902/1/1 00:00:00.000000' with the
 * default HPTMODULUS */
#define HPTERROR -2145916800000000LL

/* Macros to scale between Unix/POSIX epoch time & high precision time */
#define MS_EPOCH2HPTIME(X) X * (hptime_t) HPTMODULUS
#define MS_HPTIME2EPOCH(X) X / HPTMODULUS

/* Error code for routines that normally return a high precision time.
 * The time value corresponds to '1902/1/1 00:00:00.000000' with the
 * default HPTMODULUS */
#define HPTERROR -2145916800000000LL
//static hptime_t HPTERROR = -2145916800000000LL;
/* Default modulus of 1000000 defines tick interval as a microsecond */
#define HPTMODULUS 1000000


/* SEED binary time */
typedef struct btime_s
{
  uint16_t  year;
  uint16_t  day;
  uint8_t   hour;
  uint8_t   min;
  uint8_t   sec;
  uint8_t   unused;
  uint16_t  fract;
}
BTime;

//void RS232_Send_Char(char *str,int count,BYTE com);

hptime_t ms_timestr2hptime (char *timestr);
int  ms_md2doy (int year, int month, int mday, int *jday);
static hptime_t ms_time2hptime_int (int year, int day, int hour, int min, int sec, int usec);
hptime_t ms_btime2hptime (BTime *btime);
char *ms_hptime2mdtimestr (hptime_t hptime, char *mdtimestr, flag subseconds);

int ms_md2doy(int year, int month, int mday, int *jday);

//ms_timestr2hptime


#endif /* MSP432_DATA_LOGGER_V1_1_HP_TIME_PO_H_ */