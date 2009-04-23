/* log.h
** Created on Fri Feb 16 2009
*/

#ifndef _LOG_H_
#define _LOG_H_

extern char *logLevels[];
extern char *logNames[];

#define REPORT(level, format, ...)    printf("%s[%s][ln%d] %s" format "\n", logLevels[level], __FUNCTION__, __LINE__, logNames[level], ##__VA_ARGS__)


/*
** \brief LOG_LVL_ERR - logging at this level will always be
** enabled. Use this to print something that is an error that should
** never happen.
*/
#define LOG_LVL_ERR          0

/*
** \brief LOG_LVL_WARN - logging at this level will always be
** enabled. Use this to print something that is an error but is
** recoverable.
*/
#define LOG_LVL_WARN         1

/*
** \brief LOG_LVL_NOTICE - Startup and shutdown code may log at this
** level (as long as it is not excessive). This can assist us in
** diagnosing startup and shutdown issues from customer log files.
*/
#define LOG_LVL_NOTICE       2

/*
** \brief LOG_LVL_INFO - This level is for more detailed startup and
** shutdown logging, for other functions (ioctl, etc...) as long as
** their frequency of calling is not typically greater than once per 5
** seconds.
*/
#define LOG_LVL_INFO         3

/*
** \brief LOG_LVL_NOISE - Code that runs frequently such as code that
** processes streaming data.
*/
#define LOG_LVL_NOISE        4

#ifdef ENABLE_DEBUG
#define ERROR(format, ...)   REPORT(LOG_LVL_ERR, format, ##__VA_ARGS__)
#define WARN(format, ...)    REPORT(LOG_LVL_WARN, format, ##__VA_ARGS__)
#define NOTICE(format, ...)  REPORT(LOG_LVL_NOTICE, format, ##__VA_ARGS__)
#define INFO(format, ...)    REPORT(LOG_LVL_INFO, format, ##__VA_ARGS__)
#define NOISE(format, ...)   REPORT(LOG_LVL_NOISE, format, ##__VA_ARGS__)
#else
#define ERROR(format, ...)   REPORT(LOG_LVL_ERR, format, ##__VA_ARGS__)
#define WARN(format, ...)    REPORT(LOG_LVL_WARN, format, ##__VA_ARGS__)
#define NOTICE(format, ...)  { }
#define INFO(format, ...)    { }
#define NOISE(format, ...)   { }
#endif

#define LOG                  INFO

#endif /* _LOG_H_ */
