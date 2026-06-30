// ~ Grimar Engine ~

#pragma once

namespace grimar::engine {

    struct EngineConfig {
        int windowWidth  = 1280;
        int windowHeight = 720;
        const char* windowTitle = "Grimar Engine";
        bool vsync = true;

        double fixedDeltaTime = 1.0 / 60.0;
        double maxDeltaTime = 0.25;
        int maxFixedStepsPerFrame = 5;
    };
}
