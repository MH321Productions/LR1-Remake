#ifndef LR1_REMAKE_LOG_HPP
#define LR1_REMAKE_LOG_HPP

#include <iostream>
#include <cinttypes>
#include <array>

struct SDL_Window;

namespace LR1_Remake {
    class Logger;

    enum class LogLevel : uint8_t {
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };
    constexpr size_t numLogLevels = static_cast<size_t>(LogLevel::Fatal) + 1;

    struct LogLevelInfo {
        uint32_t messageBoxType;
        std::string name;
    };

    class LogBuf : public virtual std::streambuf {
        public:
            LogBuf(const LogLevel level, Logger* logger) : level(level), logger(logger) {}

        protected:
            int overflow(int ch) override;

        private:
            const LogLevel level;
            std::string str;
            Logger* logger;
    };

    class Logger {
        friend class Main;

    public:
        Logger() :
        debug(&bufDebug), info(&bufInfo), warning(&bufWarning), error(&bufError), fatal(&bufFatal), minLevel(LogLevel::Error),
        window(nullptr), bufDebug(LogLevel::Debug, this), bufInfo(LogLevel::Info, this), bufWarning(LogLevel::Warning, this), bufError(LogLevel::Error, this), bufFatal(LogLevel::Fatal, this) {}

        std::ostream debug;
        std::ostream info;
        std::ostream warning;
        std::ostream error;
        std::ostream fatal;
        LogLevel minLevel;

        void log(LogLevel level, const std::string& msg) const;

    private:
        static const std::array<LogLevelInfo, numLogLevels> messageBoxTypes;

        SDL_Window* window;

        LogBuf bufDebug;
        LogBuf bufInfo;
        LogBuf bufWarning;
        LogBuf bufError;
        LogBuf bufFatal;

    };
}

#endif //LR1_REMAKE_LOG_HPP
