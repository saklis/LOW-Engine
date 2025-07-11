// #pragma once
//
// #include <TGUI/TGUI.hpp>
// #include <TGUI/Backend/SFML-Graphics.hpp>
//
// #include "Game.h"
//
// namespace LowEngine {
//     class DevTools {
//     public:
//         static void Initialize(sf::RenderWindow& window);
//
//         static void ReadInput(const std::optional<sf::Event>& event);
//
//         static void Draw(sf::RenderWindow& window);
//
//         static void Free();
//
//         static void Build(Game& game);
//         static void Update(Game& game);
//
//     protected:
//         static inline const std::string SCENEOUTLINER = "DevTools_SceneOutlinerWindow";
//         static inline const std::string SCENEOUTLINER_ENTITIESLISTBOX = "DevTools_SceneOutlinerWindow";
//
//         static void BuildMainMenuBar();
//         static void BuildMainToolbar();
//         static void BuildSceneOutliner(Game& game);
//
//         static void UpdateSceneOutliner(Game& game);
//
//         static tgui::Gui* GetInstance() {
//             static tgui::Gui instance;
//             return &instance;
//         }
//
//         DevTools(){};
//         DevTools(const DevTools& other) = delete;
//         DevTools& operator=(const DevTools& other) = delete;
//         ~DevTools() = default;
//     };
// }
//
//

#pragma once

#include <fstream>
#include <sstream>

#include <imgui.h>
#include <imgui-SFML.h>

#include "Game.h"

namespace LowEngine {
    class DevTools {
    public:
        static bool Initialize(sf::RenderWindow& window);

        static void Free();

        static void ReadInput(const sf::RenderWindow& window, const std::optional<sf::Event>& event);

        static void Update(sf::RenderWindow& window, sf::Time deltaTime);

        static void Build(Game& game);

        static void Render(sf::RenderWindow& window);

    protected:
        static inline size_t _selectedEntityId = -1;
        static inline void* _selectedComponent = nullptr;
        static inline std::type_index _selectedComponentType = typeid(void);

        static void DisplayLog(size_t posX, size_t posY, size_t sizeX, size_t sizeY);

        static void DisplayMenuBar(LowEngine::Game& game);

        static void DisplayToolbar(Game& game, const sf::Vector2u& displaySize, float YOffset);

        static void DisplayWorldOutliner(Scene* scene, int posX, int posY, int width, int height);

        static void DisplayProperties(Scene* scene, int posX, int posY, int width, int height);

        static void DisplayTransformComponentProperties(Scene& scene);

        static void DisplayAnimatedSpriteComponentProperties(Scene& scene);

        static void DisplayCameraComponentProperties(Scene& scene);

        static std::string InsertSpaces(const std::string& str);

        static sf::Texture playTexture;
        static sf::Texture pauseTexture;
        static sf::Texture stopTexture;

        static bool LoadIcons();
    };
}
