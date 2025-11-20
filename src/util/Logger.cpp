#include "qs/Logger.hpp"

#include <iostream>

namespace qs
{

    void Logger::log(LogLevel level, const std::string &message)
    {
        const char *prefix = "";
        switch (level)
        {
        case LogLevel::Debug:
            prefix = "[DEBUG] ";
            break;
        case LogLevel::Info:
            prefix = "[INFO ] ";
            break;
        case LogLevel::Warn:
            prefix = "[WARN ] ";
            break;
        case LogLevel::Error:
            prefix = "[ERROR] ";
            break;
        }
        if (level == LogLevel::Error)
        {
            std::cerr << prefix << message << '\n';
        }
        else
        {
            std::cout << prefix << message << '\n';
        }
    }

} // namespace qs
