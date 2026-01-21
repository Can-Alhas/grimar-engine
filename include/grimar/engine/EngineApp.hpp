// ~ Grimar Engine ~
#pragma once

#include <cstdint>

struct SDL_Window;
struct SDL_Renderer;

namespace grimar::engine {

    struct EngineConfig {
        int windowWidth  = 1280;
        int windowHeight = 720;
        const char* windowTitle = "Grimar Engine";
        bool vsync = true;

        // Fixed Timestep (seconds)
        double fixedDeltaTime = 1.0 / 60.0;

        // Safety clamp (seconds)
        double maxDeltaTime = 0.25;

        int maxFixedStepsPerFrame = 5 ; // sprila-of-death guard
    };

    class EngineApp {
    public:
        explicit EngineApp(EngineConfig cfg = {}) noexcept;
        ~EngineApp() noexcept;

        EngineApp(const EngineApp& ) = delete;
        EngineApp operator=(const EngineApp& ) = delete;

        bool Init()     noexcept;
        int  Run()      noexcept; // main loop
        void Shutdown() noexcept; // safe to call multiple times

    private:
        bool InitSDL()       noexcept;
        void ShutdownSDL()   noexcept;

        void PollEvents()    noexcept;
        void Tick()          noexcept; // one frame: time/input/update/render

        // Hooks (now empty; later implement to systems)
        void FixedUpdate(double fixedDt) noexcept;
        void Update(double dt)           noexcept;
        void Render(double alpha)        noexcept; // alpha: [0,1)

    private:
        EngineConfig m_cfg{};
        bool m_running{false};

        SDL_Window*    m_window{nullptr};
        SDL_Renderer*  m_renderer{nullptr};
    };
}