

/////////

#include "grimar/assets/Animation2D.hpp"
#include "grimar/assets/SpriteSheet.hpp"

namespace grimar::assets {

    void AnimationState::SetClip(const AnimationClip* clip) noexcept {
        m_clip = clip;
        Reset();
    }

    void AnimationState::Reset() noexcept {
        m_frameIndex = 0;
        m_timer      = 0.0;
        m_playing    = true;
    }

    void AnimationState::Update(double dt) noexcept {
        if(!m_playing || !m_clip || m_clip->frames.empty() || dt <= 0.0) {
            return;
        }

        m_timer += dt;

        while (m_playing && m_clip && !m_clip->frames.empty()) {
            double frameDuration = m_clip->frames[m_frameIndex].duration;
            if (frameDuration <= 0.0) {
                frameDuration = 0.1;
            }

            if (m_timer < frameDuration) {
                break;
            }

            m_timer -= frameDuration;

            if (m_frameIndex + 1 < m_clip->frames.size()) {
                ++m_frameIndex;
            } else if (m_clip->loop) {
                m_frameIndex = 0;
            } else {
                m_frameIndex = m_clip->frames.size() - 1;
                m_timer = 0.0;
                m_playing = false;           
            }
        }
    }

    const AnimationFrame* AnimationState::CurrentFrame() const noexcept {
        if (!m_clip || m_clip->frames.empty()) {
            return nullptr;
        }

        if (m_frameIndex >= m_clip->frames.size()){
            return nullptr;
        }

        return &m_clip->frames[m_frameIndex];
    }


    const SpriteFrame* AnimationState::CurrentSprite(const SpriteSheet& sheet) const noexcept {
        const AnimationFrame* frame = CurrentFrame();
        if (!frame) {
            return nullptr;
        }

        return sheet.GetFrame(frame->spriteName);
    }

    
}
