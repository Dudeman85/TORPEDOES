#pragma once
#include <bitset>
#include <stack>
#include <set>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <cstring>
#include <functional>

//Allow max components to be determined outside this file
#ifndef ECS_MAX_COMPONENTS
#define ECS_MAX_COMPONENTS 100
#elif ECS_MAX_COMPONENTS > UINT16_MAX
#error The maximum possible number of components is 65535
#endif

#ifdef _DEBUG
#define ECS_ENABLE_CHECKS
#endif

//Macro to register a component outside main
#define ECS_REGISTER_COMPONENT(COMPONENT) \
struct COMPONENT; \
inline bool COMPONENT##Registered = ( engine::ecs::RegisterComponent<COMPONENT>(), true );

//Macro to register a system and its components outside main
#define ECS_REGISTER_SYSTEM(SYSTEM, ...) \
class SYSTEM; \
inline bool SYSTEM##Registered = ( engine::ecs::RegisterSystem<SYSTEM, __VA_ARGS__>(), true );


namespace engine::ecs
{
	//Define colors for errors and warnings
	constexpr const char* errorFormat = "\033[31m";
	constexpr const char* warningFormat = "\033[33m";
	constexpr const char* normalFormat = "\033[37m";

	//Entities as IDs, 0 will never be a valid ID
	using Entity = uint32_t;
	//Signatures as bitsets, where each component has its own bit
	using Signature = std::bitset<ECS_MAX_COMPONENTS>;

	//FORWARD DECLARES
	template<typename T>
	inline uint16_t GetComponentID();
	inline std::vector<std::string> GetTags(Entity);
	bool EntityExists(Entity);
	inline void LogWarning(std::string message)
	{
		std::cout << warningFormat << message << normalFormat << std::endl;
	}
	inline void LogError(std::string message)
	{
		std::cout << errorFormat << message << normalFormat << std::endl;
	}

	//ENTITY MANAGEMENT DATA

	//Custom container class for storing a system's entities, not suitable for anything else
	class EntityList
	{
	private:
		//Array of entities, order is not guaranteed to remain the same
		Entity* entities;
		uint32_t size;
		uint32_t maxSize;
		//Does the entities array contain any invalid entities
		bool packed = true;

		//Resize the entities array
		void Resize(uint32_t newSize)
		{
			Entity* newArray = new Entity[newSize];

			std::memcpy(newArray, entities, size * sizeof(Entity));

			delete[] entities;

			maxSize = newSize;
			entities = newArray;
		}

	public:
		class Iterator
		{
		private:
			Entity* currentPtr;
			EntityList* parentList;

		public:
			Iterator(Entity* current, EntityList* list)
			{
				currentPtr = current;
				parentList = list;
			}

			//Iterator constructors
			static Iterator Begin(EntityList* list)
			{
				Entity* current = &list->entities[0];

				//Make sure the iterator starts from the first valid entity
				while (*current == 0)
				{
					//Check if the pointer is out of bounds, meaning end of list
					if (current == &list->entities[list->size])
					{
						break;
					}
					current++;
				}

				return Iterator(current, list);
			}
			static Iterator End(EntityList* list)
			{
				return Iterator(&list->entities[list->size], list);
			}

			//Prefix, handles all logic
			Iterator& operator++()
			{
				//If the entity is invalid, skip it
				do
				{
					currentPtr++;
					//Check if the pointer is out of bounds, meaning end of list
					if (currentPtr == &parentList->entities[parentList->size])
					{
						break;
					}
				} while (*currentPtr == 0);

				return *this;
			}
			//Postfix
			Iterator operator++(int)
			{
				Iterator ret = *this;
				++(*this);
				return ret;
			}
			bool operator==(const Iterator& rhs)
			{
				return currentPtr == rhs.currentPtr;
			}
			bool operator!=(const Iterator& rhs)
			{
				return currentPtr != rhs.currentPtr;
			}
			Entity& operator*() const
			{
				if (*currentPtr == 0)
					LogError("ECS ERROR in Iterator: fuck");
				return *currentPtr;
			}
		};

