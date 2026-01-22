// ~ Grimar Engine ~
#pragma once

#include <cstdint>


namespace grimar::core {

    class Time {
    public:
        // Called once per frame by the engine
        static void BeginFrame() noexcept;

        // Seconds
        static double DeltaTime() noexcept;
        static double FixedDeltaTime() noexcept;

        // Accumulator access (engine loop)
        static double Accumulator() noexcept;
        static void ConsumeFixedStep() noexcept;

        //
        static void ClearAccumulator() noexcept;


        // Optional controls
        static void SetFixedDeltaTime(double seconds) noexcept;
        static void Reset() noexcept;

    private:
        Time() = delete; // default ctor private and delete because
                         // static utility class

    };
}