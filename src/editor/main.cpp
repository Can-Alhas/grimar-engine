// ~ Grimar Engine ~

#include <memory>

#include "editor/EditorScene.hpp"
#include "grimar/engine/EngineApp.hpp"

int main() {
    grimar::engine::EngineConfig config{};
    config.windowWidth = 1440;
    config.windowHeight = 900;
    config.windowTitle = "Grimar Editor";

    grimar::engine::EngineApp app{config};
    app.SetScene(std::make_unique<grimar::editor::EditorScene>());

    if (!app.Init()) {
        return 1;
    }

    return app.Run();
}
