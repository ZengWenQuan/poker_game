#include "GameLogger.h"
#include "platform/CCFileUtils.h"
#include <chrono>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

GameLogger& GameLogger::getInstance()
{
    static GameLogger instance;
    return instance;
}

bool GameLogger::initialize()
{
    std::string logFilePath;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_initialized)
        {
            return true;
        }

        auto* fileUtils = cocos2d::FileUtils::getInstance();
        const std::string logDir = fileUtils->getWritablePath() + "logs/";
        fileUtils->createDirectory(logDir);

        _logFilePath = logDir + "game_" + buildTimestamp(true) + ".log";
        _stream.open(_logFilePath, std::ios::out | std::ios::app);
        if (!_stream.is_open())
        {
            std::cerr << "Failed to open log file: " << _logFilePath << std::endl;
            return false;
        }

        _initialized = true;
        logFilePath = _logFilePath;
    }

    log(Level::Info, __FILE__, __LINE__, "Logger initialized. File: " + logFilePath);
    return true;
}

void GameLogger::shutdown()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_initialized)
    {
        return;
    }

    const std::string shutdownLine = "[" + buildTimestamp(false) + "] [INFO] [GameLogger.cpp] Logger shutting down";
    std::cout << shutdownLine << std::endl;
    _stream << shutdownLine << std::endl;
    _stream.flush();
    _stream.close();
    _initialized = false;
}

void GameLogger::log(Level level, const char* file, int line, const std::string& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    std::ostringstream oss;
    oss << "[" << buildTimestamp(false) << "] "
        << "[" << levelToString(level) << "] "
        << "[" << baseName(file) << ":" << line << "] "
        << message;

    const std::string lineText = oss.str();
    std::cout << lineText << std::endl;
    if (_initialized && _stream.is_open())
    {
        _stream << lineText << std::endl;
        _stream.flush();
    }
}

void GameLogger::logf(Level level, const char* file, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const std::string message = vformat(fmt, args);
    va_end(args);
    log(level, file, line, message);
}

std::string GameLogger::levelToString(Level level) const
{
    switch (level)
    {
    case Level::Debug: return "DEBUG";
    case Level::Info: return "INFO";
    case Level::Warn: return "WARN";
    case Level::Error: return "ERROR";
    }
    return "INFO";
}

std::string GameLogger::baseName(const char* file) const
{
    if (file == nullptr) return "unknown";
    const std::string path(file);
    const std::size_t pos = path.find_last_of("/\\");
    return pos == std::string::npos ? path : path.substr(pos + 1);
}

std::string GameLogger::buildTimestamp(bool forFileName) const
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tmValue;
#if defined(_WIN32)
    localtime_s(&tmValue, &time);
#else
    localtime_r(&time, &tmValue);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tmValue, forFileName ? "%Y%m%d_%H%M%S" : "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string GameLogger::vformat(const char* fmt, va_list args) const
{
    va_list argsCopy;
    va_copy(argsCopy, args);
    const int size = std::vsnprintf(nullptr, 0, fmt, argsCopy);
    va_end(argsCopy);

    if (size <= 0)
    {
        return "";
    }

    std::vector<char> buffer(static_cast<std::size_t>(size) + 1);
    std::vsnprintf(buffer.data(), buffer.size(), fmt, args);
    return std::string(buffer.data());
}
