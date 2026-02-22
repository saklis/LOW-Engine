#pragma once

#include <filesystem>
#include <imgui.h>
#include <imgui-SFML.h>
#include <typeindex>

#include "ComponentEditorBinding.h"

namespace LowEngine {

	namespace Terrain {
        class Layer;
    }

    class Game;
    class Scene;

    class DevTools {
    public:
        static std::vector<ComponentEditorBinding> ComponentEditorBindings;
        
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

        static inline bool _isNewProjectWizardVisible = true;
        static inline bool _isInputEditorVisible = false;
        static inline bool _isAssetBrowserVisible = false;

        static inline std::string _actionBeingBound = "";
        static inline bool _isWaitingForInput = false;

        static void DisplayLog(size_t posX, size_t posY, size_t sizeX, size_t sizeY);

        static void DisplayMainMenu(LowEngine::Game& game);

        static void DisplayProjectWizard(Game& game);

        static void DisplayToolbar(Game& game, const sf::Vector2u& displaySize, float YOffset);

        static void DisplayWorldOutliner(Scene* scene, int posX, int posY, int width, int height);

        static void DisplayProperties(Scene* scene, int posX, int posY, int width, int height);

        static void DisplayTransformComponentProperties(Scene& scene);

        static void DisplayAnimatedSpriteComponentProperties(Scene& scene);

        static void DisplayCameraComponentProperties(Scene& scene);

        static void DisplayColliderComponentProperties(Scene& scene);

        static void DisplayInputEditor(Game& game);

        static void DisplayAssetBrowser(Game& game);

        static void DisplayTextureBrowser(Game& game, ImVec2 size);
        static void DisplaySpriteSheetBrowser(Game& game, ImVec2 size);
        static void DisplayAnimationClipsBrowser(Game& game, ImVec2 size);
        static void DisplayTileMapBrowser(const Game& game, ImVec2 size);
        static void ShowTileMapEditor(const Game& game, std::string& tileMapAlias, const std::filesystem::path& newTileMapFile, bool& showEditor);
        static void ShowTileMapLayerEditor(const Game& game, Terrain::Layer& layer);
        static void DisplaySoundBrowser(const Game& game, ImVec2 size);
        static void DisplayMusicBrowser(Game& game, ImVec2 size);

        static void CreateNewProject(Game& game, const std::string& projectName, const std::string& projectPath);

        static std::string InsertSpaces(const std::string& str);
        static void InitStaticCharArray(char* arr, size_t length, const std::string& value);

        static sf::Texture playTexture;
        static sf::Texture pauseTexture;
        static sf::Texture stopTexture;
        static sf::Texture nextTexture;
        static sf::Texture loopTexture;
        static sf::Texture trashTexture;
        static sf::Texture projectIconTexture;
        static sf::Texture soundIconTexture;
        static sf::Texture musicIconTexture;

        static bool LoadIcons();
    };
}
