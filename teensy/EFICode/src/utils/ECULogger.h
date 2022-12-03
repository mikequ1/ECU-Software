#ifndef LOGGER_H
#define LOGGER_H

#include "Arduino.h"

class Logger {
public:
    enum Level {
        STREAM,
        DEBUG,
        INFO,
        ERROR,
        CRITICAL
    };

    static Level get_level();
    static void set_level(Level level);

    static void _stream(const char* fmt, ...);
    static void _debug(const char* fmt, ...);
    static void _info(const char* fmt, ...);
    static void _error(const char* fmt, ...);
    static void _critical(const char* fmt, ...);

    static void log(Level level, const char* fmt, va_list argp);

    static Logger& get();  // return reference to Logger instance

private:
    Logger();
    Logger(const Logger&);
    void out(Level level, const char* fmt, va_list argp);
    Level _level;
};

#ifndef LOGC
#define LOGC(...) CRITICAL(__VA_ARGS__, "")
#define CRITICAL(fmt, ...) \
    Logger::_critical("%s:%i " fmt " %s", __func__, __LINE__, __VA_ARGS__)
#endif

#ifndef LOGE
#define LOGE(...) ERROR(__VA_ARGS__, "")
#define ERROR(fmt, ...) \
    Logger::_error("%s:%i " fmt " %s", __func__, __LINE__, __VA_ARGS__)
#endif

#ifndef LOGI
#define LOGI(...) INFO(__VA_ARGS__, "")
#define INFO(fmt, ...) \
    Logger::_info("%s:%i " fmt " %s", __func__, __LINE__, __VA_ARGS__)
#endif

#ifndef LOGD
#define LOGD(...) DEBUG(__VA_ARGS__, "")
#define DEBUG(fmt, ...) \
    Logger::_debug("%s:%i " fmt " %s", __func__, __LINE__, __VA_ARGS__)
#endif

#ifndef LOGS
#define LOGS(...) STREAM(__VA_ARGS__, "")
#define STREAM(fmt, ...) \
    Logger::_stream("%s:%i " fmt " %s", __func__, __LINE__, __VA_ARGS__)
#endif

#endif