#pragma once

#include "Core/Core.hpp"
namespace Log
{

constexpr std::array type = {
    "DEBUG",
    "SUCCESS",
    "WARNING",
    "ERROR"
};

constexpr std::array colors = {
    "\x1b[0m",
    "\x1b[32m",
    "\x1b[1;33m",
    "\x1b[31m"
};

enum class LogLevel : uint8_t{
    LOG_DEBUG,
    LOG_SUCCESS,
    LOG_WARNING,
    LOG_ERROR,
    LOG_LEVEL_LEN
};

class Logger
{
public:
    template<typename ... TArgs>
    void LoggerLog(const char* const prefix, Log::LogLevel level, const char* const fmt, TArgs&&... args)
    {
        PrefixLog(prefix, level);
        LogMessage(level, fmt, args...);

        SuffixLog();
    }

    template<typename ... TArgs>
    void ContextLoggerLog(const char* const prefix, Log::LogLevel level, const char* const functionCtx, const char* fileCtx, uint32_t lineCtx, const char* const fmt, TArgs&&... args)
    {
        PrefixLog(prefix, level);

        std::printf("%s() from %s, line %d : ", functionCtx, fileCtx, lineCtx);
        LogMessage(level, fmt, args...);

        SuffixLog();
    }

    template<typename ... TArgs>
    void LogMessage(Log::LogLevel level, const char* const fmt, TArgs&&... args)
    {
        switch (level)
        {
        case Log::LogLevel::LOG_ERROR :
            std::printf(fmt, std::forward<TArgs>(args)...);
            break;
        default:
            std::printf(fmt, std::forward<TArgs>(args)...);
            break;
        }
    }

    static Logger& Get();

private:
    void LoggerGetTime();
    void PrefixLog(const char* const prefix, Log::LogLevel level);
    void SuffixLog();

private:
    time_t m_currentTime;
    std::tm* m_time;
    static Logger s_instance;
};

}

#ifdef SR_LOGGER
#   define CORE_LOG_DEBUG(msg, ...) Log::Logger::Get().LoggerLog("CORE", Log::LogLevel::LOG_DEBUG, msg, __VA_ARGS__)
#   define CORE_LOG_SUCCESS(msg, ...) Log::Logger::Get().LoggerLog("CORE", Log::LogLevel::LOG_SUCCESS, msg, __VA_ARGS__)
#   define CORE_LOG_WARNING(msg, ...) Log::Logger::Get().ContextLoggerLog("CORE", Log::LogLevel::LOG_WARNING, __FUNCTION__, __FILE__, __LINE__, msg, __VA_ARGS__)
#   define CORE_LOG_ERROR(msg, ...) Log::Logger::Get().ContextLoggerLog("CORE", Log::LogLevel::LOG_ERROR, __FUNCTION__, __FILE__, __LINE__, msg, __VA_ARGS__)

#   define CLIENT_LOG_DEBUG(msg, ...) Log::Logger::Get().LoggerLog("CLIENT", Log::LogLevel::LOG_DEBUG, msg, __VA_ARGS__)
#   define CLIENT_LOG_SUCCESS(msg, ...) Log::Logger::Get().LoggerLog("CLIENT", Log::LogLevel::LOG_SUCCESS, msg, __VA_ARGS__)
#   define CLIENT_LOG_WARNING(msg, ...) Log::Logger::Get().ContextLoggerLog("CLIENT", Log::LogLevel::LOG_WARNING, __FUNCTION__, __FILE__, __LINE__, msg, __VA_ARGS__)
#   define CLIENT_LOG_ERROR(msg, ...) Log::Logger::Get().ContextLoggerLog("CLIENT", Log::LogLevel::LOG_ERROR, __FUNCTION__, __FILE__, __LINE__, msg, __VA_ARGS__)

#else
#   define CORE_LOG_DEBUG(msg, ...)
#   define CORE_LOG_SUCCESS(msg, ...)
#   define CORE_LOG_WARNING(msg, ...)
#   define CORE_LOG_ERROR(msg, ...)

#   define CLIENT_LOG_DEBUG(msg, ...)
#   define CLIENT_LOG_SUCCESS(msg, ...)
#   define CLIENT_LOG_WARNING(msg, ...)
#   define CLIENT_LOG_ERROR(msg, ...)
#endif
