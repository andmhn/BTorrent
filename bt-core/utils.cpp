#include "utils.hpp"
#include <iostream>
#include <format>
#include <chrono>
#include <cmath>

static const char *logTypeStrings[] = {"INFO", "ERROR", "WARNING", "DEBUG", "TRACE"};

void Log(LogType type, std::string msg) {
    std::cout << "[" << logTypeStrings[type] << "]: " << msg << std::endl;
}

namespace utils {
// convert to readable date
std::string UnixTimeToUTC(long long unixTime) {
    return std::format("{} UTC", std::chrono::sys_seconds{std::chrono::seconds(unixTime)});
}


constexpr long long KB = 1024;
constexpr long long MB = 1024 * KB;
constexpr long long GB = 1024 * MB;
constexpr long long TB = 1024 * GB;

static double _PreciseDivision(long long l, long long r) {
    long long precision = 100;
    return std::floor((double(l) / double(r)) * precision) / precision ;
}

std::string BytesToString(long long bytes) {
    if (bytes < KB) {
        return std::format("{} B", bytes);
    }
    // KB
    else if (bytes >= KB && bytes < MB) {
        return std::format("{} KB", _PreciseDivision(bytes, KB));
    }
    // MB
    else if (bytes >= MB && bytes < GB) {
        return std::format("{} MB", _PreciseDivision(bytes, MB));
    }
    // GB
    else if (bytes >= GB && bytes < TB) {
        return std::format("{} GB", _PreciseDivision(bytes, GB));
    }
    // TB
    else if (bytes >= TB) {
        return std::format("{} TB", _PreciseDivision(bytes, TB));
    } else {
        return "";
    }
}
} // namespace utils