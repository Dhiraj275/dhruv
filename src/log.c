#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "../include/log.h"

char const *log_type[LOG_LEVEL_LEN] = {"DEBUG", "INFO", "WARN", "ERROR"};

time_t timestamp;
struct tm *m_time;

void get_time() {
  time(&timestamp);
  m_time = localtime(&timestamp);
}

void dv_log(LOG_LEVEL lvl, const char *fmt, ...) {
  get_time();
  printf("[%d:%d:%d %d:%d:%d] [%s] ", m_time->tm_mday, m_time->tm_mon + 1,
         m_time->tm_year + 1900, m_time->tm_hour, m_time->tm_min,
         m_time->tm_sec, log_type[lvl]);
  
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  printf("\n");
  fflush(stdout);
};
