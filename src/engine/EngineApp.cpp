// ~ Grimar Engine ~

#include "grimar/engine/EngineApp.hpp"

#include "grimar/core/Assert.hpp"
#include "grimar/core/Log.hpp"
#include "grimar/core/Time.hpp"

#include <SDL.h>


namespace grimar::engine {

    // for tests
    static int    s_fixedCount = 0;
    static double s_fixedTimer = 0.0;

    static int    s_frameCount = 0;
    static double s_frameTimer = 0.0;
    // for tests

    EngineApp::EngineApp(EngineConfig cfg) noexcept
    : m_cfg(cfg) { }

    EngineApp::~EngineApp() noexcept {
        Shutdown();
    }

    bool EngineApp::Init() noexcept {
        GRIMAR_LOG_INFO("EngineApp::Init()");

        if (!InitSDL()) {
            GRIMAR_LOG_ERROR("EngineApp::InitSDL failed");
            return false;
        }

        grimar::core::Time::Reset();
        grimar::core::Time::SetFixedDeltaTime(m_cfg.fixedDeltaTime);

        m_running = true;
        return true;
    }

    int EngineApp::Run() noexcept {
        GRIMAR_ASSERT(m_running && "Call Init() before Run()");
        GRIMAR_LOG_INFO("EngineApp::Run()");

        while (m_running) {
            Tick();
        }

        Shutdown();
        return 0;
    }

    void EngineApp::Shutdown() noexcept {
        if (!m_window && !m_renderer) return; // already shutdown (simple guard)

        GRIMAR_LOG_INFO("EngineApp::Shutdown()");
        ShutdownSDL();

        m_running = false;
    }

    bool EngineApp::InitSDL() noexcept {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            GRIMAR_LOG_ERROR(SDL_GetError());
            return false;
        }

        m_window = SDL_CreateWindow(
            m_cfg.windowTitle,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            m_cfg.windowWidth,
            m_cfg.windowHeight,
            SDL_WINDOW_SHOWN
        );

        if (!m_window) {
            GRIMAR_LOG_ERROR(SDL_GetError());
            SDL_Quit();
            return false;
        }

        const Uint32 flags = SDL_RENDERER_ACCELERATED |
                             (m_cfg.vsync ? SDL_RENDERER_PRESENTVSYNC : 0);

        m_renderer = SDL_CreateRenderer(m_window, -1, flags);
        if (!m_renderer) {
            GRIMAR_LOG_ERROR(SDL_GetError());
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
            SDL_Quit();
            return false;
        }

        return true;
    }

    void EngineApp::ShutdownSDL() noexcept {
        if (m_renderer) {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }

        if (m_window) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
        SDL_Quit();
    }


    void EngineApp::PollEvents() noexcept {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                m_running = false;
            }

            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                m_running = false;
            }
        }
    }

    void EngineApp::Tick() noexcept {
        PollEvents();

        // time update (delta + acccumulator)
        grimar::core::Time::BeginFrame();

        const double fixedDt = grimar::core::Time::FixedDeltaTime();
        int steps = 0;

        // fixed update loop (deterministic)
        while (grimar::core::Time::Accumulator() >= fixedDt ) {
            FixedUpdate(fixedDt);
            grimar::core::Time::ConsumeFixedStep();

            if (++steps >= m_cfg.maxFixedStepsPerFrame) {
                // Spiral-of-death guard: drop remaining accumulated time
                GRIMAR_LOG_WARN("Spiral guard triggered: dropping accumulated time");

                // (prevents infinite catch-up under heavy load)

                grimar::core::Time::Reset(); // or: just clear accumulator
                // Better: clear only accumulator (see note below)

                grimar::core::Time::ClearAccumulator();
                break;
            }
        }


        // Variable update (render-facing logic)
        const double dt = grimar::core::Time::DeltaTime();
        Update(dt);

        // Render interpolation alpha
        const double alpha =
            (fixedDt > 0.0) ? (grimar::core::Time::Accumulator() / fixedDt) : 0.0;

        Render(alpha);
    }

    // TODO ~~~~
    void EngineApp::FixedUpdate(double fixedDt) noexcept {
        // later:: physics, fixed-step systems

        ++s_fixedCount;
        s_fixedTimer += fixedDt;

        if (s_fixedTimer >= 1.0) {
            GRIMAR_LOG_INFO("FixedUpdate: running at ~60 Hz");
            s_fixedTimer = 0.0;
            s_fixedCount = 0;
        }
    }
    // TODO ~~~~
    void EngineApp::Update(double dt) noexcept {
        // later: game logic, animations, variable-step systems

        // for test
        SDL_Delay(20);
        //

        ++s_frameCount;
        s_frameTimer += dt;

        if (s_frameTimer >= 1.0) {
            GRIMAR_LOG_INFO("Update/Render: ~FPS measured");
            s_frameTimer = 0.0;
            s_fixedCount = 0;
        }
    }

    void EngineApp::Render(double /*alpha*/) noexcept {
        // Clear background
        SDL_SetRenderDrawColor(m_renderer, 30, 30, 30, 255);
        SDL_RenderClear(m_renderer);

        SDL_RenderPresent(m_renderer);
    }

}
