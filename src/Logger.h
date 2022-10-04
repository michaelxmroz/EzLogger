#pragma once
#include <memory>
#include <string>
#include <stdexcept>

//This can be your custom string type, as long as it supports .c_str()
#define EzString std::string

//set to 0 if you do not want to include windows.h for the visual studio debug out, or console out
#define VS_OUT 1

namespace Logger
{
    namespace Logging_Helpers
    {
        void MergeFinalMessage(const EzString& prefix, const EzString& message, const EzString& postfix, EzString& finalMessageOut);
        void GetFormatedDateTime(EzString& dateTimeOut);
        void VSDebugOut(const char* message);
        void ConsoleOut(const char* message);

        // see https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
#pragma warning( push )
#pragma warning(disable : 4996)
        template<typename ... Args>
        std::string string_format(const EzString& format, Args&& ... args)
        {
            int size_s = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args) ...) + 1; // Extra space for '\0'
            if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
            auto size = static_cast<size_t>(size_s);

            auto buf = std::make_unique<char[]>(size);
            std::snprintf(buf.get(), size, format.c_str(), args ...);
            return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
        }
#pragma warning( pop )

    }

//Enums
//----------------------------------//
    enum class LogLevel : unsigned int
    {
        Info = 1,
        Warning = 2,
        Error = 4,
    };

    constexpr LogLevel operator|(LogLevel a, LogLevel b)
    {
        return static_cast<LogLevel>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
    }

    enum class LogVerbosityOptions : unsigned int
    {
        DateTime = 1,
        FileLine = 2,
    };

    constexpr LogVerbosityOptions operator|(LogVerbosityOptions a, LogVerbosityOptions b)
    {
        return static_cast<LogVerbosityOptions>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
    }

    enum class LogSeverity : unsigned int
    {
        None = 0,
        Errors = static_cast<unsigned int>(LogLevel::Error),
        Warnings = static_cast<unsigned int>(LogLevel::Warning) | static_cast<unsigned int>(LogLevel::Error),
        All = static_cast<unsigned int>(LogLevel::Info) | static_cast<unsigned int>(LogLevel::Warning) | static_cast<unsigned int>(LogLevel::Error),
    };

    enum class LogVerbosity : unsigned int
    {
        Minimal = 0,
        DateTime = static_cast<unsigned int>(LogVerbosityOptions::DateTime),
        FileLine = static_cast<unsigned int>(LogVerbosityOptions::FileLine),
        All = static_cast<unsigned int>(LogVerbosityOptions::DateTime) | static_cast<unsigned int>(LogVerbosityOptions::FileLine),
    };

    template<LogLevel level>
    static constexpr const char* LogLevelToString()
    {
        switch (level)
        {
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }

//Output Controllers
//----------------------------------//
#if VS_OUT
    class VSDebugOutput
    {
    public:
        template<LogLevel level>
        static void Output(const EzString& prefix, const EzString& message, const EzString& postfix)
        {
            EzString format, finalMessage;
            format = Logging_Helpers::string_format("%s: %s",LogLevelToString<level>(), message.c_str());
            Logging_Helpers::MergeFinalMessage(prefix, format, postfix, finalMessage);
            Logging_Helpers::VSDebugOut(finalMessage.c_str());
        };
    };
#endif

    class ConsoleOutput
    {
    public:
        template<LogLevel level>
        static void Output(const EzString& prefix, const EzString& message, const EzString& postfix)
        {
            EzString format, finalMessage;
            format = Logging_Helpers::string_format("%s: %s", LogLevelToString<level>(), message.c_str());
            Logging_Helpers::MergeFinalMessage(prefix, format, postfix, finalMessage);
            Logging_Helpers::ConsoleOut(finalMessage.c_str());
        };
    };

//Logging Boilerplate
//----------------------------------//
#pragma warning( push )
#pragma warning( disable : 4702 ) //disable unreachable code warning caused by the constexpr ifs
    template <LogSeverity severity, LogVerbosity verbosity, class output>
    class Logger
    {
    public:
        template <LogLevel level>
        static void Log(const EzString& message, const char* path, const int line)
        {
            if constexpr ((static_cast<unsigned int>(level) & static_cast<unsigned int>(severity)) == 0)
            {
                return;
            }

            EzString prefix("");
            EzString postfix("");

            if constexpr ((static_cast<unsigned int>(verbosity) & static_cast<unsigned int>(LogVerbosityOptions::DateTime)) != 0)
            {
                Logging_Helpers::GetFormatedDateTime(prefix);
            }

            if constexpr ((static_cast<unsigned int>(verbosity) & static_cast<unsigned int>(LogVerbosityOptions::FileLine)) != 0)
            {
                postfix = Logging_Helpers::string_format(" - [%s(%i)]\n", path, line);
            }

            output::Output<level>(prefix, message, postfix);
        }
    };
#pragma warning( pop )

//Compound loggers through template magic
//----------------------------------//
    template<LogLevel level, class T>
    void ResolveVariadicLogCall(const EzString& message, const char* path, const int line)
    {
        T::Log<level>(message, path, line);
    }

    template<LogLevel level, class T, class ... loggers>
    typename std::enable_if<sizeof ... (loggers) != 0, void>::type
        ResolveVariadicLogCall(const EzString& message, const char* path, const int line)
    {
        T::Log<level>(message, path, line);
        ResolveVariadicLogCall<level, loggers ... >(message, path, line);
    }

    template<class ... loggers>
    class CompoundLogger
    {
    public:
        template <LogLevel level>
        static void Log(const char* message, const char* path, const int line)
        {
            ResolveVariadicLogCall<level, loggers ... >(message, path, line);
        }
    };

/*
 In this section various loggers can be defined and compounded. A compound logger can contain any amount of loggers as template arguments.
 A different DefaultLogger could be defined for different build configs.
 The DefaultLogger is used by the default LOG_ macros.
 */
 //----------------------------------//
    typedef Logger<LogSeverity::All, LogVerbosity::All, VSDebugOutput> VSLogger;
    typedef Logger<LogSeverity::All, LogVerbosity::All, ConsoleOutput> ConsoleLogger;
    typedef CompoundLogger<VSLogger, ConsoleLogger> DefaultLogger;
}

#define LOG_INFO(message) \
{ \
 Logger::DefaultLogger::Log<Logger::LogLevel::Info>(message, __FILE__, __LINE__); \
}

#define LOG_WARNING(message) \
{ \
 Logger::DefaultLogger::Log<Logger::LogLevel::Warning>(message, __FILE__, __LINE__); \
}

#define LOG_ERROR(message) \
{ \
 Logger::DefaultLogger::Log<Logger::LogLevel::Error>(message, __FILE__, __LINE__); \
}

