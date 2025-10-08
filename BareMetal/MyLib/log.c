#include "log.h"

static uint32_t      g_log_format = LOG_FMT_LEVEL_STR | LOG_FMT_TIME_STAMP | LOG_FMT_FUNC_LINE;
static uint32_t      g_log_level = CONFIG_LOG_DEF_LEVEL;
static logOutputFunc g_log_output = NULL;

static void Get_SystemTime(char *time_buf, uint16_t buf_size, uint16_t *ms);

static void Log_RegisterOutput(logOutputFunc func)
{
    g_log_output = func ? func : Usart_NorLogPrint;
}

static void Log_SetFormat(uint32_t format)
{
    g_log_format = format;
}

static void Log_SetLevel(uint32_t level)
{
    g_log_level = level;
}

void Log_Init(void)
{
    Log_SetLevel(CONFIG_LOG_DEF_LEVEL);
    Log_SetFormat(LOG_FMT_LEVEL_STR);
    if (g_flagUart1Prefix != WLR_Off) {
        Log_RegisterOutput(Usart_NorLogPrint);
    } else {
        Log_RegisterOutput(Usart_DmaLogPrint);
    }
    g_flagUart1Send = WLR_Idle;
}

static void Get_SystemTime(char *time_buf, uint16_t buf_size, uint16_t *ms)
{
    uint32_t tick = HAL_GetTick();
    *ms = tick % 1000;
    uint32_t sec = tick / 1000;

    uint32_t hour = sec / 3600;
    uint32_t min = (sec % 3600) / 60;
    sec = sec % 60;

    snprintf(time_buf, buf_size, "%02d:%02d:%02d",
             (int)(hour % 24), (int)min, (int)sec);
}

void Log_Print(uint32_t level, const char *func, uint32_t line, const char *format, ...)
{
    if (level >= LOG_LEVEL_BOTTOM || level < g_log_level)
        return;

    char    log_buf[CONFIG_LOG_BUF_SIZE] = {0};
    va_list args;
    int     idx = 0;

    if (g_log_format & LOG_FMT_LEVEL_STR) {
        static const char *level_str[] = {"DEBUG", "INFO ", "WARN", "ERROR"};
        idx += snprintf(log_buf + idx, sizeof(log_buf) - idx, "[%s]", level_str[level]);
    }

    if (g_log_format & LOG_FMT_TIME_STAMP) {
        char     time_buf[32];
        uint16_t ms = 0;
        Get_SystemTime(time_buf, sizeof(time_buf), &ms);
        idx += snprintf(log_buf + idx, sizeof(log_buf) - idx, "[%s.%03d]", time_buf, ms);
    }

    if (g_log_format & LOG_FMT_FUNC_LINE) {
        char short_func[20] = {0};
        strncpy(short_func, func, sizeof(short_func) - 1);
        idx += snprintf(log_buf + idx, sizeof(log_buf) - idx, "[%s:%d]", short_func, (int)line);
    }

    va_start(args, format);
    int len = vsnprintf(log_buf + idx, sizeof(log_buf) - idx, format, args);
    va_end(args);

    if (len < 0) {
        idx += snprintf(log_buf + idx, sizeof(log_buf) - idx, "[LOG FORMAT ERROR]");
    } else if (len > 0) {
        idx += len;
    }

    if (idx < CONFIG_LOG_BUF_SIZE - 2) {
        snprintf(log_buf + idx, sizeof(log_buf) - idx, "%s", LOG_NEWLINE_SIGN);
        idx += strlen(LOG_NEWLINE_SIGN);
    }

    g_log_output((uint8_t *)log_buf, (uint16_t)idx);
}

/*********************************************END OF FILE**********************/