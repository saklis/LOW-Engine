#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "scene/Scene.h"

namespace LowEngine {
    class Game;

    /**
     * @brief Scene Manager keeps a list of all existing scenes, alloes creation of new onse and switching between them.
     */
    class SceneManager {
    public:
        SceneManager();

        ~SceneManager() = default;

        /**
         * @brief Create new empty scene.
         *
         * New scene will not be automatically set as active scene.
         * @param name Name of the new scene.
         * @return Pointer to the new empty scene. Returns nullptr in case of error.
         */
        Scene* CreateEmptyScene(const std::string& name);

        /**
         * @brief Create new default scene with provided name.
         *
         * New scene will not be automatically set as active scene.
         * @param name Name of the new scene.
         * @return Pointer to the new scene. Returns nullptr in case of error.
         */
        Scene* CreateScene(const std::string& name);

        /**
         * @brief Create default scene.
         *
         * Default scene is created during engine initialization and is used as a fallback scene.
         * @return Pointer to the default scene. Returns nullptr in case of error.
		 */
        Scene* CreateDefaultScene();

        /**
         * @brief Create a deep copy of current scene.
         * @param nameSufix Suffix that will be added to the scene's Name.
         * @return Id of the new scene.
         */
        size_t CreateCopySceneFromCurrent(const std::string& nameSufix);

        /**
         * @brief Set scene with provided Id as 'current'
         * @param index Id of the scene.
         * @return True if current scene was changed. Returns false in case of error.
         */
        bool SelectScene(size_t index);

        /**
         * @brief Set scene with provided name as 'current'
         * @param name Name of the scene.
         * @return True if current scene was changed. Returns false in case of error.
         */
        bool SelectScene(const std::string& name);

        /**
         * @brief Set scene with provided pointer as 'current'.
         * @param scene Pointer to the scene.
         * @return True if current scene was changed. Returns false in case of error.
		 */
        bool SelectScene(const Scene* scene);

        /**
         * @brief Retrieve pointer to scene by its index.
         * @param index Index of the scene.
         * @return Pointer to scene. Returns nullptr in case of error.
		 */
		Scene* GetScene(size_t index);

        /**
         * @brief Retrieve pointer to scene that is marked as 'current'.
         * @return Pointer to scene. Returns nullptr in case of error.
         */
        Scene* GetCurrentScene();

        /**
         * @brief Retrieve const reference to the scene that is marked as 'current'.
         *
         *
         * @return
         */
        const Scene& GetCurrentScene() const;

        /**
         * @brief Check if default scene exists.
         *
         * Default scene is created during engine initialization and is used as a fallback scene.
         * @return True if default scene exists, false otherwise.
		 */
        bool DefaultSceneExists() const;

        /**
         * @brief Destroy current scene.
         *
         * Scene lower on the "stack" will be marked as current.
         */
        void DestroyCurrentScene();

        /**
         * @brief Destroy all scenes.
         */
        void DestroyAll();

    protected:
        std::vector<std::unique_ptr<Scene>> _scenes;
        size_t _currentSceneIndex;
    };
}
