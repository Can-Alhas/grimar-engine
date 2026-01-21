// ~ Grimar Engine ~
// Grimar Engine
// SPDX-License-Identifier: MIT

#include <grimar/core/Assert.hpp>
#include <grimar/core/Log.hpp>

#include <cstdlib>

#if defined(_MSC_VER)
  #include <intrin.h>
#endif

namespace grimar::core {

    [[noreturn]] void DebugBreakAndAbort() noexcept {
#if defined(_MSC_VER)
        __debugbreak();
#elif defined(__clang__) || defined(__GNUC__)
        __builtin_trap();
#else
        std::abort();
#endif
        std::abort();
    }

    [[noreturn]] void AssertFail(const char* expr,
                                const char* file,
                                int line,
                                const char* msg) noexcept
    {
        // Route all assert output through the logging system
        if (msg && msg[0] != '\0') {
            Log(LogLevel::Error, file, line,
                std::string_view{"ASSERT FAILED: "} );
            Log(LogLevel::Error, file, line, expr);
            Log(LogLevel::Error, file, line, msg);
        } else {
            // Single line is nicer; keeping simple without formatting libs
            Log(LogLevel::Error, file, line, "ASSERT FAILED");
            Log(LogLevel::Error, file, line, expr);
        }

        DebugBreakAndAbort();
    }

} // namespace grimar::core
