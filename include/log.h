#pragma once

typedef enum {
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_LEVEL_LEN
} LOG_LEVEL;



void dv_log(LOG_LEVEL lvl, const char *fmt, ...);
