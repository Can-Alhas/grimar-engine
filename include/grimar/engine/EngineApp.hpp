// ~ Grimar Engine ~
#pragma once

#include <memory>


#include "grimar/assets/AssetManager.hpp"
#include "grimar/debug/ImGuiDebugLayer.hpp"
#include "grimar/engine/EngineConfig.hpp"
#include "grimar/engine/Scene.hpp"
#include "grimar/platform/Input.hpp"
#include "grimar/platform/Window.hpp"
#include "grimar/render/Renderer2D.hpp"


namespace grimar::engine {

    class EngineApp {
    public:
        explicit EngineApp(EngineConfig cfg = {}) noexcept;
        ~EngineApp() noexcept;

        EngineApp(const EngineApp& ) = delete;
        EngineApp& operator=(const EngineApp& ) = delete;

        bool Init()     noexcept;
        int  Run()      noexcept; // main loop
        void Shutdown() noexcept; // safe to call multiple times
        void SetScene(std::unique_ptr<Scene> scene) noexcept;

    private:
        bool InitSDL()       noexcept;
        void ShutdownSDL()   noexcept;
        [[nodiscard]] SceneContext MakeSceneContext() noexcept;

        void PollEvents()    noexcept;
        void Tick()          noexcept; // one frame: time/input/update/render




        // Hooks (now empty; later implement to systems)
        void FixedUpdate(double fixedDt) noexcept;
        void Update(double dt)           noexcept;
        void Render(double alpha)        noexcept; // alpha: [0,1)

    private:
        EngineConfig m_cfg{};
        bool m_running{false};

        // SDL_Window*    m_window{nullptr}; // Deprec
        grimar::platform::Window m_window;
        grimar::platform::Input  m_input;
        std::unique_ptr<render::Renderer2D> m_renderer;

        grimar::assets::AssetManager m_assets;
        std::unique_ptr<Scene> m_activeScene{};
        bool m_sceneLoaded{false};
        bool m_debugUiEnabled{true};
        bool m_debugDrawEnabled{true};
        grimar::debug::ImGuiDebugLayer m_debugUi{};
    };
}
