#pragma once

#include <string>
#include <typeindex>
#include <vector>
#include <filesystem>

#include <imgui-SFML.h>

#include "ComponentEditorBinding.h"
#include "terrain/Tile.h"
#include "tools/TerrainBrushManager.h"

namespace LowEngine {

    class Game;
    class Scene;

    class DevTools {
    public:
        static inline sf::Time DeltaTime = sf::Time::Zero;

        static std::vector<ComponentEditorBinding> ComponentEditorBindings;

        static bool Initialize(sf::RenderWindow& window);

        static void Free();

        static void ReadInput(const sf::RenderWindow& window, const std::optional<sf::Event>& event);

        static void Update(sf::RenderWindow& window, sf::Time deltaTime);

        static void Build(Game& game);

        static void Render(sf::RenderWindow& window);

        static bool LoadProject(Game& game, const std::filesystem::path& projectFilePath);
        static bool SaveProject(Game& game, const std::filesystem::path& projectFilePath);
        static void CloseProject(Game& game);

    protected:
        static inline Scene* _currentScene = nullptr;

        static inline size_t _selectedEntityId = -1;
        static inline void* _selectedComponent = nullptr;
        static inline std::type_index _selectedComponentType = typeid(void);
        static inline size_t _selectedTerrainLayerIndex = -1;
        static inline sf::IntRect _selectedTerrainLayerTile;
        static inline TileMap::TileType _selectedTerrainLayerTileType = TileMap::TileType::Static;
        static inline std::string _selectedAnimClipName;

        static inline Tools::TerrainBrushManager _terrainBrushManager;
        static inline std::uint8_t _brushTraversalMask = TileMap::Traversal::None;
        static inline std::uint8_t _brushEntryCost = 1;
        static inline bool _brushHasCollision = false;
        static inline size_t _selectedTerrainBrushIndex = -1;
        static inline bool _navOverlayVisible = false;

        static inline bool _isInTerrainEditMode = false;
        static inline bool _isNewProjectWizardVisible = true;
        static inline bool _isInputEditorVisible = false;
        static inline bool _isAssetBrowserVisible = false;

        static inline std::string _actionBeingBound;
        static inline bool _isWaitingForInput = false;

        static inline int _resetLayoutFrames = 0;
        static inline bool _saveLayoutAfterReset = false;

        static void BuildDockingHost();
        static void ClearSpecialInputActions();

        static void DisplayMainMenu(Game& game);
    };
}
