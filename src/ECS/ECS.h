#pragma once

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent
{
protected:
	static int nextId;
};

template <typename T>
class Component : public IComponent
{
	static int GetId()
	{
		static auto id = nextId++;
		return nextId;
	}
	int id;
};

class Entity
{
private:
	int id;

public:
	Entity(int id) : id(id){};
	int GetId() const;

	Entity &operator=(const Entity &other) = default;
	bool operator==(const Entity &other) const { return id == other.id; }
	bool operator!=(const Entity &other) const { return id != other.id; }
	bool operator>(const Entity &other) const { return id > other.id; }
	bool operator<(const Entity &other) const { return id < other.id; }
};

class System
{
private:
	Signature componentSignature;
	std::vector<Entity> entities;

public:
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
	virtual ~IPool() {}
};

template <typename T>
class Pool : public IPool
{
private:
	std::vector<T> data;

public:
	Pool(int size = 100) { Resize(size); }
	virtual ~Pool() = default;
	bool IsEmpty() const { return data.empty(); }
	int GetSize() const { return data.size(); }
	void Resize(int n) { data.resize(n); }
	void Clear() { data.clear(); };
	void Add(T object) { data.push_back(object); }
	void Set(int index, T object) { data[index] = object; };
	T &Get(int index) { return static_cast<T &>(data[index]); }
	T &operator[](unsigned int index) { return data[index]; }
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

	template <typename T, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&& ...args);

	template <typename T>
	void RemoveComponent(Entity entity);
	template <typename T>
	bool HasComponent(Entity entity) const;
	template <typename T>
	bool GetComponent(Entity entity) const;

	template <typename TSystem, typename  ...TArgs>
	void AddSystem(TArgs&& ...args);
	template <typename TSystem>
	void RemoveSystem();
	template <typename TSystem>
	bool HasSystem() const;
	template <typename TSystem>
	TSystem& GetSystem() const;
};

template <typename TSystem>
TSystem& Registry::GetSystem() const
{
	auto system = system.find(std::type_index(typeid(TSystem)));
	return *std::static_pointer_cast<TSystem>(system->second(system));
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

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	if(componentId >= componentPools.size())
	{
		componentPools.resize(componentId + 1, nullptr);
	}

	if(!componentPools[componentId])
	{
		std::shared_ptr<Pool<TComponent>> newComponent = std::make_shared<Pool<TComponent>>();
		componentPools[componentId] = newComponent;
	}

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	if(entityId >= componentPool->GetSize())
	{
		componentPool->Resize(numEntities);
	}
	
	TComponent newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);
	entityComponentSignatures[entityId].set();
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	entityComponentSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	return entityComponentSignatures[entityId].test(componentId);
}
