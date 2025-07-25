#pragma once

#include <string>

#include "SFML/Graphics/RenderWindow.hpp"

#include "memory/Memory.h"
#include "ecs/ECSHeaders.h"
#include "TGUI/Event.hpp"

namespace LowEngine {
    /**
     * @brief Represents a scene containing entities.
     *
     * It provides functionality to add, remove, and manipulate elements in the scene.
     *
     * The scene acts as a container for all elements that need to
     * be rendered or processed, handling their relationships, updates,
     * and visibility.
     */
    class Scene {
    public:
        /**
         * @brief Defines methods for sorting sprites in a scene.
         *
         * Determines how sprites are sorted during the rendering process, which
         * can affect the visual layering and order of sprites on the screen.
         *
         * The available sorting methods include:
         * - `None`: No sorting is performed on sprites.
         * - `Layers`: Sprites are sorted based on their assigned layer values.
         * - `YAxisIncremental`: Sprites are sorted based on the y-axis position, with smaller y-values being rendered first.
         */
        enum class SpriteSortingMethod {
            /**
             * @brief No sorting is performed on sprites.
             */
            None,
            /**
             * @brief Sprites are sorted based on their assigned layer values.
             */
            Layers,
            /**
             * @brief Sprites are sorted based on the y-axis position, with smaller y-values being rendered first.
             */
            YAxisIncremental
        };

        /**
         * @brief Is the scene active?
         *
         * Scenes that are not active cannot be set a 'current'.
         */
        bool Initialized = false;

        /**
         * @brief Is this scene Temporary?
         *
         * Scenes created as copies of existing scenes are marked as Temporary and should be deleted before switching to different scene.
         */
        bool IsTemporary = false;

        /**
         * @brief Is this scene paused?
         *
         * Paused scenes do not update their entities and components
         */
        bool IsPaused = false;

        /**
         * @brief Name of this scene.
         */
        std::string Name;

        Scene() = default;

        Scene(Scene const& other, const std::string& nameSufix = " (COPY)");

        explicit Scene(const std::string& name);

        /**
         * @brief INTERNAL: Initialize scene as default.
         *
         * Default scene is used during initial engine load.
         */
        void InitAsDefault();

        /**
         * @brief Update all Entities and Components.
         * @param deltaTime Time passed since last updae, in seconds.
         */
        void Update(float deltaTime);

        /**
         * @brief Draw all sprites for this scene.
         * @param window Window to draw on.
         */
        void Draw(sf::RenderWindow& window);

        /**
         * @brief Add new Entity to this scene.
         * @param name Name of the new scene.
         * @return Pointer to new scene. Returns nullptr in case of error.
         */
        ECS::Entity* AddEntity(const std::string& name = "Entity");

        /**
         * @brief Check if Entity with provided Id is safe to destroy.
         * @param entityId Id of the Entity to check.
         * @return True if Entity is safe to destroy, False otherwise.
         */
        bool IsEntitySafeToDestroy(size_t entityId) const;

        /**
         * @brief Destroy Entity with provided Id.
         * @param entityId Id of the Entity to destroy.
         *
         * This method will remove all Components owned by Entity and then remove the Entity itself.
         */
        void DestroyEntity(size_t entityId);

        /**
         * @brief Retrieve pointer to Entity with provided Id.
         * @param entityId Id of the Entity.
         * @return Pointer to Entity. Return nullptr if Entity not found.
         */
        ECS::Entity* GetEntity(unsigned int entityId);

        /**
         * @brief Find pointer to Entity with provided Name.
         *
         * If there's multiple Entities with the same name, the first one found will be retrieved.
         * @param name Name of the Entity.
         * @return Pointer to Entity. Return nullptr if Entity not found.
         */
        ECS::Entity* FindEntity(const std::string& name);

        /**
         * @brief Retreieve pointer to collection of all Entities in this scene.
         * @return Pointer to Entities' collection.
         */
        std::vector<std::unique_ptr<ECS::IEntity> >* GetEntities();

        /**
         * @brief Add new Component to the Entity in this scene.
         * @tparam T Type of the component to add.
         * @tparam Args List of types that will be passed to Component's constructor.
         * @param entityId Id of the Entity that should own new Component.
         * @param args List of arguments that should be passed to Component's constructor.
         * @return Pointer to new Component. Returns nullptr in case of error.
         */
        template<typename T, typename... Args>
        T* AddComponent(size_t entityId, Args&&... args) {
            return _memory.CreateComponent<T>(entityId, std::forward<Args>(args)...);
        }

        /**
         * @brief Check if Component of given type is safe to destroy.
         * @param entityId Id of the Entity that owns Component.
         * @return True if Component is safe to destroy, False otherwise.
         */
        template<typename T>
        bool IsComponentSafeToDestroy(size_t entityId) {
            if (typeid(T) == typeid(ECS::CameraComponent)) {
                if (this->_cameraEntityId == entityId) return false;
            }

            return _memory.IsComponentSafeToDestroy<T>(entityId);
        }

        /**
         * @brief Remove Component from Entity in this scene.
         * @tparam T Type of the component to remove.
         * @param entityId Id of the Entity that should no longer own Component.
         *
         * This method will remove Component from Entity and destroy it.
         */
        template<typename T>
        void DestroyComponent(size_t entityId) {
            _memory.DestroyComponent<T>(entityId);
        }

        /**
         * @brief Retrieve pointer to Component owned by provided Entity.
         * @tparam T Type of component to retrieve.
         * @param entityId Id of the Entity.
         * @return Pointer to Component. Returns nullptr in case of error.
         */
        template<typename T>
        T* GetComponent(size_t entityId) {
            return _memory.GetComponent<T>(entityId);
        }

        /**
         * @brief Retrieve pointer to Component owned by provided Entity.
         * @param entityId Id of the Entity.
         * @param typeIndex Type of the component to retrieve.
         * @return Pointer to Component. Returns nullptr in case of error.
         */
        void* GetComponent(size_t entityId, std::type_index typeIndex);

        /**
         * @brief Set provided Entity as one that manages current View.
         *
         * Entity must have CameraComponent attached.
         * @param entityId Id of the Entity.
         * @return True if successful. Returns False in case of error.
         */
        bool SetCurrentCamera(size_t entityId);

        /**
         * @brief Get current camera entity.
         * @return Pointer to current camera Entity. Returns nullptr if no camera is set.
         */
        ECS::Entity* GetCurrentCamera();

        /**
         * @brief Inform current camera entity that window size changed.
         * @param windowSize New window size.
         */
        void SetWindowSize(sf::Vector2f windowSize);

        /**
         * @brief Set new sprite sorting method for this scene.
         * @param method New sprite sorting method.
         */
        void SetSpriteSorting(SpriteSortingMethod method);

        /**
         * @brief Destroy this scene.
         */
        void Destroy();

    protected:
        size_t _cameraEntityId = Config::MAX_SIZE;
        SpriteSortingMethod _spriteSortingMethod = SpriteSortingMethod::None;
        Memory::Memory _memory;
    };
}
