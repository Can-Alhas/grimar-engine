// ~ Grimar Engine ~

#include "grimar/core/Log.hpp"

#include <atomic>
#include <cstdio>
#include <mutex>

namespace grimar::core {
    static std::atomic<LogLevel> g_minLevel{LogLevel::Trace};
    static std::mutex g_logMutex;

    static const char* ToString(LogLevel level) noexcept {
        switch (level) {
            case LogLevel::Trace : return "TRACE";
            case LogLevel::Info  : return "INFO";
            case LogLevel::Warn  : return "WARN";
            case LogLevel::Error : return "ERROR";
            default:               return "UNKWN";
        }
    }

    void SetLogLevel(LogLevel level) noexcept {
        g_minLevel.store(level, std::memory_order_relaxed);
    }

    LogLevel GetLogLevel() noexcept {
        return g_minLevel.load(std::memory_order_relaxed);
    }

    void Log(LogLevel level,
             const char* file,
             int line,
             std::string_view msg) noexcept
    {

        // Runtime filter (compile-time filter may already remove calls)
        const auto minLevel = g_minLevel.load(std::memory_order_relaxed);
        if (static_cast<unsigned>(level) < static_cast<unsigned>(minLevel)) {
            return;
        }

        // Minimal thread safety for readable logs
        std::scoped_lock lock(g_logMutex);

        // stderr for Warn/Error, stdout for Trace/Info
        FILE* out = (level == LogLevel::Warn || level == LogLevel::Error) ? stderr : stdout;


        std::fprintf(out, "[Grimar][%s] %.*s\n at %s:%d\n",
                     ToString(level),
                     static_cast<int>(msg.size()), msg.data(),
                     file, line);
        std::fflush(out);
    }
}