		Iterator begin()
		{
			return Iterator::Begin(this);
		}
		Iterator end()
		{
			return Iterator::End(this);
		}

		EntityList()
		{
			size = 0;
			maxSize = 10;
			entities = new Entity[maxSize];
		}
		~EntityList()
		{
			delete[] entities;
		}

		//Getters
		int Size()
		{
			return size;
		}

		//Debug function, will print all entities
		void PrintEntities()
		{
			std::cout << size << ": [";
			for (uint32_t i = 0; i < size; i++)
			{
				std::cout << entities[i] << ", ";
			}
			std::cout << "]" << std::endl;
		}

		//Add an entity to the end of the list, if it does not exist in it
		void Insert(Entity e)
		{
			//Add 100 or double capacity to the list, whichever is less
			if (size >= maxSize)
				Resize(maxSize + std::min(maxSize, (uint32_t)100));

			//Look through the array and make sure the entity is not in it
			for (uint32_t i = 0; i < size; i++)
			{
				if (entities[i] == e)
				{
					return;
				}
			}

			//Add the new entity
			entities[size] = e;
			size++;
		}

		//Remove an entity from the list
		//Size is not updated, so pack() should be called shortly after
		void Erase(Entity e)
		{
			//Look through the array and set the entity to 0
			for (uint32_t i = 0; i < size; i++)
			{
				if (entities[i] == e)
				{
					entities[i] = 0;
					packed = false;
					return;
				}
			}
		}

		//Packs the array tightly, removing holes
		//This is currently called in Update
		void Pack()
		{
			//If the array is already packed nothing needs to be done
			if (packed)
				return;

			//Look through the array
			uint32_t iterations = size;
			for (uint32_t i = 0; i < iterations; i++)
			{
				//Check for invalid entity
				if (entities[i] == 0)
				{
					if (size == 0)
						break;

					//Find the next valid entity to fill the gap, from back to front
					uint32_t replacementIndex = size - 1;
					while (entities[replacementIndex] == 0)
					{
						size--;
						if (replacementIndex == 0)
							break;
						replacementIndex--;
					}
					//The list ends at i
					if (replacementIndex <= i)
						break;

					//Move the replacement entity to the hole
					entities[i] = entities[replacementIndex];
					entities[replacementIndex] = 0;
					size--;
				}
			}

			packed = true;
			//Resize to keep the free space between 50 and 150
			if (maxSize - size > 150)
				Resize(size + 50);
		}
	};

	//All currently available and used Entity IDs
	std::stack<Entity> availableEntities;
	std::set<Entity> usedEntities;
	//A map of an Entity's ID to its signature
	std::unordered_map<Entity, Signature> entitySignatures;
	std::unordered_map<Entity, std::vector<std::string>> entityTags;
	//How many entities are currently reserved
	uint32_t entityCount = 0;

	//COMPONENT MANAGEMENT DATA

	//Interface for each component array type
	class IComponentArray
	{
	public:
		virtual void RemoveComponent(Entity entity) = 0;
	};
	//A map of every components name to it's corresponding component array
	std::unordered_map<const char*, IComponentArray*> componentArrays;
	//Maps from a components type name to its ID
	std::unordered_map<const char*, uint16_t> componentTypeToID;
	std::unordered_map<uint16_t, const char*> componentIDToType;
	//The amount of components registered. Also the next available component ID
	uint16_t componentCount = 0;
	constexpr uint16_t componentArraySegmentSize = 100;

	//SYSTEM MANAGEMENT DATA

	//Base class all systems inherit from
	class System
	{
	public:
		//Set of every entity containing the required components for the system
		EntityList entities;
	};
	//Map of each system accessible by its type name
	std::unordered_map<const char*, std::shared_ptr<System>> systems;
	//Map of each system's signature accessible by their type name
	std::unordered_map<const char*, Signature> systemSignatures;


