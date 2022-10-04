#include "Logger.h"
#include <ctime>
#include <sstream>

#if VS_OUT
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#undef WIN32_LEAN_AND_MEAN
#include "debugapi.h"
#endif // VS_Out

#include <iostream>

void Logger::Logging_Helpers::MergeFinalMessage(const EzString& prefix, const EzString& message, const EzString& postfix, EzString& finalMessageOut)
{
    finalMessageOut = string_format("%s%s%s", prefix.c_str(), message.c_str(), postfix.c_str());
}

void Logger::Logging_Helpers::GetFormatedDateTime(EzString& dateTimeOut)
{
    time_t now = std::time(0);
    std::tm ltm{};
    localtime_s(&ltm, &now);
    std::stringstream time;
    time << ltm.tm_year + 1900 << "\\" << ltm.tm_mon + 1 << "\\" << ltm.tm_mday << " " << ltm.tm_hour << ":" << ltm.tm_min <<
        ":" << ltm.tm_sec << " - ";
    dateTimeOut = time.str().c_str();
}

#if VS_OUT
void Logger::Logging_Helpers::VSDebugOut(const char* message)
{
    OutputDebugStringA(message);
}
#endif

void Logger::Logging_Helpers::ConsoleOut(const char* message)
{
    std::cout << message;
}