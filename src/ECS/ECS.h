#pragma once

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <deque>
#include "../Logger/Logger.h"
#include "../Game/ObjectNames.h"

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

enum class TagsIdEnum
{
	NONE,
	ENEMIES,
	PLAYER,
	BULLETS,
	COUNT
};

enum class GroupsIdEnum
{
	OBSTACLES,
	ENEMIES,
	PLAYER,
	PROJECTILES,
	TILES,
	COUNT,
	NONE
};

struct IComponent
{
protected:
	static int nextId;
};

template <typename T>
class Component : public IComponent
{
public:
	static int GetId()
	{
		static auto id = nextId++;
		return id;
	}
};

class Entity
{
private:
	int id;

public:
	ObjectNames name = TILE_ENTITY;
	Entity(int id) : id(id){};
	int GetId() const;
	void Kill();

	template <typename TComponent, typename... TArgs> void AddComponent(TArgs &&...args);
	template <typename TComponent> void RemoveComponent();
	template <typename TComponent> bool HasComponent() const;
	template <typename TComponent> TComponent& GetComponent() const;


	Entity &operator=(const Entity &other) = default;
	bool operator==(const Entity &other) const { return id == other.id; }
	bool operator!=(const Entity &other) const { return id != other.id; }
	bool operator>(const Entity &other) const { return id > other.id; }
	bool operator<(const Entity &other) const { return id < other.id; }

	class Registry* registry;

	// tags
	void Tag(const TagsIdEnum tag);
	bool HasTag(const TagsIdEnum tag);

	// groups
	void Group(const GroupsIdEnum group);
	bool BelongsToGroup(const GroupsIdEnum group);
};

class System
{
private:
	Signature componentSignature;
	std::vector<Entity> entities;

public:
	ObjectNames name = UNKNOWN;

	System() = default;
	~System() = default;

	void AddEntityToSystem(Entity entity);
	void RemoveEntitiyFromSystem(Entity entity);

	std::vector<Entity> GetSystemEntities() const;
	const Signature &GetComponentSignature() const;

	template <typename TCompnent>
	void RequireComponent();
};

class IPool
{
public:
	virtual ~IPool() = default;
	virtual void RemoveEntityFromPool(int entityId) = 0;
};

template <typename T>
class Pool : public IPool
{
private:
	std::vector<T> data;
	int size;

	std::unordered_map<int, int> entityIdToIndex;
	std::unordered_map<int, int> indexToEntityId;

public:
	Pool(int capacity = 100) 
	{ 
		size = 0; 
		data.resize(capacity);
	}

	virtual ~Pool() = default;

	bool IsEmpty() const 
	{ 
		return size == 0; 
	}

	int GetSize() const 
	{ 
		return size; 
	}

	void Clear() { data.clear(); };
	
	void Add(T object) 
	{ 
		data.push_back(object);
		
	}
	
	void Set(int entityId, T object) { 

		if(entityIdToIndex.find(entityId) != entityIdToIndex.end())
		{
			int index = entityIdToIndex[entityId];
			data[index] = object;
		} else {
			int index = size;
			entityIdToIndex.emplace(entityId, index);
			indexToEntityId.emplace(index, entityId);

			if(index >= data.capacity())
			{
				data.resize(size * 2);
			}

			data[index] = object; 
			size++;
		}
	};
	
	void Resize(int n)
	{
		data.resize(n);
	}

	void Remove(int entityId)
	{
		int indexOfRemoved = entityIdToIndex[entityId];
		int indexOfLast = size - 1;

		data[indexOfRemoved] = data[indexOfLast];

		int entityIdOfLastElement = indexToEntityId[indexOfLast];
		entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
		indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

		entityIdToIndex.erase(entityId);
		indexToEntityId.erase(indexOfLast);

		size--;
	}

	T &Get(int entityId) 
	{ 
		int indexToReturn = entityIdToIndex[entityId];
		return static_cast<T &>(data[indexToReturn]); 
	}

	T &operator[](unsigned int index) 
	{ 
		return data[index]; 
	}

	void RemoveEntityFromPool(int entityId) override 
	{
		if(entityIdToIndex.find(entityId) != entityIdToIndex.end())
		{
			Remove(entityId);
		}
	}
};

class Registry
{
private:
	int numEntities = 0;

	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeRemoved;

	std::vector<Signature> entityComponentSignatures;
	std::vector<std::shared_ptr<IPool>> componentPools;

	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
	
	// list of free entities ID's
	std::deque<int> freeIds;

	std::unordered_map<TagsIdEnum, Entity> entityPerTag;
	std::unordered_map<int, TagsIdEnum> tagPerEntity;

	std::unordered_map<GroupsIdEnum, std::set<Entity>> entitiesPerGroup;
	std::unordered_map<int, GroupsIdEnum> groupPerEntity;

public:
	Registry() 
	{
		Logger::Log("creating registry");
	};
	~Registry() 
	{
		Logger::Log("destroying registry");
	}

	void Update();

	Entity CreateEntity();
	void KillEntity(Entity entity);

	void AddEntityToSystems(Entity entity);
	void RemoveEntityFromSystems(Entity entity);

	template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename T> void RemoveComponent(Entity entity);
	template <typename T> bool HasComponent(Entity entity) const;
	template <typename T> T& GetComponent(Entity entity) const;

	template <typename TSystem, typename ...TArgs>
	void AddSystem(TArgs&& ...args);
	template <typename TSystem>
	void RemoveSystem();
	template <typename TSystem>
	bool HasSystem() const;
	template <typename TSystem>
	TSystem& GetSystem() const;

	// Tags
	void TagEntity(Entity entity, const TagsIdEnum tag);
	bool EntityHasTag(Entity entity, const TagsIdEnum tag) const;
	Entity GetEntitiesByTag(const TagsIdEnum tag) const;
	void RemoveEntityTag(Entity entity);

	// Groups
	void GroupEntity(Entity entity, const GroupsIdEnum group);
	bool EntityBelongsToGroup(Entity entity, const GroupsIdEnum group) const;
	std::vector<Entity> GetEntitiesByGroup(const GroupsIdEnum group) const;
	void RemoveEntityGroup(Entity entity);

};

template <typename TSystem>
TSystem& Registry::GetSystem() const
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *std::static_pointer_cast<TSystem>(system->second);
}

template <typename TSystem, typename... TArgs>
void Registry::AddSystem(TArgs &&...args)
{
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem()
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const
{
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}

template <typename TComponent, typename... TArgs>
void Registry::AddComponent(Entity entity, TArgs &&...args)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	if (componentId >= static_cast<int>(componentPools.size()))
	{
		componentPools.resize(componentId + 1, nullptr);
	}

	if (!componentPools[componentId])
	{
		std::shared_ptr<Pool<TComponent>> newComponent = std::make_shared<Pool<TComponent>>();
		componentPools[componentId] = newComponent;
	}

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	TComponent newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);
	entityComponentSignatures[entityId].set(componentId);
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	componentPool->Remove(entityId);
	entityComponentSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	return componentPool->Get(entity.GetId());
}


template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args)
{
	registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent()
{
	registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const
{
	return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const
{
	return registry->GetComponent<TComponent>(this->GetId());
}