	//INTERNAL FUNCTIONS

	//Implementation internal class to interface with each type of component array
	template<typename T>
	class ComponentArray : public IComponentArray
	{
	private:
		//A vector of each component of type T
		std::vector<T> components;
		//Maps from Entities to their component indexed in component arrays
		std::unordered_map<Entity, uint32_t> entityToIndex;
		std::unordered_map<uint32_t, Entity> indexToEntity;
		//Callback funtion to be used as a component destructor
		std::function<void(Entity, T)> componentDestructor;

	public:
		void SetDestructor(std::function<void(Entity, T)> destructor)
		{
			componentDestructor = destructor;
		}

		//Return true if the entity has a component of type T
		bool HasComponent(Entity entity)
		{
			return entityToIndex.count(entity);
		}

		//Get a component from an entity
		T& GetComponent(Entity entity)
		{
			return components[entityToIndex[entity]];
		}

		//Add a component to an entity, returns a reference to that component
		void AddComponent(Entity entity, T component)
		{
			entityToIndex[entity] = components.size();
			indexToEntity[components.size()] = entity;
			components.push_back(component);
		}

		//Removes a component from an entity
		void RemoveComponent(Entity entity) override
		{
			//Call the component destructor
			if (componentDestructor)
				componentDestructor(entity, components[entityToIndex[entity]]);

			//Keep track of the deleted component's index, and the entity of the last component in the array
			uint32_t deletedIndex = entityToIndex[entity];
			Entity lastEntity = indexToEntity[components.size() - 1];

			//Move the last element to the deleted index
			components[entityToIndex[entity]] = components.back();

			//Update the maps for the moved component
			entityToIndex[lastEntity] = deletedIndex;
			indexToEntity[deletedIndex] = lastEntity;

			//Remove the deleted component from the maps
			entityToIndex.erase(entity);
			indexToEntity.erase(components.size() - 1);
			components.pop_back();
		}
	};

	//Implementation internal function. Called whenever an entity's signature changes
	void _OnEntitySignatureChanged(Entity entity)
	{
		const Signature& signature = entitySignatures[entity];

		//Loop through every system
		for (auto const& system : systems)
		{
			//If the entity's signature matches the system's signature
			if ((signature & systemSignatures[system.first]) == systemSignatures[system.first])
			{
				//Add the entity to the system's set
				system.second->entities.Insert(entity);
			}
			else
			{
				//Remove the entity from the system's set
				system.second->entities.Erase(entity);
			}
		}
	}

	//Implementation internal function. Make a signature from a series of components.
	//This is getting really complicated...
	template<typename Comp, typename... Comps>
	Signature _MakeSignature()
	{
		//Recursively add each component type to the signature
		Signature signature;
		if constexpr (sizeof...(Comps) > 0)
			signature = _MakeSignature<Comps...>();
		signature.set(GetComponentID<Comp>());
		return signature;
	}

	//Implementation internal function. Get a component array of type T
	template<typename T>
	inline ComponentArray<T>* _GetComponentArray()
	{
		return static_cast<ComponentArray<T>*>(componentArrays[typeid(T).name()]);
	}

	//DEBUG FUNCTIONS

	//Print all entities to log
	void LogEntities()
	{
		std::cout << "Entities: ";
		for (Entity entity : usedEntities)
		{
			std::cout << entity << ", ";
		}
	}
	//Log the signature and component list of an entity as a string
	void LogEntityInfo(Entity entity)
	{
		//Print id
		if (EntityExists(entity))
			std::cout << "ID: " << entity;
		else
			std::cout << "Entity " << entity << " does not exist" << std::endl;

		//Print tags
		std::cout << ", Tags: ";
		for (const std::string& tag : GetTags(entity))
		{
			std::cout << tag << ", ";
		}
		if (GetTags(entity).size() == 0)
			std::cout << "none";

		//Print components
		std::cout << ", Signature: " << entitySignatures[entity] << ", Components: ";
		for (uint16_t i = 0; i < entitySignatures[entity].size(); i++)
		{
			if (entitySignatures[entity][i])
				std::cout << componentIDToType[i] << ", ";
		}
		if (entitySignatures[entity].none())
			std::cout << "none";
		std::cout << std::endl;
	}

