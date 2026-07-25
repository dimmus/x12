/* SPDX-License-Identifier: MIT */
#include "xwm.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>

int xwm_log_level = XWM_LOG_INFO;

void
xwm_log_init(int level)
{
    const char *env = getenv("XWM_DEBUG");
    if (level > xwm_log_level)
        xwm_log_level = level;
    if (env && env[0]) {
        int n = atoi(env);
        if (n >= XWM_LOG_ERROR && n <= XWM_LOG_TRACE)
            xwm_log_level = n;
        else if (!strcmp(env, "1") || !strcasecmp(env, "true") ||
                 !strcasecmp(env, "debug"))
            xwm_log_level = XWM_LOG_DEBUG;
        else if (!strcasecmp(env, "trace") || !strcmp(env, "2"))
            xwm_log_level = XWM_LOG_TRACE;
    }
}

static const char *
level_name(int level)
{
    switch (level) {
    case XWM_LOG_ERROR:
        return "ERR";
    case XWM_LOG_INFO:
        return "INF";
    case XWM_LOG_DEBUG:
        return "DBG";
    case XWM_LOG_TRACE:
        return "TRC";
    default:
        return "???";
    }
}

void
xwm_log(int level, const char *fmt, ...)
{
    va_list ap;
    struct timeval tv;
    struct tm tm;
    char tbuf[32];

    if (level > xwm_log_level)
        return;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);
    strftime(tbuf, sizeof(tbuf), "%H:%M:%S", &tm);
    fprintf(stderr, "xwm %s.%03ld [%s] ", tbuf, (long)(tv.tv_usec / 1000),
            level_name(level));
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    fflush(stderr);
}

const char *
xwm_event_name(int type)
{
    switch (type) {
    case KeyPress:
        return "KeyPress";
    case KeyRelease:
        return "KeyRelease";
    case ButtonPress:
        return "ButtonPress";
    case ButtonRelease:
        return "ButtonRelease";
    case MotionNotify:
        return "MotionNotify";
    case EnterNotify:
        return "EnterNotify";
    case LeaveNotify:
        return "LeaveNotify";
    case FocusIn:
        return "FocusIn";
    case FocusOut:
        return "FocusOut";
    case Expose:
        return "Expose";
    case DestroyNotify:
        return "DestroyNotify";
    case UnmapNotify:
        return "UnmapNotify";
    case MapNotify:
        return "MapNotify";
    case MapRequest:
        return "MapRequest";
    case ReparentNotify:
        return "ReparentNotify";
    case ConfigureNotify:
        return "ConfigureNotify";
    case ConfigureRequest:
        return "ConfigureRequest";
    case PropertyNotify:
        return "PropertyNotify";
    case ClientMessage:
        return "ClientMessage";
    case MappingNotify:
        return "MappingNotify";
    case ColormapNotify:
        return "ColormapNotify";
    default:
        return "Event";
    }
}
