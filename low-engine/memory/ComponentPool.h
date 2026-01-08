#pragma once

#include <vector>
#include <unordered_map>

#include "nlohmann/json.hpp"

#include "Config.h"
#include "../log/Log.h"
#include "graphics/Sprite.h"

namespace LowEngine::Memory {
	class Memory;

	/**
	 * @brief Interface abstracting Component Pool
	 */
	class IComponentPool {
	public:
		virtual ~IComponentPool() = default;

		/**
		 * @brief Deep clone for Component Pool
		 * @param newMemory Pointer to Memory manager which will hold new copy of the Component Pool
		 * @return Pointer to new copy of Component Pool
		 */
		virtual std::unique_ptr<IComponentPool> Clone(Memory* newMemory) const = 0;

		/**
		 * @brief Retrieve Component for particular Entity Id.
		 * @param entityId Id of the Entity to which Component belongs.
		 * @return Pointer to Component. Returns nullptr if Entity don't own Component of this type.
		 */
		virtual void* GetComponentPtr(size_t entityId) = 0;

		/**
		 * @brief Call Update method for all Components.
		 * @param deltaTime Time passed since last update, in seconds.
		 */
		virtual void Update(float deltaTime) = 0;

		/**
		 * @brief Check all Components in search of Sprites to draw.
		 *
		 * Sprites will be added to refered collection.
		 * @param[out] sprites Reference to collection that will be filled with Sprites that needs to be drawn.
		 */
		virtual void CollectSprites(std::vector<Sprite>& sprites) = 0;

		bool IsDependantOn(size_t entityId, const std::type_info& typeInfo);

		/**
		 * @brief Destroy Component owned by Entity with provided Id.
		 * @param entityId Id of the Entity that will have its Component destroyed.
		 */
		virtual void DestroyComponent(size_t entityId) = 0;

		virtual nlohmann::ordered_json SerializeToJSON() = 0;
	};


	/**
	 * @brief Class representing a pool of components for managing entity-component storage.
	 *
	 * Component Pool stores components of particular type in sequential order in memory.
	 * It starts with a capacity of Config::DEFAULT_COMPONENT_POOL_SIZE, but will expand exponentially when limit is reached.
	 */
	template <typename T>
	class ComponentPool : public IComponentPool {
	public:
		explicit ComponentPool(size_t capacity = Config::DEFAULT_COMPONENT_POOL_SIZE) {
			Storage.reserve(capacity);
		}

		ComponentPool(const ComponentPool& other, Memory* newMem)
			: ComponentPool(other.Storage.capacity()) {
			for (const auto& [entityId, oldIndex] : other.IndexMap) {
				Storage.emplace_back();

				void* slot = &Storage.back();
				auto* oldComp = reinterpret_cast<const T*>(&other.Storage[oldIndex]);
				oldComp->CloneInto(newMem, slot);

				size_t newIndex = Storage.size() - 1;
				IndexMap[entityId] = newIndex;
				ReverseMap[newIndex] = entityId;
			}
		}

		~ComponentPool() override {
			for (auto& component : Storage) {
				reinterpret_cast<T*>(&component)->~T();
			}
		};

		/**
		 * @brief Deep-copy this Component Pool. Store new copy in the provided Memory manager.
		 * @param newMemory Pointer to Memory manager that will store new copy.
		 * @return Pointer to new copy.
		 */
		std::unique_ptr<IComponentPool> Clone(Memory* newMemory) const override {
			return std::make_unique<ComponentPool<T>>(
				*static_cast<const ComponentPool<T>*>(this),
				newMemory
			);
		}

