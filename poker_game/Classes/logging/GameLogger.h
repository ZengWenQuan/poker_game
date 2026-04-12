#ifndef POKER_GAME_GAME_LOGGER_H
#define POKER_GAME_GAME_LOGGER_H

#include <cstdarg>
#include <fstream>
#include <mutex>
#include <string>

// 简易文件+控制台日志器，支持等级前缀与可变参数格式化。
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

    // 获取全局唯一实例。
    static GameLogger& getInstance();

    // 启动时初始化日志文件，后续同时输出到控制台和文件。
    bool initialize();
    // 关闭文件句柄，结束写入。
    void shutdown();

    // 写入一条已格式化的日志。
    // level: 日志级别；file/line: 调用位置；message: 已格式化文本。
    void log(Level level, const char* file, int line, const std::string& message);
    // 写入一条带 printf 风格格式串的日志。
    // fmt: 格式模板；其余参数按 fmt 提供。
    void logf(Level level, const char* file, int line, const char* fmt, ...);

    const std::string& getLogFilePath() const { return _logFilePath; }
    bool isInitialized() const { return _initialized; }

private:
    GameLogger() = default;

    // 将枚举级别转换为字符串标签。
    std::string levelToString(Level level) const;
    // 从完整路径提取文件名。
    std::string baseName(const char* file) const;
    // 构造时间戳字符串；forFileName 为 true 时用于文件名。
    std::string buildTimestamp(bool forFileName) const;
    // 按 fmt 与 va_list 生成字符串。
    std::string vformat(const char* fmt, va_list args) const;

    bool _initialized = false; // 是否已打开日志文件
    std::string _logFilePath;  // 当前日志文件路径
    std::ofstream _stream;     // 文件输出流
    std::mutex _mutex;         // 保护并发写日志
};

#define GAME_LOG_DEBUG(fmt, ...) GameLogger::getInstance().logf(GameLogger::Level::Debug, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define GAME_LOG_INFO(fmt, ...) GameLogger::getInstance().logf(GameLogger::Level::Info, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define GAME_LOG_WARN(fmt, ...) GameLogger::getInstance().logf(GameLogger::Level::Warn, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define GAME_LOG_ERROR(fmt, ...) GameLogger::getInstance().logf(GameLogger::Level::Error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif
