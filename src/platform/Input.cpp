// ~ Grimar Engine ~

#include <grimar/platform/Input.hpp>

namespace grimar::platform {

    void Input::BeginFrame() noexcept {
        m_prev = m_curr;
    }

    void Input::SetKeyDown(Key key, bool down) noexcept {
        m_curr[static_cast<size_t>(key)] = down ? 1 : 0;
    }

    bool Input::IsKeyDown(Key key) const noexcept {
        return m_curr[static_cast<size_t>(key)] != 0;
    }

    bool Input::WasKeyPressed(Key key) const noexcept {
        const auto i = static_cast<size_t>(key);
        return m_curr[i] && !m_prev[i];
    }

    bool Input::WasKeyReleased(Key key) const noexcept {
        const auto i = static_cast<size_t>(key);
        return !m_curr[i] && m_prev[i];
    }
}
