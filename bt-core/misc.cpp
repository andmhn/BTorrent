#include "misc.hpp"
#include <iostream>

static const char *logTypeStrings[] = {"INFO", "ERROR", "WARNING", "DEBUG", "TRACE"};

void Log(LogType type, std::string msg) {
    std::cout << "[" << logTypeStrings[type] << "]: " << msg << std::endl;
}