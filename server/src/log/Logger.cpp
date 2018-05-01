#include "Logger.h"

#include <chrono>
#include "Colour.h"

namespace log {

    void Logger::logMessage(std::string message) {
        std::lock_guard<std::mutex> guard(loggerMutex);

        auto time = std::chrono::system_clock::now();
        std::time_t timeT = std::chrono::system_clock::to_time_t(time);

        *output << "LOG: " << std::ctime(&timeT) << "  " << message << std::endl;
    }

    void Logger::logError(std::string error) {
        std::lock_guard<std::mutex> guard(loggerMutex);

        auto time = std::chrono::system_clock::now();
        std::time_t timeT = std::chrono::system_clock::to_time_t(time);

        *output << Colour(Colour::Error) << "ERROR: " << std::ctime(&timeT) << "  " << error
                << Colour(Colour::White) << std::endl;
    }

    void Logger::logDebug(std::string debug) {
        if (!shouldShowDebug)
            return;

        std::lock_guard<std::mutex> guard(loggerMutex);

        auto time = std::chrono::system_clock::now();
        std::time_t timeT = std::chrono::system_clock::to_time_t(time);

        *output << Colour(Colour::Warning) << "DEBUG: " << std::ctime(&timeT) << "  " << debug
                << Colour(Colour::White) << std::endl;
    }

    void Logger::setShouldShowDebug(bool shouldShowDebug) {
        Logger::shouldShowDebug = shouldShowDebug;
    }

    void Logger::setOutputStream(std::ostream *output) {
        Logger::output = output;
    }

} // end log namespace