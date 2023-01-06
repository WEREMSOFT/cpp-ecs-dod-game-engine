#pragma once

#include <bitset>
#include <vector>

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
	T &Get(int index) { return static_cast(T &)(data[index]); }
	T &operator[](unsigned int index) { return data[index]; }
};

class Registry
{
private:
	int numEntities = 0;

	std::vector<IPool *> componentPool;

public:
	Registry() = default;
	~Registry() = default;

	Entity CreateEntity();
	void KillEntity();
	void AddSystem();
	void RemoveComponent();
};

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}