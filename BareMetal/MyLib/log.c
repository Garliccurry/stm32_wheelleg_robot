#include "log.h"

/* 全局配置 */
static uint32_t s_log_fmt = LOG_FMT_LEVEL_STR | LOG_FMT_TIME_STAMP | LOG_FMT_FUNC_LINE;
static uint32_t s_log_level = CONFIG_LOG_DEF_LEVEL;
static logOutputFunc log_output = NULL;

static void Get_SystemTime(char *time_buf, uint16_t buf_size, uint16_t *ms);

/* 注册输出函数（默认使用串口） */
void log_RegisterOutput(logOutputFunc func)
{
    log_output = func ? func : Usart_LogPrint;
}

/* 设置日志格式 */
void log_SetFmt(uint32_t fmt)
{
    s_log_fmt = fmt;
}

/* 系统时间获取（基于HAL_GetTick） */
static void Get_SystemTime(char *time_buf, uint16_t buf_size, uint16_t *ms)
{
    uint32_t tick = HAL_GetTick(); // 获取系统运行时间（毫秒）
    *ms = tick % 1000;
    uint32_t sec = tick / 1000;

    uint32_t hour = sec / 3600;
    uint32_t min = (sec % 3600) / 60;
    sec = sec % 60;

    snprintf(time_buf, buf_size, "%02d:%02d:%02d",
             (int)(hour % 24), (int)min, (int)sec);
}

/* 核心日志函数 */
void log(uint32_t level, const char *func, uint32_t line, const char *fmt, ...)
{
    /* 级别过滤 */
    if (level >= LOG_LEVEL_BOTTOM || level < s_log_level)
        return;

    /* 缓冲区初始化 */
    char log_buf[CONFIG_LOG_BUF_SIZE] = {0};
    va_list args;
    int idx = 0;

    /* 级别字符串 */
    if (s_log_fmt & LOG_FMT_LEVEL_STR)
    {
        static const char *level_str[] = {"DEBUG", "INFO ", "ERROR"};
        idx += snprintf(log_buf + idx, sizeof(log_buf) - idx, "[%s]", level_str[level]);
    }

    /* 时间戳 */
    if (s_log_fmt & LOG_FMT_TIME_STAMP)
    {
        char time_buf[32];
        uint16_t ms = 0;
        Get_SystemTime(time_buf, sizeof(time_buf), &ms);
        idx += snprintf(log_buf + idx, sizeof(log_buf) - idx, "[%s.%03d]", time_buf, ms);
    }

    /* 函数名+行号 */
    if (s_log_fmt & LOG_FMT_FUNC_LINE)
    {
        char short_func[20] = {0};
        strncpy(short_func, func, sizeof(short_func) - 1);
        idx += snprintf(log_buf + idx, sizeof(log_buf) - idx, "[%s:%d]", short_func, (int)line);
    }

    /* 日志内容 */
    va_start(args, fmt);
    int len = vsnprintf(log_buf + idx, sizeof(log_buf) - idx, fmt, args);
    va_end(args);

    /* 处理格式化错误 */
    if (len < 0)
    {
        idx += snprintf(log_buf + idx, sizeof(log_buf) - idx, "[LOG FORMAT ERROR]");
    }
    else if (len > 0)
    {
        idx += len;
    }

    /* 添加换行符 */
    if (idx < CONFIG_LOG_BUF_SIZE - 2)
    {
        snprintf(log_buf + idx, sizeof(log_buf) - idx, "%s", LOG_NEWLINE_SIGN);
        idx += strlen(LOG_NEWLINE_SIGN);
    }

    /* 输出日志 */
    log_output((uint8_t *)log_buf, (uint16_t)idx);
}

void Test_LogFunctions(void)
{
    LOG_DEBUG("调试信息 - 变量值: %d", 100);
    LOG_INFO("系统初始化完成");
    LOG_ERROR("文件打开失败: %s", "test.log");
}

void Test_LogRunTimeDebug(void)
{
    static uint32_t u32Cnt = 0;
    u32Cnt++;
    LOG_DEBUG("系统运行中，%04d", u32Cnt);
}
/*********************************************END OF FILE**********************/