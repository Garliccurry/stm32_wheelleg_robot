#ifndef __LOG_H__
#define __LOG_H__

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

/* 配置宏定义 */
#define CONFIG_LOG_BUF_SIZE  TX_BUF_SIZE
#define CONFIG_LOG_DEF_LEVEL LOG_LEVEL_DEBUG
#define LOG_NEWLINE_SIGN     "\r\n" // STM32串口常用换行符

/* 日志级别枚举 */
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_BOTTOM
} log_level_t;

/* 格式控制宏 */
#define LOG_FMT_RAW        (0u)
#define LOG_FMT_LEVEL_STR  (1u << 0)
#define LOG_FMT_TIME_STAMP (1u << 1)
#define LOG_FMT_FUNC_LINE  (1u << 2)

/* 启用日志级别开关 */
#define LOG_DEBUG_EN 1
#define LOG_INFO_EN  1
#define LOG_WARN_EN  1
#define LOG_ERROR_EN 1

/* 日志宏定义 */

#if LOG_DEBUG_EN
#define LOG_DEBUG(fmt, ...) Log_Print(LOG_LEVEL_DEBUG, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#if LOG_INFO_EN
#define LOG_INFO(fmt, ...) Log_Print(LOG_LEVEL_INFO, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif

#if LOG_WARN_EN
#define LOG_WARN(fmt, ...) Log_Print(LOG_LEVEL_WARN, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...)
#endif

#if LOG_ERROR_EN
#define LOG_ERROR(fmt, ...) Log_Print(LOG_LEVEL_ERROR, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)
#endif

/* 日志输出函数类型 */
typedef void (*logOutputFunc)(uint8_t *data, uint16_t size);

void Log_Init(void);
void Log_Print(uint32_t level, const char *func, uint32_t line, const char *fmt, ...);
#endif