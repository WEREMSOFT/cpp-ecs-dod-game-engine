#include "ECS.h"
#include <algorithm>

int IComponent::nextId = 0;

int Entity::GetId() const
{
	return id;
}

void Entity::Kill()
{
	registry->KillEntity(*this);
}

void System::AddEntityToSystem(Entity entity)
{
	entities.push_back(entity);
}

void Registry::AddEntityToSystems(Entity entity)
{
	const auto entityId = entity.GetId();
	const auto& entityComponentSignature = entityComponentSignatures[entityId];

	for (auto &system : systems)
	{
		const auto &systemComponentSignature = system.second->GetComponentSignature();

		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested)
		{
			system.second->AddEntityToSystem(entity);
		}
	}
}

void System::RemoveEntitiyFromSystem(Entity entity)
{
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
        return entity == other;
    }), entities.end());

	// int entityIndex = -1;
	// for (auto &e : entities)
	// {
	// 	entityIndex++;
	// 	if (e.GetId() == entity.GetId())
	// 	{
	// 		break;
	// 	}
	// }

	// if (entityIndex > -1)
	// 	entities.erase(entities.begin() + entityIndex);
}

std::vector<Entity> System::GetSystemEntities() const
{
	return entities;
}
const Signature &System::GetComponentSignature() const
{
	return componentSignature;
}

Entity Registry::CreateEntity()
{
	int entityId;
	
	if(freeIds.empty())
	{
		entityId = numEntities++;
		if (entityId >= static_cast<int>(entityComponentSignatures.size()))
		{
			entityComponentSignatures.resize(entityId + 1);
		}
	} else {
		entityId = freeIds.front();
		freeIds.pop_front();
	}
	
	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);
	
	Logger::Log("Entity created with id = " + std::to_string(entityId));

	return entity;
}

void Registry::RemoveEntityFromSystems(Entity entity)
{
	for(auto system: systems)
	{
		system.second->RemoveEntitiyFromSystem(entity);
	}
}

void Registry::KillEntity(Entity entity)
{
	entitiesToBeRemoved.insert(entity);
}

void Registry::Update()
{
	for(auto& entity: entitiesToBeAdded)
	{
		AddEntityToSystems(entity);
	}

	entitiesToBeAdded.clear();

	for (auto entity: entitiesToBeRemoved)
	{
		RemoveEntityFromSystems(entity);
		entityComponentSignatures[entity.GetId()].reset();
		freeIds.push_back(entity.GetId());
	}

	entitiesToBeRemoved.clear();
}