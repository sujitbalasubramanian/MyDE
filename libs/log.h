#ifndef LOG_H

#define LOG_H

#define LOG_ERR_MSG "\x1b[31m[error]\x1b[0m "
#define LOG_WARN_MSG "\x1b[33m[warn]\x1b[0m "
#define LOG_INFO_MSG "\x1b[32m[info]\x1b[0m "

#define LOG_ERR(...) fprintf(stderr, LOG_ERR_MSG __VA_ARGS__)
#define LOG_WARN(...) fprintf(stderr, LOG_WARN __VA_ARGS__)
#define LOG_INFO(...) fprintf(stderr, LOG_INFO_MSG __VA_ARGS__)
#define LOG(...) fprintf(stderr, __VA_ARGS__)

#endif  // !LOG_H
