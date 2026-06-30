// ~ Grimar Engine ~

#include "editor/EditorScene.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>

#include <imgui.h>

#include "grimar/assets/AssetManager.hpp"
#include "grimar/assets/Texture2D.hpp"
#include "grimar/platform/FileSystem.hpp"
#include "grimar/platform/Input.hpp"
#include "grimar/render/Color.hpp"
#include "grimar/render/Renderer2D.hpp"

namespace grimar::editor {

    namespace {
        constexpr float ToolbarHeight = 44.f;
        constexpr float LeftPanelWidth = 280.f;
        constexpr float RightPanelWidth = 380.f;

        template <std::size_t N>
        void CopyText(std::array<char, N>& dst, const char* src) noexcept {
            dst.fill('\0');
            if (!src) {
                return;
            }

            std::snprintf(dst.data(), dst.size(), "%s", src);
        }

        template <std::size_t N>
        [[nodiscard]] std::string BufferText(const std::array<char, N>& buffer) {
            return std::string(buffer.data());
        }

        [[nodiscard]] bool ImGuiWantsMouse() noexcept {
            return ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureMouse;
        }

        [[nodiscard]] bool ImGuiWantsKeyboard() noexcept {
            return ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureKeyboard;
        }

        [[nodiscard]] bool HighlightButton(const char* label, bool active) noexcept {
            if (active) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.46f, 0.74f, 1.00f));
            }

            const bool clicked = ImGui::Button(label);

            if (active) {
                ImGui::PopStyleColor();
            }

            return clicked;
        }

        template <std::size_t N>
        void CopyResolvedOrRaw(std::array<char, N>& dst, const char* path) noexcept {
            const std::string resolved = grimar::platform::ResolveExistingPath(path ? path : "");
            CopyText(dst, resolved.empty() ? path : resolved.c_str());
        }
    }

    bool EditorScene::OnLoad(grimar::engine::SceneContext& context) noexcept {
        m_camera.SetViewport(context.config.windowWidth, context.config.windowHeight);
        ConfigureDefaultPaths();
        SetStatus("Choose a sprite source, then create a map");
        return true;
    }

    void EditorScene::OnUnload(grimar::engine::SceneContext&) noexcept {
        m_spriteSheet.Clear();
        m_tileMap.Clear();
        m_world.Clear();
        m_palette.clear();
    }

    void EditorScene::OnFixedUpdate(grimar::engine::SceneContext&, double) noexcept {
    }

    void EditorScene::OnUpdate(grimar::engine::SceneContext& context, double dt) noexcept {
        HandleShortcuts(context);
        HandleCamera(context, dt);
        HandleCanvas(context);
    }

    void EditorScene::OnRender(grimar::engine::SceneContext& context, double) noexcept {
        if (m_mapReady) {
            m_tileMapSystem.Render(m_tileMap, m_spriteSheet, context.renderer, m_tileMapOrigin, 0);
        }

        if (m_mapReady && m_showGrid) {
            RenderGrid(context.renderer);
        }
    }

    void EditorScene::OnImGui(grimar::engine::SceneContext& context) noexcept {
        RenderToolbar(context);
        RenderPalettePanel();
        RenderSettingsPanel(context);
        RenderNewMapWizard();
    }

    bool EditorScene::LoadSpriteSheet(grimar::engine::SceneContext& context) noexcept {
        const auto rawPath = BufferText(m_spriteSheetPath);
        if (rawPath.empty()) {
            SetStatus("SpriteSheet path is empty", true);
            return false;
        }

        const auto path = grimar::platform::ResolveExistingPath(rawPath);
        if (path.empty()) {
            SetStatus("SpriteSheet file not found", true);
            return false;
        }

        CopyText(m_spriteSheetPath, path.c_str());

        if (!m_spriteSheet.Load(context.assets, context.renderer, path)) {
            SetStatus("SpriteSheet load failed; check texture path", true);
            m_spriteSheetLoaded = false;
            return false;
        }

        m_spriteSheetLoaded = true;
        m_tileMap.SetSpriteSheetPath(path);
        RebuildPaletteFromSpriteSheet();
        SetStatus("SpriteSheet loaded");
        return true;
    }

    bool EditorScene::GenerateGridSpriteSheet(grimar::engine::SceneContext& context) noexcept {
        const auto rawTexturePath = BufferText(m_texturePath);
        if (rawTexturePath.empty()) {
            SetStatus("PNG texture path is empty", true);
            return false;
        }

        const auto texturePath = grimar::platform::ResolveExistingPath(rawTexturePath);
        if (texturePath.empty()) {
            SetStatus("PNG texture file not found", true);
            return false;
        }

        CopyText(m_texturePath, texturePath.c_str());

        if (m_gridTileWidth <= 0 || m_gridTileHeight <= 0) {
            SetStatus("Grid tile size must be positive", true);
            return false;
        }

        auto texture = context.assets.LoadTexture(context.renderer, texturePath);
        if (!texture) {
            SetStatus("PNG texture load failed", true);
            return false;
        }

        const grimar::assets::SpriteSheetGridDesc desc{
            texturePath,
            texture->Width(),
            texture->Height(),
            m_gridTileWidth,
            m_gridTileHeight,
            BufferText(m_spritePrefix)
        };

        const auto generatedPath = BufferText(m_generatedSpriteSheetPath);
        if (!grimar::assets::SaveSpriteSheetGridJson(desc, generatedPath)) {
            SetStatus("Grid SpriteSheet save failed", true);
            return false;
        }

        CopyText(m_spriteSheetPath, generatedPath.c_str());
        m_newTileWidth = m_gridTileWidth;
        m_newTileHeight = m_gridTileHeight;
        return LoadSpriteSheet(context);
    }

    bool EditorScene::NewMap() noexcept {
        if (!m_tileMap.Create(
                m_newMapWidth,
                m_newMapHeight,
                m_newTileWidth,
                m_newTileHeight,
                BufferText(m_spriteSheetPath))) {
            SetStatus("New map failed", true);
            m_mapReady = false;
            return false;
        }

        if (m_spriteSheetLoaded) {
            RebuildPaletteFromSpriteSheet();
        } else {
            m_palette.clear();
        }

        m_mapReady = true;
        m_rectFillActive = false;
        CenterCameraOnMap();
        SetStatus(m_spriteSheetLoaded ? "Map created" : "Map created; import sprites to paint");
        return true;
    }

    bool EditorScene::OpenMap(grimar::engine::SceneContext& context) noexcept {
        const auto rawPath = BufferText(m_mapPath);
        if (rawPath.empty()) {
            SetStatus("Map path is empty", true);
            return false;
        }

        const auto path = grimar::platform::ResolveExistingPath(rawPath);
        if (path.empty()) {
            SetStatus("Map file not found", true);
            m_mapReady = false;
            return false;
        }

        CopyText(m_mapPath, path.c_str());

        if (!m_tileMap.Load(path)) {
            SetStatus("Open map failed", true);
            m_mapReady = false;
            return false;
        }

        m_newMapWidth = m_tileMap.Width();
        m_newMapHeight = m_tileMap.Height();
        m_newTileWidth = m_tileMap.TileWidth();
        m_newTileHeight = m_tileMap.TileHeight();
        m_mapReady = true;

        if (!m_tileMap.SpriteSheetPath().empty()) {
            const auto spriteSheetPath = grimar::platform::ResolveExistingPathRelativeTo(
                path,
                m_tileMap.SpriteSheetPath()
            );
            if (!spriteSheetPath.empty()) {
                CopyText(m_spriteSheetPath, spriteSheetPath.c_str());
                m_spriteSheetLoaded = m_spriteSheet.Load(
                    context.assets,
                    context.renderer,
                    spriteSheetPath
                );
            } else {
                CopyText(m_spriteSheetPath, m_tileMap.SpriteSheetPath().c_str());
                m_spriteSheetLoaded = false;
            }
        }

        RebuildPaletteFromTileMap();
        CenterCameraOnMap();
        SetStatus(m_spriteSheetLoaded ? "Map opened" : "Map opened; SpriteSheet load failed");
        return true;
    }

    bool EditorScene::SaveMap() noexcept {
        if (!m_mapReady) {
            SetStatus("No map to save", true);
            return false;
        }

        if (!m_tileMap.Save(BufferText(m_mapPath))) {
            SetStatus("Save map failed", true);
            return false;
        }

        SetStatus("Map saved");
        return true;
    }

    void EditorScene::ConfigureDefaultPaths() noexcept {
        const std::string workspace = grimar::platform::JoinPath(
            grimar::platform::UserDocumentsDirectory(),
            "GrimarEditor"
        );

        CopyText(m_mapPath, grimar::platform::JoinPath(
            grimar::platform::JoinPath(workspace, "maps"),
            "untitled.tilemap.json"
        ).c_str());
        CopyText(m_generatedSpriteSheetPath, grimar::platform::JoinPath(
            grimar::platform::JoinPath(workspace, "spritesheets"),
            "generated.sprites.json"
        ).c_str());
        CopyResolvedOrRaw(m_spriteSheetPath, "assets/test.sprites.json");
        CopyResolvedOrRaw(m_texturePath, "assets/test.png");
        CopyText(m_spritePrefix, "tile");
    }

    void EditorScene::ApplyPresetSmall() noexcept {
        m_newMapWidth = 16;
        m_newMapHeight = 10;
    }

    void EditorScene::ApplyPresetPlatformer() noexcept {
        m_newMapWidth = 32;
        m_newMapHeight = 18;
    }

    void EditorScene::ApplyPresetLarge() noexcept {
        m_newMapWidth = 64;
        m_newMapHeight = 36;
    }

    void EditorScene::RebuildPaletteFromSpriteSheet() noexcept {
        std::vector<std::string> frameNames{};
        m_spriteSheet.ForEachFrame([&](const std::string& name,
                                       const grimar::assets::SpriteFrame&) {
            frameNames.push_back(name);
        });
        std::sort(frameNames.begin(), frameNames.end());

        m_palette.clear();
        grimar::assets::TileId id = 1;
        for (const auto& name : frameNames) {
            grimar::assets::TileDefinition definition{};
            definition.spriteName = name;
            definition.solid = true;

            m_tileMap.SetDefinition(id, definition);
            m_palette.push_back(PaletteEntry{id, name, true});
            ++id;
        }

        if (!m_palette.empty()) {
            m_selectedTile = m_palette.front().id;
        }
    }

    void EditorScene::RebuildPaletteFromTileMap() noexcept {
        m_palette.clear();
        m_tileMap.ForEachDefinition([&](grimar::assets::TileId id,
                                        const grimar::assets::TileDefinition& definition) {
            m_palette.push_back(PaletteEntry{id, definition.spriteName, definition.solid});
        });

        std::sort(m_palette.begin(), m_palette.end(),
                  [](const PaletteEntry& a, const PaletteEntry& b) {
                      return a.id < b.id;
                  });

        if (!m_palette.empty()) {
            m_selectedTile = m_palette.front().id;
        }
    }

    void EditorScene::HandleShortcuts(grimar::engine::SceneContext& context) noexcept {
        using grimar::platform::Key;

        if (ImGuiWantsKeyboard()) {
            return;
        }

        if (context.input.IsKeyDown(Key::Ctrl) && context.input.WasKeyPressed(Key::S)) {
            SaveMap();
        }
        if (context.input.IsKeyDown(Key::Ctrl) && context.input.WasKeyPressed(Key::O)) {
            OpenMap(context);
        }
        if (context.input.WasKeyPressed(Key::B)) {
            SetTool(Tool::Paint);
        }
        if (context.input.WasKeyPressed(Key::E)) {
            SetTool(Tool::Erase);
        }
        if (context.input.WasKeyPressed(Key::I)) {
            SetTool(Tool::Picker);
        }
        if (context.input.WasKeyPressed(Key::R)) {
            SetTool(Tool::RectFill);
        }
    }

    void EditorScene::HandleCamera(grimar::engine::SceneContext& context, double dt) noexcept {
        using grimar::platform::Key;
        using grimar::platform::MouseButton;

        auto pos = m_camera.Position();
        const float speed = 520.f * static_cast<float>(dt);

        if (!ImGuiWantsKeyboard()) {
            if (context.input.IsKeyDown(Key::A)) pos.x -= speed;
            if (context.input.IsKeyDown(Key::D)) pos.x += speed;
            if (context.input.IsKeyDown(Key::W)) pos.y += speed;
            if (context.input.IsKeyDown(Key::S)) pos.y -= speed;
        }

        const auto mouse = context.input.MousePosition();
        if (context.input.IsMouseButtonDown(MouseButton::Middle) && !ImGuiWantsMouse()) {
            if (m_hasLastMousePosition) {
                const grimar::core::Vec2f delta{
                    mouse.x - m_lastMousePosition.x,
                    mouse.y - m_lastMousePosition.y
                };
                pos.x -= delta.x / m_camera.Zoom();
                pos.y += delta.y / m_camera.Zoom();
            }
        }
        m_lastMousePosition = mouse;
        m_hasLastMousePosition = true;

        float zoom = m_camera.Zoom();
        const auto wheel = context.input.MouseWheel();
        if (!ImGuiWantsMouse() && wheel.y != 0.f) {
            zoom *= (wheel.y > 0.f) ? 1.1f : 0.9f;
        }

        if (zoom < 0.20f) zoom = 0.20f;
        if (zoom > 6.00f) zoom = 6.00f;

        m_camera.SetPosition(pos);
        m_camera.SetZoom(zoom);
    }

    void EditorScene::HandleCanvas(grimar::engine::SceneContext& context) noexcept {
        using grimar::platform::Key;
        using grimar::platform::MouseButton;

        if (ImGuiWantsMouse() ||
            context.input.IsMouseButtonDown(MouseButton::Middle) ||
            !m_mapReady ||
            m_tileMap.Width() <= 0 ||
            m_tileMap.Height() <= 0) {
            return;
        }

        int tileX = 0;
        int tileY = 0;
        if (!MouseTile(context, tileX, tileY)) {
            return;
        }

        if (context.input.IsMouseButtonDown(MouseButton::Right)) {
            PaintBrush(tileX, tileY, 0);
            m_rectFillActive = false;
            return;
        }

        if (context.input.IsKeyDown(Key::Alt) &&
            context.input.WasMouseButtonPressed(MouseButton::Left)) {
            m_selectedTile = m_tileMap.TileAt(tileX, tileY);
            if (m_selectedTile == 0 && HasPalette()) {
                m_selectedTile = m_palette.front().id;
            }
            SetTool(Tool::Paint);
            return;
        }

        if (!context.input.IsMouseButtonDown(MouseButton::Left) &&
            !context.input.WasMouseButtonPressed(MouseButton::Left)) {
            return;
        }

        if (m_tool == Tool::Paint) {
            PaintBrush(tileX, tileY, m_selectedTile);
        } else if (m_tool == Tool::Erase) {
            PaintBrush(tileX, tileY, 0);
        } else if (m_tool == Tool::Picker && context.input.WasMouseButtonPressed(MouseButton::Left)) {
            m_selectedTile = m_tileMap.TileAt(tileX, tileY);
            if (m_selectedTile == 0 && HasPalette()) {
                m_selectedTile = m_palette.front().id;
            }
            SetTool(Tool::Paint);
        } else if (m_tool == Tool::RectFill && context.input.WasMouseButtonPressed(MouseButton::Left)) {
            if (!m_rectFillActive) {
                m_rectStartX = tileX;
                m_rectStartY = tileY;
                m_rectFillActive = true;
                SetStatus("Rect fill: choose end tile");
            } else {
                FillRect(m_rectStartX, m_rectStartY, tileX, tileY, m_selectedTile);
                m_rectFillActive = false;
                SetStatus("Rect fill applied");
            }
        }
    }

    void EditorScene::RenderToolbar(grimar::engine::SceneContext& context) noexcept {
        const ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, ToolbarHeight), ImGuiCond_Always);
        ImGui::Begin("EditorToolbar",
                     nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse);

        if (ImGui::Button("New")) {
            m_showNewMapWizard = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
            OpenMap(context);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            SaveMap();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save As")) {
            SaveMap();
        }
        ImGui::SameLine();
        if (ImGui::Button("Center")) {
            CenterCameraOnMap();
        }

        ImGui::SameLine();
        ImGui::Text("|");
        ImGui::SameLine();

        if (HighlightButton("Paint [B]", m_tool == Tool::Paint)) {
            SetTool(Tool::Paint);
        }
        ImGui::SameLine();
        if (HighlightButton("Erase [E]", m_tool == Tool::Erase)) {
            SetTool(Tool::Erase);
        }
        ImGui::SameLine();
        if (HighlightButton("Picker [I]", m_tool == Tool::Picker)) {
            SetTool(Tool::Picker);
        }
        ImGui::SameLine();
        if (HighlightButton("Rect [R]", m_tool == Tool::RectFill)) {
            SetTool(Tool::RectFill);
        }

        ImGui::SameLine();
        ImGui::Checkbox("Grid", &m_showGrid);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80.f);
        ImGui::InputInt("Brush", &m_brushSize);
        if (m_brushSize < 1) m_brushSize = 1;
        if (m_brushSize > 16) m_brushSize = 16;

        ImGui::SameLine();
        ImGui::Text("Tool: %s", ToolName());

        ImGui::End();
    }

    void EditorScene::RenderPalettePanel() noexcept {
        const ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0.f, ToolbarHeight), ImGuiCond_Always);
        ImGui::SetNextWindowSize(
            ImVec2(LeftPanelWidth, io.DisplaySize.y - ToolbarHeight),
            ImGuiCond_Always
        );
        ImGui::Begin("Palette",
                     nullptr,
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse);

        RenderSelectedTilePreview();
        ImGui::Separator();

        if (!HasPalette()) {
            ImGui::TextWrapped("No tiles loaded. Import a SpriteSheet or PNG grid from the Project panel.");
        }

        ImGui::BeginChild("TilePaletteList", ImVec2(0.f, 0.f), true);
        for (const auto& entry : m_palette) {
            char label[160]{};
            std::snprintf(label, sizeof(label), "%d  %s", entry.id, entry.spriteName.c_str());
            if (ImGui::Selectable(label, entry.id == m_selectedTile)) {
                m_selectedTile = entry.id;
                SetTool(Tool::Paint);
            }
        }
        ImGui::EndChild();

        ImGui::End();
    }

    void EditorScene::RenderSettingsPanel(grimar::engine::SceneContext& context) noexcept {
        const ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(
            ImVec2(io.DisplaySize.x - RightPanelWidth, ToolbarHeight),
            ImGuiCond_Always
        );
        ImGui::SetNextWindowSize(
            ImVec2(RightPanelWidth, io.DisplaySize.y - ToolbarHeight),
            ImGuiCond_Always
        );
        ImGui::Begin("Project",
                     nullptr,
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse);

        if (m_statusIsError) {
            ImGui::TextColored(ImVec4(1.f, 0.32f, 0.25f, 1.f), "Status: %s", m_status.data());
        } else {
            ImGui::TextWrapped("Status: %s", m_status.data());
        }

        ImGui::Separator();
        ImGui::Text("Map");
        ImGui::InputText("Map Path", m_mapPath.data(), m_mapPath.size());
        ImGui::Text("Size: %dx%d  Tile: %dx%d",
                    m_tileMap.Width(),
                    m_tileMap.Height(),
                    m_tileMap.TileWidth(),
                    m_tileMap.TileHeight());

        ImGui::Separator();
        ImGui::Text("Sprite Import");
        if (ImGui::BeginTabBar("SpriteImportTabs")) {
            if (ImGui::BeginTabItem("PNG Grid")) {
                ImGui::InputText("PNG Texture", m_texturePath.data(), m_texturePath.size());
                ImGui::InputText("Generated JSON",
                                 m_generatedSpriteSheetPath.data(),
                                 m_generatedSpriteSheetPath.size());
                ImGui::InputText("Prefix", m_spritePrefix.data(), m_spritePrefix.size());
                ImGui::InputInt("Grid W", &m_gridTileWidth);
                ImGui::InputInt("Grid H", &m_gridTileHeight);
                if (ImGui::Button("Generate + Load")) {
                    GenerateGridSpriteSheet(context);
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("SpriteSheet JSON")) {
                ImGui::InputText("SpriteSheet JSON",
                                 m_spriteSheetPath.data(),
                                 m_spriteSheetPath.size());
                if (ImGui::Button("Load SpriteSheet")) {
                    LoadSpriteSheet(context);
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::Separator();
        ImGui::Text("Map Settings");
        if (ImGui::Button("Small")) ApplyPresetSmall();
        ImGui::SameLine();
        if (ImGui::Button("Platformer")) ApplyPresetPlatformer();
        ImGui::SameLine();
        if (ImGui::Button("Large")) ApplyPresetLarge();
        ImGui::Checkbox("Advanced", &m_showAdvancedMapSettings);
        if (m_showAdvancedMapSettings) {
            ImGui::InputInt("Width", &m_newMapWidth);
            ImGui::InputInt("Height", &m_newMapHeight);
            ImGui::InputInt("Tile W", &m_newTileWidth);
            ImGui::InputInt("Tile H", &m_newTileHeight);
        }
        if (ImGui::Button("Create New Map")) {
            NewMap();
        }

        ImGui::Separator();
        ImGui::TextWrapped("Controls: LMB paint, RMB erase, Alt+LMB pick, MMB pan, wheel zoom. Ctrl+S save.");

        ImGui::End();
    }

    void EditorScene::RenderNewMapWizard() noexcept {
        if (!m_showNewMapWizard) {
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(460.f, 360.f), ImGuiCond_FirstUseEver);
        ImGui::Begin("New Map Wizard", &m_showNewMapWizard, ImGuiWindowFlags_NoCollapse);

        ImGui::TextWrapped("Import a SpriteSheet or PNG grid, choose a preset, then create a map.");
        ImGui::Separator();

        if (ImGui::Button("Small Test 16x10")) ApplyPresetSmall();
        ImGui::SameLine();
        if (ImGui::Button("Platformer 32x18")) ApplyPresetPlatformer();
        ImGui::SameLine();
        if (ImGui::Button("Large 64x36")) ApplyPresetLarge();

        ImGui::InputInt("Width", &m_newMapWidth);
        ImGui::InputInt("Height", &m_newMapHeight);
        ImGui::InputInt("Tile W", &m_newTileWidth);
        ImGui::InputInt("Tile H", &m_newTileHeight);
        ImGui::InputText("Map Path", m_mapPath.data(), m_mapPath.size());

        ImGui::Separator();
        ImGui::Text("Sprite source: %s", m_spriteSheetLoaded ? "loaded" : "not loaded");
        ImGui::Text("Palette tiles: %zu", m_palette.size());

        if (ImGui::Button("Create Map")) {
            if (NewMap()) {
                m_showNewMapWizard = false;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            m_showNewMapWizard = false;
        }

        ImGui::End();
    }

    void EditorScene::RenderSelectedTilePreview() noexcept {
        ImGui::Text("Selected: %d", m_selectedTile);

        const auto selectedIt = std::find_if(
            m_palette.begin(),
            m_palette.end(),
            [&](const PaletteEntry& entry) {
                return entry.id == m_selectedTile;
            });

        if (selectedIt == m_palette.end()) {
            ImGui::TextWrapped("No tile selected");
            return;
        }

        ImGui::TextWrapped("%s", selectedIt->spriteName.c_str());

        const auto* frame = m_spriteSheet.GetFrame(selectedIt->spriteName);
        if (!frame || !frame->texture || !frame->texture->NativeTexture()) {
            return;
        }

        const auto textureW = static_cast<float>(frame->texture->Width());
        const auto textureH = static_cast<float>(frame->texture->Height());
        const ImVec2 uv0{
            static_cast<float>(frame->srcRect.x) / textureW,
            static_cast<float>(frame->srcRect.y) / textureH
        };
        const ImVec2 uv1{
            static_cast<float>(frame->srcRect.x + frame->srcRect.w) / textureW,
            static_cast<float>(frame->srcRect.y + frame->srcRect.h) / textureH
        };

        ImGui::Image(
            ImTextureRef(static_cast<void*>(frame->texture->NativeTexture())),
            ImVec2(96.f, 96.f),
            uv0,
            uv1
        );
    }

    void EditorScene::RenderGrid(grimar::render::Renderer2D& renderer) const noexcept {
        if (m_tileMap.Width() <= 0 || m_tileMap.Height() <= 0) {
            return;
        }

        const float tileW = static_cast<float>(m_tileMap.TileWidth());
        const float tileH = static_cast<float>(m_tileMap.TileHeight());
        const float left = m_tileMapOrigin.x;
        const float bottom = m_tileMapOrigin.y;
        const float right = left + (static_cast<float>(m_tileMap.Width()) * tileW);
        const float top = bottom + (static_cast<float>(m_tileMap.Height()) * tileH);
        const grimar::render::Color gridColor{80, 180, 220, 110};

        for (int x = 0; x <= m_tileMap.Width(); ++x) {
            const float wx = left + (static_cast<float>(x) * tileW);
            renderer.DrawLine({wx, bottom}, {wx, top}, gridColor, 1000);
        }

        for (int y = 0; y <= m_tileMap.Height(); ++y) {
            const float wy = bottom + (static_cast<float>(y) * tileH);
            renderer.DrawLine({left, wy}, {right, wy}, gridColor, 1000);
        }
    }

    void EditorScene::SetTool(Tool tool) noexcept {
        m_tool = tool;
        if (m_tool != Tool::RectFill) {
            m_rectFillActive = false;
        }
    }

    void EditorScene::PaintBrush(int tileX,
                                 int tileY,
                                 grimar::assets::TileId id) noexcept {
        const int radius = m_brushSize / 2;
        const int startX = tileX - radius;
        const int startY = tileY - radius;

        for (int y = 0; y < m_brushSize; ++y) {
            for (int x = 0; x < m_brushSize; ++x) {
                m_tileMap.SetTile(startX + x, startY + y, id);
            }
        }
    }

    void EditorScene::FillRect(int ax,
                               int ay,
                               int bx,
                               int by,
                               grimar::assets::TileId id) noexcept {
        const int minX = (ax < bx) ? ax : bx;
        const int maxX = (ax > bx) ? ax : bx;
        const int minY = (ay < by) ? ay : by;
        const int maxY = (ay > by) ? ay : by;

        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                m_tileMap.SetTile(x, y, id);
            }
        }
    }

    bool EditorScene::MouseTile(grimar::engine::SceneContext& context,
                                int& tileX,
                                int& tileY) const noexcept {
        const auto mouse = context.input.MousePosition();
        const auto world = m_camera.ScreenToWorld(mouse);
        const float tileW = static_cast<float>(m_tileMap.TileWidth());
        const float tileH = static_cast<float>(m_tileMap.TileHeight());

        if (tileW <= 0.f || tileH <= 0.f) {
            return false;
        }

        tileX = static_cast<int>(std::floor((world.x - m_tileMapOrigin.x) / tileW));
        tileY = static_cast<int>(std::floor((world.y - m_tileMapOrigin.y) / tileH));

        return tileX >= 0 &&
               tileY >= 0 &&
               tileX < m_tileMap.Width() &&
               tileY < m_tileMap.Height();
    }

    void EditorScene::CenterCameraOnMap() noexcept {
        if (!m_mapReady) {
            m_camera.SetPosition({0.f, 0.f});
            return;
        }

        const float width = static_cast<float>(m_tileMap.Width() * m_tileMap.TileWidth());
        const float height = static_cast<float>(m_tileMap.Height() * m_tileMap.TileHeight());
        m_camera.SetPosition({
            m_tileMapOrigin.x + (width * 0.5f),
            m_tileMapOrigin.y + (height * 0.5f)
        });
    }

    void EditorScene::SetStatus(const char* text, bool error) noexcept {
        CopyText(m_status, text);
        m_statusIsError = error;
    }

    const char* EditorScene::ToolName() const noexcept {
        switch (m_tool) {
            case Tool::Paint:    return "Paint";
            case Tool::Erase:    return "Erase";
            case Tool::Picker:   return "Picker";
            case Tool::RectFill: return "Rect Fill";
        }

        return "Unknown";
    }
}
