/*
 * log.h - log
 *
 * Date   : 2020/04/19
 */
#ifndef LOG_H
#define LOG_H
#include <stdio.h>

#define Debug(format, ...)                                              \
    printf("%s:%d [DEBUG] " format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define Info(format, ...)                                               \
    printf("%s:%d [INFO ] " format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define Warn(format, ...)                                               \
    printf("%s:%d [WARN ] " format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define Error(format, ...)                                              \
    printf("%s:%d [ERROR] " format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif
