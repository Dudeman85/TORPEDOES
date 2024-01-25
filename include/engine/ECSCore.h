#pragma once

#include <assert.h>
#include <bitset>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stack>

//Max amount of unique entities and components, needed for array sizing
const uint16_t MAX_ENTITIES = 10000;
const uint16_t MAX_COMPONENTS = 100;

//Entity is only a numerical id
using Entity = uint16_t;
//The signature is a bitset with one bit per possible component
using Signature = std::bitset<MAX_COMPONENTS>;

//Class for managing entities
class EntityManager
{
public:
	std::stack<Entity> availableEntities;
	std::set<Entity> usedEntities;
	Signature entitySignatures[MAX_ENTITIES];
	uint16_t entityCount = 0;

	EntityManager()
	{
		//Initialize availeableEntityIds to include every possible id
		for (size_t i = 0; i < MAX_ENTITIES; i++)
		{
			availableEntities.push(i);
		}
	}

	//Return a unique entity id
	Entity newEntity()
	{
		assert((entityCount < MAX_ENTITIES) && "Too Many Entities!");

		entityCount++;

		Entity entity = availableEntities.top();
		availableEntities.pop();

		usedEntities.insert(entity);

		return entity;
	}

	//Set the entity id as available
	void deleteEntity(Entity entity)
	{
		assert(usedEntities.count(entity) > 0);

		usedEntities.erase(entity);
		entitySignatures[entity].reset();
		entityCount--;
		availableEntities.push(entity);
	}

	//Check if this entity exists
	bool entityExists(Entity entity)
	{
		return usedEntities.count(entity);
	}
};

//Generic component array interface for component manager
//Unfortunaly there is seemingly no way to do this without virtual inheritance
class IComponentArray
{
public: virtual void removeComponent(Entity entity) = 0;
};

//Stores all the instances of a component type
template<typename T>
class ComponentArray : public IComponentArray
{
public:
	//Packed array of all components of type T
	T componentArray[MAX_ENTITIES];

	//Map from an entity id to the index of its component in the componentArray
	std::map<Entity, int> entityToIndex;
	//Map from a components index in the componentArray to its entity's id
	std::map<int, Entity> indexToEntity;

	//Amount of components in existance
	int size = 0;

	void addComponent(Entity entity, T component)
	{
		//Update entity and index maps to include new entity
		entityToIndex[entity] = size;
		indexToEntity[size] = entity;

		//Add the component to the packed array
		componentArray[size] = component;

		size++;
	}

	void removeComponent(Entity entity) override
	{
		if (entityToIndex.find(entity) == entityToIndex.end())
		{
			std::cout << "Warning: Trying to remove non-existent component" << std::endl;
			return;
		}

		//Keep track of the deleted components index, and the entity of the last component in the array
		int deletedIndex = entityToIndex[entity];
		Entity lastEntity = indexToEntity[size - 1];

		//Overwrite the deleted component by moving the last component in the component array to its index
		componentArray[deletedIndex] = componentArray[size - 1];

		//Update the maps of the moved component
		entityToIndex[lastEntity] = deletedIndex;
		indexToEntity[deletedIndex] = lastEntity;

		//Remove the deleted entity, and moved component from the maps
		entityToIndex.erase(entity);
		indexToEntity.erase(size - 1);
	}

	T& getComponent(Entity entity)
	{
		assert((entityToIndex.count(entity) > 0) && "Entity does not have desired component!");

		//Return a reference to entity's component
		return componentArray[entityToIndex[entity]];
	}

	bool hasComponent(Entity entity)
	{
		return entityToIndex.count(entity) > 0;
	}
};

//Stores and manages every component array
class ComponentManager
{
public:
	static ComponentManager& getInstance()
	{
		static ComponentManager instance;
		return instance;
	}
	
	uint16_t nextId = 0;

	//A map from a component type's name to the component array of its type
	std::map<const char*, std::shared_ptr<IComponentArray>> componentArrays;

	//A map from a component type's name to its id
	std::map<std::string, uint16_t> typeToId;
	//A map from a component type's id to its name
	std::map<uint16_t, std::string> idToType;

	//TODO: Obsolete, delete
	template<typename T>
	void registerComponent()
	{
		//Name of the new component's type
		const char* componentName = typeid(T).name();

		superRegister(componentName);

		componentArrays.insert({ typeid(T).name(), std::make_shared<ComponentArray<T>>() });

		nextId++;

		std::cout << "Manually registered component: " << componentName << ", id: "
		<< ComponentManager::getInstance().typeToId[componentName] << "\n";
	}