	//PUBLIC FUNCTIONS

	//Updates entity arrays and maybe some other things, call this at the very end of a frame
	void Update()
	{
		for (auto& system : systems)
		{
			system.second->entities.Pack();
		}
	}

	//Checks if an entity exists
	inline bool EntityExists(Entity entity)
	{
		return usedEntities.find(entity) != usedEntities.end();
	}

	//Set a list of tags to an entity
	inline void SetTags(Entity entity, std::vector<std::string> tags)
	{
#ifdef ECS_ENABLE_CHECKS
		//Make sure the entity exists
		if (!EntityExists(entity))
		{
			LogWarning("ECS WARNING in SetTags(): The entity does not exist!");
			return;
		}
#endif

		entityTags[entity] = tags;
	}

	//Add a tag to an entity
	inline void AddTag(Entity entity, std::string tag)
	{
#ifdef ECS_ENABLE_CHECKS
		//Make sure the entity exists
		if (!EntityExists(entity))
		{
			LogWarning("ECS WARNING in AddTag(): The entity does not exist!");
			return;
		}
#endif

		if (entityTags.count(entity) != 0)
			entityTags[entity].push_back(tag);
		else
			SetTags(entity, { tag });
	}

	//Remove a tag from an entity
	inline void RemoveTag(Entity entity, std::string tag)
	{
#ifdef ECS_ENABLE_CHECKS
		//Make sure the entity exists
		if (!EntityExists(entity))
		{
			LogWarning("ECS WARNING in RemoveTag(): The entity does not exist!");
			return;
		}
#endif

		//Remove every instance of the tag
		for (size_t i = 0; i < entityTags[entity].size(); i++)
		{
			if (entityTags[entity][i] == tag)
			{
				entityTags[entity].erase(entityTags[entity].begin() + i);
			}
		}
	}

	//Removes aevery tag from an entity
	inline void RemoveAllTags(Entity entity)
	{
#ifdef ECS_ENABLE_CHECKS
		//Make sure the entity exists
		if (!EntityExists(entity))
		{
			LogWarning("ECS WARNING in RemoveAllTags(): The entity does not exist!");
			return;
		}
#endif

		entityTags[entity].clear();
	}

	//Get the list of tags for entity
	inline std::vector<std::string> GetTags(Entity entity)
	{
#ifdef ECS_ENABLE_CHECKS
		//Make sure the entity exists
		if (!EntityExists(entity))
		{
			LogWarning("ECS WARNING in GetTags(): The entity does not exist!");
			return {};
		}
#endif

		return entityTags[entity];
	}

	//Returns true if entity has the specified tag
	inline bool HasTag(Entity entity, std::string tag)
	{
#ifdef ECS_ENABLE_CHECKS
		//Make sure the entity exists
		if (!EntityExists(entity))
		{
			LogWarning("ECS WARNING in HasTag(): The entity does not exist!");
			return false;
		}
#endif

		return std::find(entityTags[entity].begin(), entityTags[entity].end(), tag) != entityTags[entity].end();
	}

