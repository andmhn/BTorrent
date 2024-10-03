#include "utils.hpp"
#include <iostream>
#include <format>
#include <chrono>

static const char *logTypeStrings[] = {"INFO", "ERROR", "WARNING", "DEBUG", "TRACE"};

void Log(LogType type, std::string msg) {
    std::cout << "[" << logTypeStrings[type] << "]: " << msg << std::endl;
}

namespace utils {
// convert to readable date
std::string UnixTimeToUTC(long long unixTime) {
    return std::format("{} UTC", std::chrono::sys_seconds{std::chrono::seconds(unixTime)});
}

} // namespace utils