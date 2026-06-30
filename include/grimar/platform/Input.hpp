// ~ Grimar Engine ~

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "grimar/core/Math2D.hpp"


namespace grimar::platform {

    enum class Key : uint16_t {
        Escape,
        A, D, W, S,
        B, I, O, R,
        Q, E,
        F1, F2,
        Space,
        Ctrl, Alt, Shift,
        Left, Right, Up, Down,
        Count
    };

    enum class MouseButton : std::uint16_t {
        Left,
        Middle,
        Right,
        X1,
        X2,
        Count
    };

    class Input {
    public:
        void BeginFrame() noexcept; // call once per frame
        void SetKeyDown(Key key, bool down) noexcept; // called from event pump
        void SetMousePosition(float x, float y) noexcept;
        void SetMouseButtonDown(MouseButton button, bool down) noexcept;
        void AddMouseWheel(float x, float y) noexcept;

        [[nodiscard]] bool IsKeyDown(Key key) const noexcept;
        [[nodiscard]] bool WasKeyPressed(Key key) const noexcept;
        [[nodiscard]] bool WasKeyReleased(Key key) const noexcept;
        [[nodiscard]] bool IsMouseButtonDown(MouseButton button) const noexcept;
        [[nodiscard]] bool WasMouseButtonPressed(MouseButton button) const noexcept;
        [[nodiscard]] bool WasMouseButtonReleased(MouseButton button) const noexcept;
        [[nodiscard]] grimar::core::Vec2f MousePosition() const noexcept { return m_mousePosition; }
        [[nodiscard]] grimar::core::Vec2f MouseWheel() const noexcept { return m_mouseWheel; }

    private:
        std::array<uint8_t, static_cast<size_t>(Key::Count)> m_curr{};
        std::array<uint8_t, static_cast<size_t>(Key::Count)> m_prev{};
        std::array<uint8_t, static_cast<size_t>(MouseButton::Count)> m_mouseCurr{};
        std::array<uint8_t, static_cast<size_t>(MouseButton::Count)> m_mousePrev{};
        grimar::core::Vec2f m_mousePosition{};
        grimar::core::Vec2f m_mouseWheel{};
    };
}

