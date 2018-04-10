
#ifndef BugsnagLogger_h
#define BugsnagLogger_h

#define BSG_LOGLEVEL_DEBUG 40
#define BSG_LOGLEVEL_INFO 30
#define BSG_LOGLEVEL_WARN 20
#define BSG_LOGLEVEL_ERR 10
#define BSG_LOGLEVEL_NONE 0

#ifndef BSG_LOG_LEVEL
#define BSG_LOG_LEVEL BSG_LOGLEVEL_INFO
#endif

#if BSG_LOG_LEVEL >= BSG_LOGLEVEL_ERR
#define bsg_log_err NSLog
#else
#define bsg_log_err(format, ...)
#endif

#if BSG_LOG_LEVEL >= BSG_LOGLEVEL_WARN
#define bsg_log_warn NSLog
#else
#define bsg_log_warn(format, ...)
#endif

#if BSG_LOG_LEVEL >= BSG_LOGLEVEL_INFO
#define bsg_log_info NSLog
#else
#define bsg_log_info(format, ...)
#endif

#if BSG_LOG_LEVEL >= BSG_LOGLEVEL_DEBUG
#define bsg_log_debug NSLog
#else
#define bsg_log_debug(format, ...)
#endif

#endif /* BugsnagLogger_h */
