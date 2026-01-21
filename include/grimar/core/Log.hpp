// ~ Grimar Engine ~
#pragma once

#include <string_view>

namespace grimar::core {
    enum class LogLevel : unsigned char {
        Trace,
        Info,
        Warn,
        Error
    };

    // Set runtime minimum level (messages below are ignored at runtime).
    // Note: compile-time filtering may still remove calls entirely.
    void SetLogLevel(LogLevel level) noexcept;
    LogLevel GetLogLevel() noexcept;

    // Core Logging Function (Dependency-free)
    // msg is treated as already-formatted text.
    void Log(LogLevel level,
             const char* file,
             int line,
             std::string_view msg ) noexcept;

}

// -------------------------
// Compile-time log filtering
// -------------------------
// Debug:   enable Trace+
// Release: enable Warn+ by default

#if !defined(NDEBUG)
#define GRIMAR_LOG_COMPILETIME_LEVEL ::grimar::core::LogLevel::Trace
#else
#define GRIMAR_LOG_COMPILETIME_LEVEL ::grimar::core::LogLevel::Warn
#endif

// Helper: compare levels at compile time
#define GRIMAR_LOG_LEVEL_GE(a, b) (static_cast<unsigned>(a) >= static_cast<unsigned>(b))


// Macro sugar (file/line captured)
// These macros compile out if below GRIMAR_LOG_COMPILETIME_LEVEL.
#define GRIMAR_LOG_TRACE(msg)                                                     \
    do {                                                                          \
        if constexpr (GRIMAR_LOG_LEVEL_GE(::grimar::core::LogLevel::Trace,        \
                                          GRIMAR_LOG_COMPILETIME_LEVEL)) {       \
            ::grimar::core::Log(::grimar::core::LogLevel::Trace, __FILE__,        \
                                __LINE__, (msg));                                \
        }                                                                         \
    } while (0)

#define GRIMAR_LOG_INFO(msg)                                                      \
    do {                                                                          \
        if constexpr (GRIMAR_LOG_LEVEL_GE(::grimar::core::LogLevel::Info,         \
                                          GRIMAR_LOG_COMPILETIME_LEVEL)) {       \
            ::grimar::core::Log(::grimar::core::LogLevel::Info, __FILE__,         \
                                __LINE__, (msg));                                \
        }                                                                         \
    } while (0)

#define GRIMAR_LOG_WARN(msg)                                                      \
    do {                                                                          \
        if constexpr (GRIMAR_LOG_LEVEL_GE(::grimar::core::LogLevel::Warn,         \
                                          GRIMAR_LOG_COMPILETIME_LEVEL)) {       \
            ::grimar::core::Log(::grimar::core::LogLevel::Warn, __FILE__,         \
                                __LINE__, (msg));                                \
        }                                                                         \
    } while (0)

#define GRIMAR_LOG_ERROR(msg)                                                     \
    do {                                                                          \
        if constexpr (GRIMAR_LOG_LEVEL_GE(::grimar::core::LogLevel::Error,        \
                                          GRIMAR_LOG_COMPILETIME_LEVEL)) {       \
            ::grimar::core::Log(::grimar::core::LogLevel::Error, __FILE__,        \
                                __LINE__, (msg));                                \
        }                                                                         \
    } while (0)