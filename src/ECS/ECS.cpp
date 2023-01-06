#include "ECS.h"

int Entity::GetId() const
{
	return id;
}

void System::AddEntityToSystem(Entity entity)
{
	entities.push_back(entity);
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

template <typename TCompnent>
void RequireComponent();