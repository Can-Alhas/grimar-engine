// ~ Grimar Engine ~

#include "grimar/engine/EngineApp.hpp"


#include "grimar/core/Assert.hpp"
#include "grimar/core/Log.hpp"
#include "grimar/core/Time.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include "grimar/render/SdlRenderer2D.hpp"


namespace grimar::engine {

    // for tests
    static int    s_fixedCount = 0;
    static double s_fixedTimer = 0.0;

    static int    s_frameCount = 0;
    static double s_frameTimer = 0.0;
    // for tests

    static grimar::platform::Key MapKey(SDL_Keycode key) {
        using grimar::platform::Key;
        switch (key) {
            case SDLK_ESCAPE: return Key::Escape;
            case SDLK_a:      return Key::A;
            case SDLK_d:      return Key::D;
            case SDLK_w:      return Key::W;
            case SDLK_s:      return Key::S;
            case SDLK_q:      return Key::Q;
            case SDLK_e:      return Key::E;
            case SDLK_SPACE:  return Key::Space;
            case SDLK_LEFT:   return Key::Left;
            case SDLK_RIGHT:  return Key::Right;
            case SDLK_UP:     return Key::Up;
            case SDLK_DOWN:   return Key::Down;
            default:      return Key::Count;
        }
    }

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

        GRIMAR_LOG_INFO("InitSDL OK");

        m_renderer = std::make_unique<grimar::render::SdlRenderer2D>();

        grimar::render::Renderer2DDesc rd{};
        rd.vsync = m_cfg.vsync;

        if (!m_renderer->Init(m_window, rd)) {
            GRIMAR_LOG_ERROR("Renderer2D init failed");
            m_renderer.reset();
            return false;
        }

        GRIMAR_LOG_INFO("Renderer2D OK");

        // === Camera init ===
        m_camera.SetViewport(m_cfg.windowWidth, m_cfg.windowHeight);
        m_camera.SetPosition({0.f, 0.f});
        m_camera.SetZoom(1.0f);
        m_renderer->SetCamera(&m_camera);

        auto t1 = m_assets.LoadTexture(*m_renderer, "assets/test.png");
        auto t2 = m_assets.LoadTexture(*m_renderer, "assets/test.png");

        GRIMAR_ASSERT(m_assets.TextureCount() == 1);

        if (t1 && t2) {
            GRIMAR_LOG_INFO("AssetManager load OK (twice)");
        } else {
            GRIMAR_LOG_WARN("AssetManager load failed");
        }


        m_textTex = m_assets.LoadTexture(*m_renderer, "assets/test.png");
        if (!m_textTex) {
            GRIMAR_LOG_WARN("test.png failed to load");
        } else {
            GRIMAR_LOG_INFO("test.png loaded (cached)");
        }

        /*if (!m_testTexture.LoadFromFile(*m_renderer, "assets/test.png")) {
            GRIMAR_LOG_WARN("test.png failed to load (check path)");
        } else {
            //GRIMAR_LOG_INFO("test.png loaded");
        }*/

        // === Time ===
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
        if (!m_running && !m_window.IsValid() && !m_renderer) return; // already shutdown (simple guard)

        GRIMAR_LOG_INFO("EngineApp::Shutdown()");
        ShutdownSDL();

        m_running = false;
    }

    bool EngineApp::InitSDL() noexcept {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            GRIMAR_LOG_ERROR(SDL_GetError());
            return false;
        }

        // (SDL_image init)
        const int imgFlags = IMG_INIT_PNG;
        if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
            GRIMAR_LOG_ERROR(IMG_GetError());
            SDL_Quit();
            return false;
        }

        grimar::platform::WindowDesc wd{};
        wd.title     = m_cfg.windowTitle;
        wd.width     = m_cfg.windowWidth;
        wd.height    = m_cfg.windowHeight;
        wd.resizable = false;

        if (!m_window.Create(wd)) {
            GRIMAR_LOG_ERROR(SDL_GetError());
            IMG_Quit();  // SDL_image close
            SDL_Quit();
            return false;
        }


        return true;
    }

    void EngineApp::ShutdownSDL() noexcept {

        m_renderer.reset(); // Renderer2D::~Renderer2D() cagrilir

        m_window.Destroy();

        IMG_Quit(); // img_quit ===
        SDL_Quit();
    }


    void EngineApp::PollEvents() noexcept {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) m_running = false;


            if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                const bool down = (e.type == SDL_KEYDOWN);
                const auto mapped = MapKey(e.key.keysym.sym);
                if (mapped != grimar::platform::Key::Count) {
                    m_input.SetKeyDown(mapped, down);
                }
            }


        }

        // exit with ESC ------ input
        if (m_input.WasKeyPressed(grimar::platform::Key::Escape)) {
            m_running = false;
        }
    }

    void EngineApp::Tick() noexcept {
        m_input.BeginFrame(); // prev = curr
        PollEvents();        // curr filled

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

                //grimar::core::Time::Reset(); // or: just clear accumulator
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
            //GRIMAR_LOG_INFO("FixedUpdate: running at ~60 Hz");
            s_fixedTimer = 0.0;
            s_fixedCount = 0;
        }
    }
    // TODO ~~~~
    void EngineApp::Update(double dt) noexcept {

        // this or [[maybe_unused]]
        //(void)dt;

        auto pos = m_camera.Position();
        const float speed = 300.f * static_cast<float>(dt);


        using grimar::platform::Key;

        if (m_input.WasKeyPressed(Key::A)) {
            GRIMAR_LOG_INFO("A pressed");
        }

        // Movement (Hold)
        if (m_input.IsKeyDown(Key::A)) pos.x += speed;
        if (m_input.IsKeyDown(Key::D)) pos.x -= speed;
        if (m_input.IsKeyDown(Key::W)) pos.y -= speed;
        if (m_input.IsKeyDown(Key::S)) pos.y += speed;

        m_camera.SetPosition(pos);


        // zoom (Step)
        float zoom = m_camera.Zoom();
        if (m_input.WasKeyPressed(Key::Q)) {
            GRIMAR_LOG_INFO("Zoom OUT");
            zoom *= 0.9f;
        }
        if (m_input.WasKeyPressed(Key::E)) {
            GRIMAR_LOG_INFO("Zoom IN");
            zoom *= 1.1f;
        }

        if (zoom < 0.25f) zoom = 0.25f;
        if (zoom > 6.0f)  zoom = 6.0f;
        m_camera.SetZoom(zoom);

        //fps test log
        ++s_frameCount;
        s_frameTimer += dt;

        if (s_frameTimer >= 1.0) {
            //GRIMAR_LOG_INFO("Update/Render: ~FPS measured");
            s_frameTimer = 0.0;
            s_frameCount = 0;
        }
    }

    void EngineApp::Render(double /*alpha*/) noexcept {

        if (!m_renderer) return;


        m_renderer->BeginFrame();

        m_renderer->Clear({20, 20, 20, 255});



        m_renderer->DrawRect({100, 100, 200, 140},
                              {60, 180, 255, 255},
                              0);





        if (m_textTex) {
            m_renderer->DrawSprite(
                *m_textTex,
                grimar::render::RectI{0, 0, m_textTex->Width(), m_textTex->Height()}, // full texture
                grimar::render::RectF{200.f, 150.f, 256.f, 256.f  }, // world dst
                5
            );
        } else {
            GRIMAR_LOG_WARN("m_testTex is null");
        }


        m_renderer->DrawRect({160, 140, 200, 140},
                    {220, 80, 80, 255},
                    10);

        m_renderer->EndFrame();
    }

}