	//Register component type, this being templated is stupid
	template<typename T>
	bool autoRegister()
	{
		const char* componentName = typeid(T).name();

		//Assigns an id and makes a new component array for the registered component type
		superRegister(componentName);
		
		componentArrays.insert({ componentName, std::make_shared<ComponentArray<T>>() });

		nextId++;

		std::cout << "Automatically registered component: " << componentName << ", id: "
		<< ComponentManager::getInstance().typeToId[componentName] << "\n";

		return true;
	}

	void superRegister(const char* componentName)
	{
		//Assigns an id and makes a new component array for the registered component type
		typeToId.insert({ componentName, nextId });
		idToType.insert({ nextId, componentName });
	}

	// TODO: WHY IS THIS TEMPLATE? With 100 components we have 100 functions doing the same code, just do typeid(T).name() when
	// calling this! Delete!

	// Returns the id of a component type 
	template<typename T>
	uint16_t getComponentId()
	{
		assert((typeToId.count(typeid(T).name()) > 0) && "Trying to get ID of non-registered component!");

		return typeToId[typeid(T).name()];
	}

	uint16_t getComponentId(const char* componentName)
	{
		assert((typeToId.count(componentName) > 0) && "Trying to get ID of non-registered component!");

		return typeToId[componentName];
	}

	//Adds a component of type T to entity
	template<typename T>
	void addComponent(Entity entity, T component)
	{
		//Call the addComponent method of the correct component array
		getComponentArray<T>()->addComponent(entity, component);
	}

	//Removes a component of type T from entity
	template<typename T>
	void removeComponent(Entity entity)
	{
		//Call the removeComponent method of the correct component array
		getComponentArray<T>()->removeComponent(entity);
	}

	//Get a reference to a component of type T from entity
	template<typename T>
	T& getComponent(Entity entity)
	{
		//Call the getComponent method of the correct component array
		return getComponentArray<T>()->getComponent(entity);
	}

	//Check if entity has a component
	template<typename T>
	bool hasComponent(Entity entity)
	{
		//Call the getComponent method of the correct component array
		return getComponentArray<T>()->hasComponent(entity);
	}

	void destroyEntity(Entity entity, Signature signature)
	{
		for (size_t i = 0; i < signature.size(); i++)
		{
			if (signature[i] != 0)
			{
				componentArrays[idToType[i].c_str()]->removeComponent(entity);
			}
		}
	}

private:
	//QOL function to get the casted ComponentArray
	template<typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray()
	{
		const char* componentType = typeid(T).name();

		//If the component has not been registered, do it
		if (componentArrays.find(componentType) == componentArrays.end())
		{
			std::cout << "Warning: trying to add unregistered component " << componentType << "! ECS automatically registered it, but it is recommended you manually do it to avoid unexpected behavior!\n";
			registerComponent<T>();
		}

		//Return a Cast ComponentArray of desired type
		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[componentType]);
	}
};

//Base class all systems inherit from
class System
{
public:
	// Unique list of every entity containing the required components for the system
	std::set<Entity> entities;
};

//Manager class to make sure every system has the correct list of entitites
class SystemManager
{
public:
	static SystemManager& getInstance()
	{
		static SystemManager instance;
		return instance;
	}

	//Map of all systems accessible by their type names
	std::map<const char*, std::shared_ptr<System>> systems;
	//Map of each system's signature accessible by their type name
	std::map<const char*, Signature> systemSignatures;

	std::vector<std::function<void()>> setSignatureFunctions;

	template<typename T>
	std::shared_ptr<T> registerSystem()
	{
		//Name of the new system's type
		const char* systemType = typeid(T).name();

		//Create new system and return a pointer to it
		std::shared_ptr<T> system = std::make_shared<T>();
		systems.insert({ systemType, (system) });
		return system;
	}

	//Sets the signature (required components) for the system
	template<typename T>
	void setSignature(Signature signature)
	{
		//Name of the new system's type
		const char* sytemType = typeid(T).name();

		systemSignatures.insert({ sytemType, signature });
	}

	// Call as first thing in main
	void setAllSignatures()
	{
		for (auto setSignatureFunction : setSignatureFunctions)
		{
			setSignatureFunction();
		}
		// Clear functions, we don't need them anymore
		setSignatureFunctions.clear(); 
	}