	//Register a new component of type T
	template<typename T>
	void RegisterComponent()
	{
		const char* componentType = typeid(T).name();

#ifdef ECS_ENABLE_CHECKS
		//Make sure the component has not been previously registered
		if (componentArrays.count(componentType) != 0)
		{
			LogWarning("ECS WARNING in RegisterComponent(): The component you are trying to register has alredy been registered!");
			return;
		}
		//Make sure there are not too many components registered
		if (componentCount >= ECS_MAX_COMPONENTS)
		{
			LogError("ECS ERROR in RegisterComponent(): Too many registered components! The default limit is 100. This can be increased with \"#define ECS_MAX_COMPONENTS num\" before you include ECS.h!");
			throw std::runtime_error("ECS ERROR: Too many registered components!");
		}
#endif

		//Assigns an ID and makes a new component array for the registered component type
		componentTypeToID[componentType] = componentCount;
		componentIDToType[componentCount] = componentType;
		componentArrays[componentType] = new ComponentArray<T>();

		componentCount++;
	}

	//Add a destructor function to be called when a component is deleted
	template<typename T>
	inline void SetComponentDestructor(std::function<void(Entity, T)> destructor)
	{
#ifdef ECS_ENABLE_CHECKS
		//Make sure the component has been registered
		if (componentArrays.count(typeid(T).name()) == 0)
		{
			LogWarning("ECS WARNING in SetComponentDestructor(): The component you are trying to add a destructor to has not been registered!");
			return;
		}
#endif

		_GetComponentArray<T>()->SetDestructor(destructor);
	}

	//Check if the entity has a component
	template<typename T>
	inline bool HasComponent(Entity entity)
	{
		//Call HasComponent of the relevant component array
		return _GetComponentArray<T>()->HasComponent(entity);
	}

	//Get a reference to entity's component of type T
	template<typename T>
	inline T& GetComponent(Entity entity)
	{
		const char* componentType = typeid(T).name();

#ifdef ECS_ENABLE_CHECKS
		//Make sure the entity exists
		if (!EntityExists(entity))
		{
			LogError("ECS ERROR in GetComponent(): The Entity does not exist!");
			throw std::runtime_error("ECS ERROR: Entity does not exist!");
		}
		//Make sure the entity has the component
		if (!HasComponent<T>(entity))
		{
			LogError("ECS ERROR in GetComponent(): Entity does not have the desired component!");
			throw std::runtime_error("ECS ERROR: Entity does not have the desired component!");
		}
#endif

		return _GetComponentArray<T>()->GetComponent(entity);
	}

	//Get the ID of a component
	template<typename T>
	inline uint16_t GetComponentID()
	{
		const char* componentType = typeid(T).name();

#ifdef ECS_ENABLE_CHECKS
		//Make sure the component has been registered
		if (componentArrays.count(componentType) == 0)
		{
			LogError("ECS ERROR in GetComponentID(): The component has not been registered!");
			throw std::runtime_error("ECS ERROR: Component not registered!");
		}
#endif

		return componentTypeToID[componentType];
	}

	//Add a component to entity.
	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		const char* componentType = typeid(T).name();

#ifdef ECS_ENABLE_CHECKS
		//Make sure the entity exists
		if (!EntityExists(entity))
		{
			LogError("ECS ERROR in AddComponent(): The entity you are trying to add the component to does not exist!");
			throw std::runtime_error("ECS ERROR: Entity does not exist!");
		}
		//Make sure the entity does not already have the component
		if (HasComponent<T>(entity))
		{
			LogWarning("ECS WARNING in AddComponent(): Entity already has the component you are trying to add!");
			return;
		}
#endif

		_GetComponentArray<T>()->AddComponent(entity, component);

