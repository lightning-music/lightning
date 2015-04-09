#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <stdarg.h>
#include <stdio.h>

typedef enum {
    Error,
    Warn,
    Info,
    Debug
} LogLevel;

typedef struct Log *Log;

Log
Log_init(FILE *stream);

void
Log_free(Log *log);

void
lightning_log(Log log, const char *file, long line,
              LogLevel level, const char *fmt, ...);

#define LOG(level, fmt, args...)                           \
    lightning_log(Log_init(NULL), __FILE__, __LINE__, level, fmt, args)

#endif