	// Function template for setting all of a given class' signatures
	template <typename T>
	void setRequiredComponents(std::vector<const char*> requiredComponentNames)
	{
		const char* componentToSetFor = typeid(T).name();
		std::cout << "Automatically set setSignatureFunctions for: " << componentToSetFor << "\n";

		setSignatureFunctions.push_back(
			[componentToSetFor, requiredComponentNames]()
			{
				Signature requiredComponentSignature;
				for (const char* requiredComponentName : requiredComponentNames)
				{
					// Set component signature bitmask to ComponentId values
					requiredComponentSignature.set(ComponentManager::getInstance().getComponentId(requiredComponentName));
				}
		
				SystemManager::getInstance().systemSignatures.insert({ componentToSetFor, requiredComponentSignature });
				std::cout << "Automatically set system signature for: " << componentToSetFor << "\n";
			}
		);
	}

	void destroyEntity(Entity entity)
	{
		//Loop through each system and remove the destoyed entity
		for (auto const& system : systems)
		{
			system.second->entities.erase(entity);
		}
	}

	void onEntitySignatureChanged(Entity entity, Signature entitySignature)
	{
		//Loop through every system
		for (auto const& system : systems)
		{
			//Bitwise and to check if the entity contains all the required components for the system
			if ((entitySignature & systemSignatures[system.first]) == systemSignatures[system.first])
			{
				//Add the entity to the system's set
				system.second->entities.insert(entity);
			}
			else
			{
				//Remove the entity from the system's set
				system.second->entities.erase(entity);
			}
		}
	}
};

//General ECS manager class to interface with the entire ECS framework
class ECS
{
private:
	EntityManager* entityManager;

public:
	ECS()
	{
		//Create instance for every manager
		entityManager = new EntityManager();
	}
	~ECS()
	{
		delete entityManager;
	}

	//Returns a newly created entity with nothing attached
	Entity newEntity()
	{
		return entityManager->newEntity();
	}

	//Destroys an entity and all of its components
	void destroyEntity(Entity entity)
	{
		ComponentManager::getInstance().destroyEntity(entity, entityManager->entitySignatures[entity]);
		entityManager->deleteEntity(entity);
		SystemManager::getInstance().destroyEntity(entity);
	}

	//Check if an entity exists
	bool entityExists(Entity entity)
	{
		return entityManager->entityExists(entity);
	}

	//Register a component
	template<typename T>
	void registerComponent()
	{
		ComponentManager::getInstance().registerComponent<T>();
	}

	//Add a component to an entity and return a reference to it
	template<typename T>
	T& addComponent(Entity entity, T component)
	{
		assert(!hasComponent<T>(entity) && "Don't try to add a component to an entity which already has it!");

		ComponentManager::getInstance().addComponent(entity, component);
		entityManager->entitySignatures[entity].set(ComponentManager::getInstance().getComponentId<T>(), true);
		SystemManager::getInstance().onEntitySignatureChanged(entity, entityManager->entitySignatures[entity]);
		return ComponentManager::getInstance().getComponent<T>(entity);
	}

	//Remove a component from an entity
	template<typename T>
	void removeComponent(Entity entity)
	{
		ComponentManager::getInstance().removeComponent<T>(entity);
		entityManager->entitySignatures[entity].set(ComponentManager::getInstance().getComponentId<T>(), false);
		SystemManager::getInstance().onEntitySignatureChanged(entity, entityManager->entitySignatures[entity]);
	}

	//Returns a pointer to the desired component of the entity
	template<typename T>
	T& getComponent(Entity entity)
	{
		return ComponentManager::getInstance().getComponent<T>(entity);
	}

	//Returns true if entity has desired component
	template<typename T>
	bool hasComponent(Entity entity)
	{
		return ComponentManager::getInstance().hasComponent<T>(entity);
	}

	//Returns the id of the component type
	template<typename T>
	uint16_t getComponentId()
	{
		return ComponentManager::getInstance().getComponentId<T>();
	}

	//Registers a system, returns a pointer to that system
	//Every system needs to be registered before it can be used
	template<typename T>
	std::shared_ptr<T> registerSystem()
	{
		return SystemManager::getInstance().registerSystem<T>();
	}

	//Sets the signature of required components for a system
	template<typename T>
	void setSystemSignature(Signature signature)
	{
		SystemManager::getInstance().setSignature<T>(signature);
	}
};