		/**
		 * @brief Create new Component and attach it to Entity with provided Id.
		 * @tparam Args List of arguments to pass to Component's constructor.
		 * @param memory Pointer to Memory manager that owns this Component Pool.
		 * @param entityId Id of the Entity that will own new Component
		 * @param args List of arguments that should be passed to Component's constructor.
		 * @return Pointer to new Component. Returns nullptr if case of error
		 */
		template <typename... Args>
		T* CreateComponent(Memory* memory, size_t entityId, Args&&... args) {
			if (IndexMap.contains(entityId)) {
				_log->error("Component pool: Component {} already exists for entity id {}.", typeid(T).name(),
				            entityId);
				return nullptr;
			}

			// get index to create component
			size_t index = Storage.size();
			if (index >= Storage.capacity()) {
				Storage.reserve(Storage.capacity() * 2);
				_log->debug("Component pool: Reallocating memory for component type {}. Current size: {}",
				            typeid(T).name(), Storage.capacity());
				// note: this memcopy data to new location, which will cause problems if components are not trivially copyable
				// make sure that component don't have any pointers or references to other objects
			}

			// placement-new to initialize memory
			Storage.emplace_back();
			try {
				T* component = new(&Storage.back()) T(memory, std::forward<Args>(args)...);
				// map entityId to component index
				IndexMap[entityId] = index;
				ReverseMap[index] = entityId;
				return component;
			} catch (...) {
				// since placement new don't allocate, it can't fail.
				// This try...catch seems to be the only way to detect constructor failure?

				_log->error("ERROR!: Creating component of type '{0}' critically failed.", typeid(T).name());

				Storage.pop_back(); // in case of error remove pre-allocated element on the vector
				return nullptr;
			}
		}

		/**
		 * @brief Destroy Component owned by Entity with provided Id.
		 * @param entityId Id of the Entity that will have its Component destroyed.
		 */
		void DestroyComponent(size_t entityId) override {
			auto it = IndexMap.find(entityId);
			if (it == IndexMap.end()) {
				return; // component not found
			}

			size_t removedIndex = it->second;
			reinterpret_cast<T*>(&Storage[removedIndex])->~T();

			size_t lastIndex = Storage.size() - 1;

			// swamp component to remove (index) with the last one
			if (removedIndex != lastIndex) {
				std::swap(Storage[removedIndex], Storage[lastIndex]);

				size_t swappedEntityId = ReverseMap[lastIndex];
				IndexMap[swappedEntityId] = removedIndex;
				ReverseMap[removedIndex] = swappedEntityId;
			}

			Storage.pop_back();
			IndexMap.erase(it);
			ReverseMap.erase(lastIndex);
		}

		/**
		 * @brief Retrieve pointer to a component belonging to Entity with provided Id.
		 * @param entityId Id of the Entity the component belong to.
		 * @return Pointer to component. Returns nullptr when Component not found
		 */
		void* GetComponentPtr(size_t entityId) override {
			auto it = IndexMap.find(entityId);
			if (it == IndexMap.end()) {
				return nullptr;
			}
			return &Storage[it->second];
		}

		/**
		 * @brief Executes provided callback for all components.
		 * @tparam Callback Template for callback.
		 * @param callback Callback to execute.
		 */
		template <typename Callback>
		void ForEachComponent(Callback&& callback) {
			for (auto& storage : Storage) {
				T* component = reinterpret_cast<T*>(&storage);
				callback(*component);
			}
		}

		/**
		 * @brief Call update function for all active components.
		 * @param deltaTime Time passed since last update, in seconds.
		 */
		void Update(float deltaTime) override {
			for (auto& storage : Storage) {
				auto component = reinterpret_cast<T*>(&storage);
				if (component->Active) {
					component->Update(deltaTime);
				}
			}
		}

		/**
		 * @brief Check all Components in search of Sprites to draw.
		 *
		 * Sprites will be added to refered collection.
		 * @param[out] sprites Reference to collection that will be filled with Sprites that needs to be drawn.
		 */
		void CollectSprites(std::vector<Sprite>& sprites) override {
			for (auto& storage : Storage) {
				T* component = reinterpret_cast<T*>(&storage);
				if (component->Active) {
					Sprite* sprite = component->Draw();
					if (sprite != nullptr) {
						sprites.emplace_back(*sprite);
					}
				}
			}
		}

		nlohmann::ordered_json SerializeToJSON() override {
			nlohmann::ordered_json componentJson = nlohmann::ordered_json::array();

			for (auto& storage : Storage) {
				T* component = reinterpret_cast<T*>(&storage);
				componentJson.push_back(component->SerializeToJSON());
			}

			return componentJson;
		}

	protected:
		/**
		 * @brief Collection of storage objects. Each object is a single component.
		 */
		std::vector<std::aligned_storage_t<sizeof(T), alignof(T)>> Storage;

		/**
		 * @brief Map of Entity Id to Component Id.
		 *
		 * Key: Entity Id
		 * Value: Component Id
		 */
		std::unordered_map<size_t, size_t> IndexMap;
		/**
		 * @brief Map of Component Id to Entity Id
		 *
		 * Key: Component Id
		 * Value Entity Id
		 */
		std::unordered_map<size_t, size_t> ReverseMap;
	};
}
