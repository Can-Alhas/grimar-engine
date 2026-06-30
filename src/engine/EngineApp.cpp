// ~ Grimar Engine ~

#include "grimar/engine/EngineApp.hpp"

#include "grimar/core/Assert.hpp"
#include "grimar/core/Log.hpp"
#include "grimar/core/Time.hpp"
#include "grimar/render/SdlRenderer2D.hpp"

#include <SDL.h>
#include <SDL_image.h>

namespace grimar::engine {

    namespace {
        grimar::platform::Key MapKey(SDL_Keycode key) {
            using grimar::platform::Key;
            switch (key) {
                case SDLK_ESCAPE: return Key::Escape;
                case SDLK_a:      return Key::A;
                case SDLK_b:      return Key::B;
                case SDLK_d:      return Key::D;
                case SDLK_i:      return Key::I;
                case SDLK_o:      return Key::O;
                case SDLK_r:      return Key::R;
                case SDLK_w:      return Key::W;
                case SDLK_s:      return Key::S;
                case SDLK_q:      return Key::Q;
                case SDLK_e:      return Key::E;
                case SDLK_F1:     return Key::F1;
                case SDLK_F2:     return Key::F2;
                case SDLK_SPACE:  return Key::Space;
                case SDLK_LCTRL:
                case SDLK_RCTRL:  return Key::Ctrl;
                case SDLK_LALT:
                case SDLK_RALT:   return Key::Alt;
                case SDLK_LSHIFT:
                case SDLK_RSHIFT: return Key::Shift;
                case SDLK_LEFT:   return Key::Left;
                case SDLK_RIGHT:  return Key::Right;
                case SDLK_UP:     return Key::Up;
                case SDLK_DOWN:   return Key::Down;
                default:          return Key::Count;
            }
        }

        grimar::platform::MouseButton MapMouseButton(Uint8 button) {
            using grimar::platform::MouseButton;

            switch (button) {
                case SDL_BUTTON_LEFT:   return MouseButton::Left;
                case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
                case SDL_BUTTON_RIGHT:  return MouseButton::Right;
                case SDL_BUTTON_X1:     return MouseButton::X1;
                case SDL_BUTTON_X2:     return MouseButton::X2;
                default:                return MouseButton::Count;
            }
        }
    }

    EngineApp::EngineApp(EngineConfig cfg) noexcept
        : m_cfg(cfg) {
    }

    EngineApp::~EngineApp() noexcept {
        Shutdown();
    }

    void EngineApp::SetScene(std::unique_ptr<Scene> scene) noexcept {
        if (m_sceneLoaded && m_activeScene && m_renderer) {
            auto context = MakeSceneContext();
            m_activeScene->OnUnload(context);
            m_sceneLoaded = false;
        }

        m_activeScene = std::move(scene);

        if (m_running && m_activeScene && m_renderer) {
            auto context = MakeSceneContext();
            m_sceneLoaded = m_activeScene->OnLoad(context);
            if (!m_sceneLoaded) {
                GRIMAR_LOG_ERROR("EngineApp::SetScene failed: scene OnLoad returned false");
                m_running = false;
            }
        }
    }

    bool EngineApp::Init() noexcept {
        GRIMAR_LOG_INFO("EngineApp::Init()");

        if (!InitSDL()) {
            GRIMAR_LOG_ERROR("EngineApp::InitSDL failed");
            return false;
        }

        m_renderer = std::make_unique<grimar::render::SdlRenderer2D>();

        grimar::render::Renderer2DDesc rd{};
        rd.vsync = m_cfg.vsync;

        if (!m_renderer->Init(m_window, rd)) {
            GRIMAR_LOG_ERROR("Renderer2D init failed");
            m_renderer.reset();
            return false;
        }

        grimar::core::Time::Reset();
        grimar::core::Time::SetFixedDeltaTime(m_cfg.fixedDeltaTime);

        if (m_activeScene) {
            auto context = MakeSceneContext();
            m_sceneLoaded = m_activeScene->OnLoad(context);
            if (!m_sceneLoaded) {
                GRIMAR_LOG_ERROR("EngineApp::Init failed: scene OnLoad returned false");
                ShutdownSDL();
                return false;
            }
        }

        if (!m_debugUi.Init(m_window, *m_renderer)) {
            GRIMAR_LOG_WARN("EngineApp::Init: ImGui debug layer failed to initialize");
        }

        m_running = true;
        return true;
    }

    int EngineApp::Run() noexcept {
        GRIMAR_ASSERT(m_running && "Call Init() before Run()");

        while (m_running) {
            Tick();
        }

        Shutdown();
        return 0;
    }

    void EngineApp::Shutdown() noexcept {
        if (!m_running && !m_window.IsValid() && !m_renderer) {
            return;
        }

        GRIMAR_LOG_INFO("EngineApp::Shutdown()");

        if (m_sceneLoaded && m_activeScene && m_renderer) {
            auto context = MakeSceneContext();
            m_activeScene->OnUnload(context);
            m_sceneLoaded = false;
        }

        m_debugUi.Shutdown();
        ShutdownSDL();
        m_running = false;
    }

