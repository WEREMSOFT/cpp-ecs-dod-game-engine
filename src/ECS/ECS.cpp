#include "ECS.h"

int Entity::GetId() const
{
	return id;
}

void System::AddEntityToSystem(Entity entity)
{
	entities.push_back(entity);
}

void Registry::AddEntityToSystems(Entity entity)
{
	const auto entityId = entity.GetId();
	const auto entityComponentSignature = entityComponentSignatures[entityId];

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
	int entityIndex = -1;
	for (auto &e : entities)
	{
		entityIndex++;
		if (e.GetId() == entity.GetId())
		{
			break;
		}
	}

	if (entityIndex > -1)
		entities.erase(entities.begin() + entityIndex);
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
	int entityId = numEntities++;
	Entity entity(entityId);
	entitiesToBeAdded.insert(entity);

	if (entityId >= static_cast<int>(entityComponentSignatures.size()))
	{
		entityComponentSignatures.resize(entityId + 1);
	}
	
	Logger::Log("Entity created with id = " + std::to_string(entityId));

	return entity;
}



void Registry::KillEntity(Entity entity)
{
	
}

void Registry::Update()
{
	for(auto entity: entitiesToBeAdded)
	{
		AddEntityToSystems(entity);
	}

	entitiesToBeAdded.clear();
}