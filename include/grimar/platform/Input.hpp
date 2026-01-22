// ~ Grimar Engine ~

#pragma once

#include <array>
#include <cstdint>


namespace grimar::platform {

    enum class Key : uint16_t {
        Escape,
        A, D, W, S,
        Space,
        Left, Right, Up, Down,
        Count
    };


    class Input {
    public:
        void BeginFrame() noexcept; // call once per frame
        void SetKeyDown(Key key, bool down) noexcept; // called from event pump

        bool IsKeyDown(Key key) const noexcept;
        bool WasKeyPressed(Key key) const noexcept;
        bool WasKeyReleased(Key key) const noexcept;

    private:
        std::array<uint8_t, static_cast<size_t>(Key::Count)> m_curr{};
        std::array<uint8_t, static_cast<size_t>(Key::Count)> m_prev{};
    };
}

