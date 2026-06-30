// ~ Grimar Engine ~

#include "grimar/debug/ImGuiDebugLayer.hpp"

#include "grimar/engine/Scene.hpp"
#include "grimar/engine/World.hpp"
#include "grimar/engine/components/Animator2D.hpp"
#include "grimar/engine/components/BoxCollider2D.hpp"
#include "grimar/engine/components/CharacterController2D.hpp"
#include "grimar/engine/components/RigidBody2D.hpp"
#include "grimar/engine/components/SpriteRenderer.hpp"
#include "grimar/engine/components/Transform2D.hpp"
#include "grimar/platform/Window.hpp"
#include "grimar/render/Camera2D.hpp"
#include "grimar/render/Renderer2D.hpp"

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#include <cstdio>

namespace grimar::debug {

    namespace {
        void TextVec2(const char* label, grimar::core::Vec2f value) {
            ImGui::Text("%s: %.2f, %.2f", label, value.x, value.y);
        }
    }

    ImGuiDebugLayer::~ImGuiDebugLayer() {
        Shutdown();
    }

    bool ImGuiDebugLayer::Init(grimar::platform::Window& window,
                               grimar::render::Renderer2D& renderer) noexcept {
        if (m_initialized) {
            return true;
        }

        auto* sdlRenderer = static_cast<SDL_Renderer*>(renderer.NativeHandle());
        if (!window.IsValid() || !sdlRenderer) {
            return false;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        if (!ImGui_ImplSDL2_InitForSDLRenderer(window.NativeHandle(), sdlRenderer)) {
            ImGui::DestroyContext();
            return false;
        }

        if (!ImGui_ImplSDLRenderer2_Init(sdlRenderer)) {
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();
            return false;
        }

        m_initialized = true;
        return true;
    }

    void ImGuiDebugLayer::Shutdown() noexcept {
        if (!m_initialized) {
            return;
        }

        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        m_initialized = false;
        m_selected = {};
    }

    void ImGuiDebugLayer::ProcessEvent(const SDL_Event& event) noexcept {
        if (!m_initialized) {
            return;
        }

        ImGui_ImplSDL2_ProcessEvent(&event);
    }

    void ImGuiDebugLayer::BeginFrame() noexcept {
        if (!m_initialized) {
            return;
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiDebugLayer::Render(grimar::engine::Scene& scene,
                                 bool& debugDrawEnabled) noexcept {
        if (!m_initialized) {
            return;
        }

        ImGui::Begin("Grimar Debug");

        const auto stats = scene.GetWorld().DebugStats();
        ImGui::Text("Entities: %u", stats.aliveEntities);
        ImGui::Text("Components: T%u S%u A%u CC%u RB%u C%u",
                    stats.transforms,
                    stats.spriteRenderers,
                    stats.animators,
                    stats.characterControllers,
                    stats.rigidBodies,
                    stats.boxColliders);

        TextVec2("Camera", scene.GetCamera().Position());
        ImGui::Text("Zoom: %.2f", scene.GetCamera().Zoom());

        ImGui::Separator();
        ImGui::Checkbox("Debug draw", &debugDrawEnabled);

        ImGui::Separator();
        RenderEntityBrowser(scene);

        ImGui::End();
    }

    void ImGuiDebugLayer::Draw(grimar::render::Renderer2D& renderer) noexcept {
        if (!m_initialized) {
            return;
        }

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(
            ImGui::GetDrawData(),
            static_cast<SDL_Renderer*>(renderer.NativeHandle())
        );
    }

    void ImGuiDebugLayer::RenderEntityBrowser(grimar::engine::Scene& scene) noexcept {
        auto& world = scene.GetWorld();

        if (m_selected.IsValid() && !world.IsAlive(m_selected)) {
            m_selected = {};
        }

        ImGui::BeginChild("Entities", ImVec2(180.f, 260.f), true);
        world.ForEachEntity([&](grimar::engine::Entity entity) {
            char label[64]{};
            std::snprintf(label, sizeof(label), "Entity %u:%u", entity.id, entity.generation);

            const bool selected = (entity == m_selected);
            if (ImGui::Selectable(label, selected)) {
                m_selected = entity;
            }
        });
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("EntityDetails", ImVec2(0.f, 260.f), true);
        if (!m_selected.IsValid() || !world.IsAlive(m_selected)) {
            ImGui::Text("No entity selected");
            ImGui::EndChild();
            return;
        }

        ImGui::Text("Selected: %u:%u", m_selected.id, m_selected.generation);
        ImGui::Separator();

        if (const auto* transform = world.GetTransform(m_selected)) {
            ImGui::Text("Transform");
            TextVec2("  position", transform->position);
            TextVec2("  scale", transform->scale);
            ImGui::Text("  rotation: %.2f", transform->rotation);
        }

        if (const auto* sprite = world.GetSpriteRenderer(m_selected)) {
            ImGui::Separator();
            ImGui::Text("SpriteRenderer");
            ImGui::Text("  sprite: %s", sprite->spriteName.c_str());
            TextVec2("  size", sprite->size);
            ImGui::Text("  layer: %d", sprite->layer);
            ImGui::Text("  visible: %s", sprite->visible ? "true" : "false");
        }

        if (const auto* animator = world.GetAnimator2D(m_selected)) {
            ImGui::Separator();
            ImGui::Text("Animator2D");
            ImGui::Text("  frame: %zu", animator->frameIndex);
            ImGui::Text("  timer: %.3f", animator->timer);
            ImGui::Text("  playing: %s", animator->playing ? "true" : "false");
        }

        if (const auto* body = world.GetRigidBody(m_selected)) {
            ImGui::Separator();
            ImGui::Text("RigidBody2D");
            TextVec2("  velocity", body->velocity);
            ImGui::Text("  gravityScale: %.2f", body->gravityScale);
            ImGui::Text("  grounded: %s", body->grounded ? "true" : "false");
        }

        if (const auto* controller = world.GetCharacterController2D(m_selected)) {
            ImGui::Separator();
            ImGui::Text("CharacterController2D");
            ImGui::Text("  moveSpeed: %.2f", controller->moveSpeed);
            ImGui::Text("  jumpVelocity: %.2f", controller->jumpVelocity);
            ImGui::Text("  maxFallSpeed: %.2f", controller->maxFallSpeed);
            ImGui::Text("  coyoteTimer: %.3f", controller->coyoteTimer);
            ImGui::Text("  jumpBufferTimer: %.3f", controller->jumpBufferTimer);
        }

        if (const auto* collider = world.GetBoxCollider(m_selected)) {
            ImGui::Separator();
            ImGui::Text("BoxCollider2D");
            TextVec2("  offset", collider->offset);
            TextVec2("  size", collider->size);
            ImGui::Text("  trigger: %s", collider->isTrigger ? "true" : "false");
        }

        ImGui::EndChild();
    }
}
