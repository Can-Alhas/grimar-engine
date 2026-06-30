// ~ Grimar Engine ~

#pragma once

#include <string>

namespace grimar::platform {

    [[nodiscard]] bool IsAbsolutePath(const std::string& path) noexcept;
    [[nodiscard]] bool FileExists(const std::string& path) noexcept;
    [[nodiscard]] std::string CurrentWorkingDirectory() noexcept;
    [[nodiscard]] std::string UserDocumentsDirectory() noexcept;
    [[nodiscard]] std::string JoinPath(const std::string& lhs,
                                       const std::string& rhs) noexcept;
    [[nodiscard]] std::string ParentPath(const std::string& path) noexcept;
    [[nodiscard]] std::string ResolveExistingPath(const std::string& path) noexcept;
    [[nodiscard]] std::string ResolveExistingPathRelativeTo(const std::string& basePath,
                                                            const std::string& path) noexcept;
    [[nodiscard]] bool EnsureDirectory(const std::string& path) noexcept;
    [[nodiscard]] bool EnsureParentDirectory(const std::string& path) noexcept;
}
