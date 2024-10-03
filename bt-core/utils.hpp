#pragma once

#include <string>
#include <format>
#include <string_view>

enum LogType { LOG_INFO, LOG_ERROR, LOG_WARNING, LOG_DEBUG, LOG_TRACE };

void Log(LogType type, std::string msg);

// pass c++ style format string
template <typename... Args>
void LogInfo(std::string_view rt_fmt_str, Args &&...args) {
    Log(LOG_INFO, std::vformat(rt_fmt_str, std::make_format_args(args...)));
}

template <typename... Args>
void LogError(std::string_view rt_fmt_str, Args &&...args) {
    Log(LOG_ERROR, std::vformat(rt_fmt_str, std::make_format_args(args...)));
}

template <typename... Args>
void LogWarning(std::string_view rt_fmt_str, Args &&...args) {
    Log(LOG_WARNING, std::vformat(rt_fmt_str, std::make_format_args(args...)));
}

template <typename... Args>
void LogDebug(std::string_view rt_fmt_str, Args &&...args) {
    Log(LOG_DEBUG, std::vformat(rt_fmt_str, std::make_format_args(args...)));
}

template <typename... Args>
void LogTrace(std::string_view rt_fmt_str, Args &&...args) {
    Log(LOG_TRACE, std::vformat(rt_fmt_str, std::make_format_args(args...)));
}

namespace utils {
std::string UnixTimeToUTC(long long unixTime);
}