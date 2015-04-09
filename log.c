#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "mem.h"

static const char *logfile = "lightning.log";

static const char *
level_to_string(LogLevel l) {
    switch(l) {
    case Error: return "Error";
    case Warn:  return "Warn";
    case Info:  return "Info";
    case Debug: return "Debug";
    }
    return "Unknown";
}

struct Log {
    FILE *stream;
};

static Log default_logger = NULL;

Log
Log_init(FILE *stream) {
    if (stream != NULL) {
        Log log;
        NEW(log);
        log->stream = stream;
        return log;
    }
    if (default_logger == NULL) {
        NEW(default_logger);
        default_logger->stream = fopen(logfile, "a+");
    }
    return default_logger;
}

void
Log_free(Log *log)
{
    assert(log && *log);
    fclose((*log)->stream);
    FREE(*log);
}

void
lightning_log(Log log, const char *file, long line,
              LogLevel level, const char *fmt, ...)
{
    assert(log);
    va_list ap;
    char msg[4096];
    va_start(ap, fmt);
    vsprintf(msg, fmt, ap);
    fprintf(log->stream, "%s:%ld [%s] %s\n",
            file, line, level_to_string(level), msg);
    fflush(log->stream);
    va_end(ap);
}
