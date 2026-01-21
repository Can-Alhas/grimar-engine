// Grimar Engine
#pragma once

#include <cstdlib> // std::abort

// Debug tespiti: MSVC/clang/gss uyumlu
#if !defined(NDEBUG)
      #define GRIMAR_DEBUG 1
#else
      #define GRIMAR_DEBUG 0
#endif

namespace grimar::core {

    /// Assertion failure handler (out-of-line).
    /// msg can be nullptr.
    [[noreturn]] void AssertFail(const char* expr,
                                 const char* file,
                                 int line,
                                 const char* msg) noexcept;

    /// Trigger a debugger break if possible, otherwise abort.
    /// Kept as a function for consistent behavior across compilers.
    [[noreturn]] void DebugBreakAndAbort() noexcept;


}


#if GRIMAR_DEBUG

#define GRIMAR_ASSERT(expr)                                                     \
do {                                                                          \
if (!(expr)) {                                                              \
::grimar::core::AssertFail(#expr, __FILE__, __LINE__, nullptr);           \
}                                                                           \
} while (0)

#define GRIMAR_ASSERT_MSG(expr, msg)                                            \
do {                                                                          \
if (!(expr)) {                                                              \
::grimar::core::AssertFail(#expr, __FILE__, __LINE__, (msg));             \
}                                                                           \
} while (0)

#else

// Release: compile out completely (no evaluation side effects)
#define GRIMAR_ASSERT(expr)       do { (void)sizeof(expr); } while (0)
#define GRIMAR_ASSERT_MSG(expr, msg) do { (void)sizeof(expr); } while (0)

#endif