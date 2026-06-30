// ~ Grimar Engine ~

#include <grimar/platform/Input.hpp>

namespace grimar::platform {

    void Input::BeginFrame() noexcept {
        m_prev = m_curr;
        m_mousePrev = m_mouseCurr;
        m_mouseWheel = {};
    }

    void Input::SetKeyDown(Key key, bool down) noexcept {
        m_curr[static_cast<size_t>(key)] = down ? 1 : 0;
    }

    void Input::SetMousePosition(float x, float y) noexcept {
        m_mousePosition = {x, y};
    }

    void Input::SetMouseButtonDown(MouseButton button, bool down) noexcept {
        if (button == MouseButton::Count) {
            return;
        }

        m_mouseCurr[static_cast<size_t>(button)] = down ? 1 : 0;
    }

    void Input::AddMouseWheel(float x, float y) noexcept {
        m_mouseWheel.x += x;
        m_mouseWheel.y += y;
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

    bool Input::IsMouseButtonDown(MouseButton button) const noexcept {
        if (button == MouseButton::Count) {
            return false;
        }

        return m_mouseCurr[static_cast<size_t>(button)] != 0;
    }

    bool Input::WasMouseButtonPressed(MouseButton button) const noexcept {
        if (button == MouseButton::Count) {
            return false;
        }

        const auto i = static_cast<size_t>(button);
        return m_mouseCurr[i] && !m_mousePrev[i];
    }

    bool Input::WasMouseButtonReleased(MouseButton button) const noexcept {
        if (button == MouseButton::Count) {
            return false;
        }

        const auto i = static_cast<size_t>(button);
        return !m_mouseCurr[i] && m_mousePrev[i];
    }
}