		//Update the entity signature
		entitySignatures[entity].set(GetComponentID<T>());
		_OnEntitySignatureChanged(entity);
	}

	//Remove a component of type T from entity
	template<typename T>
	inline void RemoveComponent(Entity entity)
	{
		const char* componentType = typeid(T).name();

#ifdef ECS_ENABLE_CHECKS
		//Make sure the entity exists
		if (!EntityExists(entity))
		{
			LogWarning("ECS WARNING in RemoveComponent(): The entity you are trying to remove the component from does not exist!");
			return;
		}
		//Make sure the entity has the component
		if (!HasComponent<T>(entity))
		{
			LogWarning("ECS WARNING in RemoveComponent(): Entity does not have the component you are trying to remove!");
			return;
		}
#endif

		_GetComponentArray<T>()->RemoveComponent(entity);

		//Update the entity's signature
		entitySignatures[entity].reset(componentTypeToID[componentType]);
		_OnEntitySignatureChanged(entity);
	}

	//Returns a new entity with no components
	Entity NewEntity()
	{
#ifdef ECS_ENABLE_CHECKS
		//Make sure there are not too many entities
		if (entityCount > UINT32_MAX)
		{
			LogError("ECS ERROR in NewEntity(): Too many Entities!");
			throw std::runtime_error("ECS ERROR: Too many Entities!");
		}
#endif

		entityCount++;

		//Make more entity IDs available in batches of 100
		if (availableEntities.size() == 0)
		{
			for (uint32_t i = entityCount + 99; i >= entityCount; i--)
			{
				availableEntities.push(i);
			}
		}

		//Get an available ID and mark it as used
		Entity entity = availableEntities.top();
		availableEntities.pop();
		usedEntities.insert(entity);
		entitySignatures[entity] = Signature();

		return entity;
	}

	//Delete an entity and all of its components
	void DestroyEntity(Entity entity)
	{
#ifdef ECS_ENABLE_CHECKS
		//Make sure the entity exists
		if (!EntityExists(entity))
		{
			LogWarning("ECS WARNING in DestroyEntity(): The Entity you are trying to destroy does not exist!");
			return;
		}
#endif

		//Delete all components
		for (uint16_t i = 0; i < componentCount; i++)
		{
			if (entitySignatures[entity][i])
			{
				componentArrays[componentIDToType[i]]->RemoveComponent(entity);
			}
		}
		//Set the entitys signature to none temporarily
		entitySignatures[entity].reset();

		_OnEntitySignatureChanged(entity);
		RemoveAllTags(entity);

		//Set the entity as available and update relevant trackers
		entitySignatures.erase(entity);
		usedEntities.erase(entity);
		availableEntities.push(entity);
		entityCount--;
	}

	//Destroys all entities without the "persistent" tag along with all their components.
	//If ignorePersistent is set to true, will also delete "persistent" entities.
	void DestroyAllEntities(bool ignorePersistent = false)
	{
		//Make a copy of the entities set to prevent iterator breaking
		std::set<Entity> usedEntitiesCopy = usedEntities;

		for (auto itr = usedEntitiesCopy.begin(); itr != usedEntitiesCopy.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;

			//Check validity of entity, it can get deleted by component destructors
			if (!EntityExists(entity))
				continue;

			//Only delete "persistent" entities when forced
			if (!HasTag(entity, "persistent") || ignorePersistent)
			{
				DestroyEntity(entity);
			}
		}
	}

	//Returns a reference to the desired system
	template<typename T>
	inline std::shared_ptr<T> GetSystem()
	{
		const char* systemType = typeid(T).name();

#ifdef ECS_ENABLE_CHECKS
		//Make sure the system has been registered
		if (systems.count(systemType) == 0)
		{
			LogError("ECS ERROR in GetSystem(): The system has not been registered!");
			throw std::runtime_error("ECS ERROR: System not registered!");
		}
#endif

		return std::static_pointer_cast<T>(systems[systemType]);
	}

	//Register a system to require the specified components
	template<typename Sys, typename... Comps>
	std::shared_ptr<Sys> RegisterSystem()
	{
		const char* systemType = typeid(Sys).name();

#ifdef ECS_ENABLE_CHECKS
		//Make sure the system has not been registered
		if (systems.count(systemType) != 0)
		{
			LogWarning("ECS WARNING in RegisterSystem(): The system has already been registered!");
			return GetSystem<Sys>();
		}
#endif

		//Make the signature and system
		systemSignatures[systemType] = _MakeSignature<Comps...>();
		std::shared_ptr<Sys> system = std::make_shared<Sys>();
		systems[systemType] = system;
		return system;
	}
}