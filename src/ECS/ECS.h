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

class Registry
{
};

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}