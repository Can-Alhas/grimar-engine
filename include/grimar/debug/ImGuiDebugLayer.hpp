// ~ Grimar Engine ~

#pragma once

#include "grimar/engine/Entity.hpp"

union SDL_Event;

namespace grimar::engine {
    class Scene;
}

namespace grimar::platform {
    class Window;
}

namespace grimar::render {
    class Renderer2D;
}

namespace grimar::debug {

    class ImGuiDebugLayer {
    public:
        ImGuiDebugLayer() = default;
        ~ImGuiDebugLayer();

        ImGuiDebugLayer(const ImGuiDebugLayer&) = delete;
        ImGuiDebugLayer& operator=(const ImGuiDebugLayer&) = delete;

        [[nodiscard]] bool Init(grimar::platform::Window& window,
                                grimar::render::Renderer2D& renderer) noexcept;
        void Shutdown() noexcept;

        void ProcessEvent(const SDL_Event& event) noexcept;
        void BeginFrame() noexcept;
        void Render(grimar::engine::Scene& scene, bool& debugDrawEnabled) noexcept;
        void Draw(grimar::render::Renderer2D& renderer) noexcept;

        [[nodiscard]] bool IsInitialized() const noexcept { return m_initialized; }

    private:
        void RenderEntityBrowser(grimar::engine::Scene& scene) noexcept;

        bool m_initialized{false};
        grimar::engine::Entity m_selected{};
    };
}
