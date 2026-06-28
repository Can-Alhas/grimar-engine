// ~ Grimar Engine Animation2D BASE


#pragma once

#include <cstddef>
#include <string>
#include <vector>


namespace grimar::assets {

    class SpriteSheet;
    struct SpriteFrame;

    struct AnimationFrame {

        std::string spriteName{};
        double duration{0.1};
    };

    struct AnimationClip{

        std::vector<AnimationFrame> frames{};
        bool loop{true};
    };

    class AnimationState {
    public:
        AnimationState() noexcept = default;

        void SetClip(const AnimationClip* clip)    noexcept;
        void Reset()                               noexcept;
        void Update(double dt)                     noexcept;

        [[nodiscard]] const AnimationFrame* CurrentFrame() const noexcept;

        [[nodiscard]] const SpriteFrame* CurrentSprite(const SpriteSheet& sheet) const noexcept;

        void Play()  noexcept {m_playing = true;  }
        void Pause() noexcept {m_playing = false; }

        [[nodiscard]] bool isPlaying() const noexcept {return m_playing; }
        [[nodiscard]] std::size_t CurrentFrameIndex() const noexcept { return m_frameIndex; }

    private:

        const AnimationClip* m_clip{nullptr};
        std::size_t          m_frameIndex{0};
        double               m_timer{0.0};
        bool                 m_playing{true};
        
    };
}
