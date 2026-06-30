// ~ Grimar Engine ~

#include "grimar/platform/FileSystem.hpp"

#include <cstdlib>
#include <exception>
#include <filesystem>

#include "grimar/core/Log.hpp"

namespace grimar::platform {
    namespace {
        [[nodiscard]] std::string NormalizeExistingPath(const std::filesystem::path& path) {
            return std::filesystem::absolute(path).lexically_normal().string();
        }
    }

    bool IsAbsolutePath(const std::string& path) noexcept {
        try {
            return std::filesystem::path(path).is_absolute();
        } catch (const std::exception&) {
            return false;
        }
    }

    bool FileExists(const std::string& path) noexcept {
        try {
            return std::filesystem::exists(std::filesystem::path(path));
        } catch (const std::exception&) {
            return false;
        }
    }

    std::string CurrentWorkingDirectory() noexcept {
        try {
            return std::filesystem::current_path().string();
        } catch (const std::exception&) {
            return {};
        }
    }

    std::string UserDocumentsDirectory() noexcept {
        const char* userProfile = std::getenv("USERPROFILE");
        if (userProfile && userProfile[0] != '\0') {
            return JoinPath(userProfile, "Documents");
        }

        const char* home = std::getenv("HOME");
        if (home && home[0] != '\0') {
            return JoinPath(home, "Documents");
        }

        return "GrimarEditor";
    }

    std::string JoinPath(const std::string& lhs,
                         const std::string& rhs) noexcept {
        try {
            return (std::filesystem::path(lhs) / std::filesystem::path(rhs)).string();
        } catch (const std::exception&) {
            if (lhs.empty()) {
                return rhs;
            }
            if (rhs.empty()) {
                return lhs;
            }
            return lhs + "/" + rhs;
        }
    }

    std::string ParentPath(const std::string& path) noexcept {
        try {
            return std::filesystem::path(path).parent_path().string();
        } catch (const std::exception&) {
            return {};
        }
    }

    std::string ResolveExistingPath(const std::string& path) noexcept {
        if (path.empty()) {
            return {};
        }

        try {
            const std::filesystem::path requested{path};
            if (std::filesystem::exists(requested)) {
                return NormalizeExistingPath(requested);
            }

            if (requested.is_absolute()) {
                return {};
            }

            auto current = std::filesystem::current_path();
            while (!current.empty()) {
                const auto candidate = current / requested;
                if (std::filesystem::exists(candidate)) {
                    return NormalizeExistingPath(candidate);
                }

                const auto parent = current.parent_path();
                if (parent == current) {
                    break;
                }
                current = parent;
            }
        } catch (const std::exception&) {
            return {};
        }

        return {};
    }

    std::string ResolveExistingPathRelativeTo(const std::string& basePath,
                                              const std::string& path) noexcept {
        if (path.empty()) {
            return {};
        }

        try {
            const std::filesystem::path requested{path};
            if (requested.is_absolute()) {
                return std::filesystem::exists(requested) ? NormalizeExistingPath(requested) : std::string{};
            }

            if (!basePath.empty()) {
                std::filesystem::path base{basePath};
                if (!std::filesystem::is_directory(base)) {
                    base = base.parent_path();
                }

                if (!base.empty()) {
                    const auto candidate = base / requested;
                    if (std::filesystem::exists(candidate)) {
                        return NormalizeExistingPath(candidate);
                    }
                }
            }
        } catch (const std::exception&) {
            return {};
        }

        return ResolveExistingPath(path);
    }

    bool EnsureDirectory(const std::string& path) noexcept {
        if (path.empty()) {
            return false;
        }

        try {
            std::error_code ec{};
            std::filesystem::create_directories(std::filesystem::path(path), ec);
            if (ec) {
                GRIMAR_LOG_ERROR("EnsureDirectory failed: {}", ec.message());
                return false;
            }

            return std::filesystem::is_directory(std::filesystem::path(path));
        } catch (const std::exception&) {
            GRIMAR_LOG_ERROR("EnsureDirectory failed: filesystem exception");
            return false;
        }
    }

    bool EnsureParentDirectory(const std::string& path) noexcept {
        if (path.empty()) {
            return false;
        }

        try {
            const auto parent = std::filesystem::path(path).parent_path();
            if (parent.empty()) {
                return true;
            }

            return EnsureDirectory(parent.string());
        } catch (const std::exception&) {
            GRIMAR_LOG_ERROR("EnsureParentDirectory failed: filesystem exception");
            return false;
        }
    }
}
