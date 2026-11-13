#ifndef _TIME_H
#define _TIME_H
typedef long time_t;
typedef struct { long tv_sec; long tv_usec; } struct_timeval;
#define timeval struct_timeval
struct tm { int tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year, tm_wday, tm_yday, tm_isdst; long tm_gmtoff; const char *tm_zone; };
time_t time(time_t *);
struct tm *localtime(const time_t *);
size_t strftime(char *, size_t, const char *, const struct tm *);
int gettimeofday(struct timeval *, void *);
struct timespec { long tv_sec; long tv_nsec; };
#define CLOCK_MONOTONIC 6
#define CLOCK_REALTIME 0
int clock_gettime(int, struct timespec *);
#endif
