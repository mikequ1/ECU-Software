#include "ECULogger.h"
#include <Arduino.h>

Logger::Logger() : _level(STREAM) {
}

void Logger::set_level(Level level) {
    get()._level = level;
}

Logger::Level Logger::get_level() {
    return get()._level;
}

void Logger::_stream(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log(STREAM, fmt, args);
    va_end(args);
}

void Logger::_debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log(DEBUG, fmt, args);
    va_end(args);
}

void Logger::_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log(INFO, fmt, args);
    va_end(args);
}

void Logger::_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log(ERROR, fmt, args);
    va_end(args);
}

void Logger::_critical(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log(CRITICAL, fmt, args);
    va_end(args);
}

void Logger::log(Level level, const char* fmt, va_list argp) {
    if (get()._level <= level) {
        get().out(level, fmt, argp);
    }
}


Logger& Logger::get() {
    static Logger logger;
    return logger;
}


void Logger::out(Level level, const char* fmt, va_list argp) {
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), fmt, argp);
    char tag[16];
    switch (level) {
        case Logger::Level::STREAM:
            strcpy(tag,"STREAM");
            break;
        case Logger::Level::DEBUG:
            strcpy(tag,"DEBUG");
            break;
        case Logger::Level::INFO:
            strcpy(tag,"INFO");
            break;
        case Logger::Level::ERROR:
            strcpy(tag,"ERROR");
            break;
        case Logger::Level::CRITICAL:
            strcpy(tag,"CRITICAL");
            break;
    }
    // print log level tag

    char out[320];
    sprintf(out, "[%s] %s", tag, buffer);
    Serial.write(out);
}