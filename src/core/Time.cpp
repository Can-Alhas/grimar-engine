// ~ Grimar Engine ~

#include "grimar/core/Time.hpp"

#include <chrono>


namespace grimar::core {
    using clock = std::chrono::steady_clock;

    static clock::time_point s_lastFrame{};
    static double s_deltaTime{0.0};

    // Fixed timestep
    static double s_fixedDelta(1.0 / 60.0);
    static double s_accumulator(0.0);

    // Safety cap to avoid spiral of death (250 ms)
    static constexpr double MAX_DELTA = 0.25;

    void Time::BeginFrame() noexcept {
        const auto now = clock::now();

        if (s_lastFrame.time_since_epoch().count() == 0) {
            s_lastFrame = now;
            s_deltaTime = 0.0;
            return;
        }
        const std::chrono::duration<double> frameTime = now - s_lastFrame;
        s_lastFrame = now;

        s_deltaTime = std::min(frameTime.count(), MAX_DELTA);
        s_accumulator += s_deltaTime;
    }

    double Time::DeltaTime() noexcept {
        return s_deltaTime;
    }

    double Time::FixedDeltaTime() noexcept {
        return s_fixedDelta;
    }

    double Time::Accumulator() noexcept {
        return s_accumulator;
    }

    void Time::ConsumeFixedStep() noexcept {
        s_accumulator -= s_fixedDelta;
    }

    void Time::SetFixedDeltaTime(double seconds) noexcept {
        s_fixedDelta = seconds;
    }

    void Time::Reset() noexcept {
        s_lastFrame = {};
        s_deltaTime    = 0.0;
        s_accumulator  = 0.0;
    }
}