    SceneContext EngineApp::MakeSceneContext() noexcept {
        return SceneContext{
            *m_renderer,
            m_input,
            m_assets,
            m_cfg,
            m_debugDrawEnabled
        };
    }

    bool EngineApp::InitSDL() noexcept {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            GRIMAR_LOG_ERROR("{}", SDL_GetError());
            return false;
        }

        const int imgFlags = IMG_INIT_PNG;
        if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
            GRIMAR_LOG_ERROR("{}", IMG_GetError());
            SDL_Quit();
            return false;
        }

        grimar::platform::WindowDesc wd{};
        wd.title     = m_cfg.windowTitle;
        wd.width     = m_cfg.windowWidth;
        wd.height    = m_cfg.windowHeight;
        wd.resizable = false;

        if (!m_window.Create(wd)) {
            GRIMAR_LOG_ERROR("{}", SDL_GetError());
            IMG_Quit();
            SDL_Quit();
            return false;
        }

        return true;
    }

    void EngineApp::ShutdownSDL() noexcept {
        m_assets.ClearAll();
        m_renderer.reset();
        m_window.Destroy();

        IMG_Quit();
        SDL_Quit();
    }

    void EngineApp::PollEvents() noexcept {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                m_running = false;
            }

            m_debugUi.ProcessEvent(e);

            if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                const bool down = (e.type == SDL_KEYDOWN);
                const auto mapped = MapKey(e.key.keysym.sym);
                if (mapped != grimar::platform::Key::Count) {
                    m_input.SetKeyDown(mapped, down);
                }
            }

            if (e.type == SDL_MOUSEMOTION) {
                m_input.SetMousePosition(
                    static_cast<float>(e.motion.x),
                    static_cast<float>(e.motion.y)
                );
            }

            if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
                const bool down = (e.type == SDL_MOUSEBUTTONDOWN);
                const auto mapped = MapMouseButton(e.button.button);
                m_input.SetMousePosition(
                    static_cast<float>(e.button.x),
                    static_cast<float>(e.button.y)
                );
                m_input.SetMouseButtonDown(mapped, down);
            }

            if (e.type == SDL_MOUSEWHEEL) {
                m_input.AddMouseWheel(
                    static_cast<float>(e.wheel.x),
                    static_cast<float>(e.wheel.y)
                );
            }
        }

        if (m_input.WasKeyPressed(grimar::platform::Key::Escape)) {
            m_running = false;
        }

        if (m_input.WasKeyPressed(grimar::platform::Key::F1)) {
            m_debugUiEnabled = !m_debugUiEnabled;
        }

        if (m_input.WasKeyPressed(grimar::platform::Key::F2)) {
            m_debugDrawEnabled = !m_debugDrawEnabled;
        }
    }

    void EngineApp::Tick() noexcept {
        m_input.BeginFrame();
        PollEvents();

        if (!m_running) {
            return;
        }

        grimar::core::Time::BeginFrame();

        const double fixedDt = grimar::core::Time::FixedDeltaTime();
        int steps = 0;

        while (grimar::core::Time::Accumulator() >= fixedDt) {
            FixedUpdate(fixedDt);
            grimar::core::Time::ConsumeFixedStep();

            if (++steps >= m_cfg.maxFixedStepsPerFrame) {
                GRIMAR_LOG_WARN("Spiral guard triggered: dropping accumulated time");
                grimar::core::Time::ClearAccumulator();
                break;
            }
        }

        const double dt = grimar::core::Time::DeltaTime();
        Update(dt);

        const double alpha =
            (fixedDt > 0.0) ? (grimar::core::Time::Accumulator() / fixedDt) : 0.0;

        Render(alpha);
    }

    void EngineApp::FixedUpdate(double fixedDt) noexcept {
        if (!m_activeScene || !m_sceneLoaded) {
            return;
        }

        auto context = MakeSceneContext();
        m_activeScene->OnFixedUpdate(context, fixedDt);
    }

    void EngineApp::Update(double dt) noexcept {
        if (!m_activeScene || !m_sceneLoaded) {
            return;
        }

        auto context = MakeSceneContext();
        m_activeScene->OnUpdate(context, dt);
    }

    void EngineApp::Render(double alpha) noexcept {
        if (!m_renderer) {
            return;
        }

        m_renderer->BeginFrame();
        m_renderer->Clear({20, 20, 20, 255});

        if (m_activeScene && m_sceneLoaded) {
            m_renderer->SetCamera(&m_activeScene->GetCamera());

            auto context = MakeSceneContext();
            m_activeScene->OnRender(context, alpha);
        }

        m_renderer->Flush();

        if (m_activeScene && m_sceneLoaded && m_debugUi.IsInitialized()) {
            m_debugUi.BeginFrame();

            if (m_debugUiEnabled) {
                m_debugUi.Render(*m_activeScene, m_debugDrawEnabled);
            }

            auto context = MakeSceneContext();
            m_activeScene->OnImGui(context);
            m_debugUi.Draw(*m_renderer);
        }

        m_renderer->Present();
    }
}
