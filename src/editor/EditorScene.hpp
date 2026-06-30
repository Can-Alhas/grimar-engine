// ~ Grimar Engine ~

#pragma once

#include <array>
#include <string>
#include <vector>

#include "grimar/assets/SpriteSheet.hpp"
#include "grimar/assets/TileMap.hpp"
#include "grimar/core/Math2D.hpp"
#include "grimar/engine/Scene.hpp"
#include "grimar/engine/World.hpp"
#include "grimar/engine/systems/TileMapSystem.hpp"
#include "grimar/render/Camera2D.hpp"

namespace grimar::editor {

    class EditorScene final : public grimar::engine::Scene {
    public:
        bool OnLoad(grimar::engine::SceneContext& context) noexcept override;
        void OnUnload(grimar::engine::SceneContext& context) noexcept override;
        void OnFixedUpdate(grimar::engine::SceneContext& context, double fixedDt) noexcept override;
        void OnUpdate(grimar::engine::SceneContext& context, double dt) noexcept override;
        void OnRender(grimar::engine::SceneContext& context, double alpha) noexcept override;
        void OnImGui(grimar::engine::SceneContext& context) noexcept override;

        [[nodiscard]] grimar::engine::World& GetWorld() noexcept override { return m_world; }
        [[nodiscard]] const grimar::engine::World& GetWorld() const noexcept override { return m_world; }
        [[nodiscard]] grimar::render::Camera2D& GetCamera() noexcept override { return m_camera; }
        [[nodiscard]] const grimar::render::Camera2D& GetCamera() const noexcept override { return m_camera; }

    private:
        enum class Tool {
            Paint,
            Erase,
            Picker,
            RectFill
        };

        struct PaletteEntry {
            grimar::assets::TileId id{0};
            std::string spriteName{};
            bool solid{true};
        };

        bool LoadSpriteSheet(grimar::engine::SceneContext& context) noexcept;
        bool GenerateGridSpriteSheet(grimar::engine::SceneContext& context) noexcept;
        bool NewMap() noexcept;
        bool OpenMap(grimar::engine::SceneContext& context) noexcept;
        bool SaveMap() noexcept;

        void ConfigureDefaultPaths() noexcept;
        void ApplyPresetSmall() noexcept;
        void ApplyPresetPlatformer() noexcept;
        void ApplyPresetLarge() noexcept;
        void RebuildPaletteFromSpriteSheet() noexcept;
        void RebuildPaletteFromTileMap() noexcept;
        void HandleShortcuts(grimar::engine::SceneContext& context) noexcept;
        void HandleCamera(grimar::engine::SceneContext& context, double dt) noexcept;
        void HandleCanvas(grimar::engine::SceneContext& context) noexcept;

        void RenderToolbar(grimar::engine::SceneContext& context) noexcept;
        void RenderPalettePanel() noexcept;
        void RenderSettingsPanel(grimar::engine::SceneContext& context) noexcept;
        void RenderNewMapWizard() noexcept;
        void RenderSelectedTilePreview() noexcept;
        void RenderGrid(grimar::render::Renderer2D& renderer) const noexcept;

        void SetTool(Tool tool) noexcept;
        void PaintBrush(int tileX, int tileY, grimar::assets::TileId id) noexcept;
        void FillRect(int ax, int ay, int bx, int by, grimar::assets::TileId id) noexcept;
        [[nodiscard]] bool MouseTile(grimar::engine::SceneContext& context,
                                     int& tileX,
                                     int& tileY) const noexcept;
        void CenterCameraOnMap() noexcept;
        void SetStatus(const char* text, bool error = false) noexcept;
        [[nodiscard]] bool HasPalette() const noexcept { return !m_palette.empty(); }
        [[nodiscard]] const char* ToolName() const noexcept;

        grimar::engine::World m_world{};
        grimar::render::Camera2D m_camera{};
        grimar::assets::SpriteSheet m_spriteSheet{};
        grimar::assets::TileMap m_tileMap{};
        grimar::engine::TileMapSystem m_tileMapSystem{};

        grimar::core::Vec2f m_tileMapOrigin{0.f, 0.f};
        grimar::core::Vec2f m_lastMousePosition{};
        bool m_hasLastMousePosition{false};

        std::vector<PaletteEntry> m_palette{};
        grimar::assets::TileId m_selectedTile{1};
        Tool m_tool{Tool::Paint};
        int m_brushSize{1};
        bool m_showGrid{true};
        bool m_showNewMapWizard{true};
        bool m_showAdvancedMapSettings{false};
        bool m_spriteSheetLoaded{false};
        bool m_mapReady{false};
        bool m_statusIsError{false};
        bool m_rectFillActive{false};
        int m_rectStartX{0};
        int m_rectStartY{0};

        int m_newMapWidth{32};
        int m_newMapHeight{18};
        int m_newTileWidth{64};
        int m_newTileHeight{64};
        int m_gridTileWidth{64};
        int m_gridTileHeight{64};

        std::array<char, 320> m_mapPath{};
        std::array<char, 320> m_spriteSheetPath{};
        std::array<char, 320> m_texturePath{};
        std::array<char, 320> m_generatedSpriteSheetPath{};
        std::array<char, 64> m_spritePrefix{};
        std::array<char, 192> m_status{};
    };
}
