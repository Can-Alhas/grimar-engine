
#include "grimar/engine/EngineApp.hpp"
#include "grimar/core/Log.hpp"


int main()
{
#pragma region old logs
    //GRIMAR_ASSERT(2 + 2 == 4);
    //GRIMAR_ASSERT_MSG(false, "Intentional assert test");

    // GRIMAR_LOG_TRACE("info test");
    // GRIMAR_LOG_INFO("info test");
    // GRIMAR_LOG_WARN("warn test");
    // GRIMAR_LOG_ERROR("error test");

    //GRIMAR_LOG_INFO("Boot");
    //GRIMAR_ASSERT_MSG(false, "Intentional assert test");


    // for (int i = 0; i < 5; ++i) {
    //     grimar::core::Time::BeginFrame();
    //
    //     GRIMAR_LOG_INFO("Frame start");
    //
    //     while (grimar::core::Time::Accumulator() >=
    //            grimar::core::Time::FixedDeltaTime()) {
    //
    //         GRIMAR_LOG_TRACE("FixedUpdate tick");
    //         grimar::core::Time::ConsumeFixedStep();
    //     }
    // }

#pragma endregion


    grimar::core::SetLogLevel(grimar::core::LogLevel::Trace);

    grimar::engine::EngineConfig cfg{};
    cfg.windowTitle  = "Grimar Sandbox";
    cfg.windowWidth  = 1280;
    cfg.windowHeight = 720;

    // Test : vsync false
    cfg.vsync = true;

    // Fixed timestep
    cfg.fixedDeltaTime = 1.0 / 60.0;

    // Spiral-of-death guard
    cfg.maxFixedStepsPerFrame = 5;

    grimar::engine::EngineApp engine(cfg);
    if (!engine.Init())
        return 1;

    return engine.Run();


}
