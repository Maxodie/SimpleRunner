#include "Core/Core.hpp"
#include "Log/Log.hpp"

namespace Log
{

Logger Logger::s_instance;

Logger& Logger::Get()
{
    return s_instance;
}

void Logger::LoggerGetTime()
{
    time(&m_currentTime);
    m_time = std::localtime(&m_currentTime);
}

void Logger::PrefixLog(const char* const prefix, Log::LogLevel level)
{
    LoggerGetTime();

#ifndef NO_ANSI
    std::cout << colors[static_cast<int>(level)];
#endif

    std::printf("[%d:%d:%d][%s_%s] ",
        m_time->tm_hour,
        m_time->tm_min,
        m_time->tm_sec,
        prefix,
        type[static_cast<int>(level)]);
}

void Logger::SuffixLog()
{
    std::cout << "\n" << colors[static_cast<int>(Log::LogLevel::LOG_DEBUG)];
}

}
