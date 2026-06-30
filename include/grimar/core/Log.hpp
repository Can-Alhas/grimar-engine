// ~ Grimar Engine ~
#pragma once

#include <string_view>
#include <format> // Required for std::format_string and std::format

namespace grimar::core {
    enum class LogLevel : unsigned char {
        Trace,
        Info,
        Warn,
        Error
    };

    void SetLogLevel(LogLevel level) noexcept;
    LogLevel GetLogLevel() noexcept;

    // Core Logging Function (Dependency-free)
    void Log(LogLevel level,
             const char* file,
             int line,
             std::string_view msg) noexcept;

    // Helper function providing compile-time format string validation.
    // Validates the '{}' placeholder layout at compile time via template arguments.
    template<typename... Args>
    [[nodiscard]] inline std::string FormatLog(std::format_string<Args...> fmt, Args&&... args) {
        return std::format(fmt, std::forward<Args>(args)...);
    }
}

// -------------------------
// Compile-time log filtering
// -------------------------
#if !defined(NDEBUG)
#define GRIMAR_LOG_COMPILETIME_LEVEL ::grimar::core::LogLevel::Trace
#else
#define GRIMAR_LOG_COMPILETIME_LEVEL ::grimar::core::LogLevel::Warn
#endif

// Helper: compare levels at compile time
#define GRIMAR_LOG_LEVEL_GE(a, b) (static_cast<unsigned>(a) >= static_cast<unsigned>(b))


// Macro sugar (file/line captured)
// Macros route through the FormatLog helper function.
// This preserves compile-time safety and prevents static analysis (Clangd) errors.

#define GRIMAR_LOG_TRACE(fmt, ...)                                                \
    do {                                                                          \
        if constexpr (GRIMAR_LOG_LEVEL_GE(::grimar::core::LogLevel::Trace,         \
                                          GRIMAR_LOG_COMPILETIME_LEVEL)) {       \
            ::grimar::core::Log(::grimar::core::LogLevel::Trace, __FILE__,        \
                                __LINE__, ::grimar::core::FormatLog(fmt, ##__VA_ARGS__)); \
        }                                                                         \
    } while (0)

#define GRIMAR_LOG_INFO(fmt, ...)                                                 \
    do {                                                                          \
        if constexpr (GRIMAR_LOG_LEVEL_GE(::grimar::core::LogLevel::Info,          \
                                          GRIMAR_LOG_COMPILETIME_LEVEL)) {       \
            ::grimar::core::Log(::grimar::core::LogLevel::Info, __FILE__,         \
                                __LINE__, ::grimar::core::FormatLog(fmt, ##__VA_ARGS__)); \
        }                                                                         \
    } while (0)

#define GRIMAR_LOG_WARN(fmt, ...)                                                 \
    do {                                                                          \
        if constexpr (GRIMAR_LOG_LEVEL_GE(::grimar::core::LogLevel::Warn,          \
                                          GRIMAR_LOG_COMPILETIME_LEVEL)) {       \
            ::grimar::core::Log(::grimar::core::LogLevel::Warn, __FILE__,         \
                                __LINE__, ::grimar::core::FormatLog(fmt, ##__VA_ARGS__)); \
        }                                                                         \
    } while (0)

#define GRIMAR_LOG_ERROR(fmt, ...)                                                \
    do {                                                                          \
        if constexpr (GRIMAR_LOG_LEVEL_GE(::grimar::core::LogLevel::Error,         \
                                          GRIMAR_LOG_COMPILETIME_LEVEL)) {       \
            ::grimar::core::Log(::grimar::core::LogLevel::Error, __FILE__,        \
                                __LINE__, ::grimar::core::FormatLog(fmt, ##__VA_ARGS__)); \
        }                                                                         \
    } while (0)