#include <iostream>
#include <chrono>
#include <iomanip>

#include <SDL3/SDL_messagebox.h>

#include <LR1-remake/util/log.hpp>

using namespace std;
using namespace std::chrono;

namespace LR1_Remake {
    const array<LogLevelInfo, numLogLevels> Logger::messageBoxTypes = {
        {
            {.messageBoxType = SDL_MESSAGEBOX_INFORMATION, .name = "Debug"},
            {.messageBoxType = SDL_MESSAGEBOX_INFORMATION, .name = "Info"},
            {.messageBoxType = SDL_MESSAGEBOX_WARNING, .name = "Warning"},
            {.messageBoxType = SDL_MESSAGEBOX_ERROR, .name = "Error"},
            {.messageBoxType = SDL_MESSAGEBOX_ERROR, .name = "Fatal"}
        }
    };

    int LogBuf::overflow(const int ch) {
        if (ch != EOF) {
            if (ch == '\n') {
                logger->log(level, str);
                str.clear();
            }
            else if (ch != '\r') str.push_back(static_cast<char>(ch));
        }
        return ch;
    }

    void Logger::log(const LogLevel level, const std::string& msg) const {
        const auto&[messageBoxType, name] = messageBoxTypes.at(static_cast<size_t>(level));

        const time_t t = system_clock::to_time_t(system_clock::now());
        std::ostream& out = level > LogLevel::Warning ? cerr : cout;
        out << '[' << put_time(localtime(&t), "%H:%M:%S") << "][" << name << "] " << msg << endl;

        if (level >= minLevel) SDL_ShowSimpleMessageBox(messageBoxType, name.c_str(), msg.c_str(), window);
    }
}
