#pragma once

#include <fstream>
#include <sstream>

#include <imgui.h>
#include <imgui-SFML.h>
#include <ImGuiFileDialog.h>

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

        static inline bool _isAssetBrowserVisible = false;
        static inline bool _isNewProjectWizardVisible = false;

        static void DisplayLog(size_t posX, size_t posY, size_t sizeX, size_t sizeY);

        static void DisplayMainMenu(LowEngine::Game& game);

        static void DisplayProjectWizard();

        static void DisplayToolbar(Game& game, const sf::Vector2u& displaySize, float YOffset);

        static void DisplayWorldOutliner(Scene* scene, int posX, int posY, int width, int height);

        static void DisplayProperties(Scene* scene, int posX, int posY, int width, int height);

        static void DisplayTransformComponentProperties(Scene& scene);

        static void DisplayAnimatedSpriteComponentProperties(Scene& scene);

        static void DisplayCameraComponentProperties(Scene& scene);

        static void DisplayAssetBrowser(Game& game);

        static std::string InsertSpaces(const std::string& str);

        static sf::Texture playTexture;
        static sf::Texture pauseTexture;
        static sf::Texture stopTexture;

        static bool LoadIcons();
    };
}
