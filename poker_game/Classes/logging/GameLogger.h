#ifndef POKER_GAME_GAME_LOGGER_H
#define POKER_GAME_GAME_LOGGER_H

#include <cstdarg>
#include <fstream>
#include <mutex>
#include <string>

class GameLogger
{
public:
    enum class Level
    {
        Debug,
        Info,
        Warn,
        Error
    };

    static GameLogger& getInstance();

    bool initialize();
    void shutdown();

    void log(Level level, const char* file, int line, const std::string& message);
    void logf(Level level, const char* file, int line, const char* fmt, ...);

    const std::string& getLogFilePath() const { return _logFilePath; }
    bool isInitialized() const { return _initialized; }

private:
    GameLogger() = default;

    std::string levelToString(Level level) const;
    std::string baseName(const char* file) const;
    std::string buildTimestamp(bool forFileName) const;
    std::string vformat(const char* fmt, va_list args) const;

    bool _initialized = false;
    std::string _logFilePath;
    std::ofstream _stream;
    std::mutex _mutex;
};

#define GAME_LOG_DEBUG(fmt, ...) GameLogger::getInstance().logf(GameLogger::Level::Debug, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define GAME_LOG_INFO(fmt, ...) GameLogger::getInstance().logf(GameLogger::Level::Info, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define GAME_LOG_WARN(fmt, ...) GameLogger::getInstance().logf(GameLogger::Level::Warn, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define GAME_LOG_ERROR(fmt, ...) GameLogger::getInstance().logf(GameLogger::Level::Error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif
