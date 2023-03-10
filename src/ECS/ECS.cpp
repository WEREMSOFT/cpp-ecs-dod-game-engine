#include "ECS.h"
#include <algorithm>
#include <assert.h>
#include <vector>

int IComponent::nextId = 0;

int Entity::GetId() const
{
	return id;
}

void Entity::Kill()
{
	registry->KillEntity(*this);
}

void Entity::Tag(const TagsIdEnum tag)
{
	registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const TagsIdEnum tag)
{
	return registry->EntityHasTag(*this, tag);
}

void Entity::Group(const GroupsIdEnum group)
{
	registry->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const GroupsIdEnum group)
{
	return registry->EntityBelongsToGroup(*this, group);
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
	
	return entity;
}

void Registry::RemoveEntityFromSystems(Entity entity)
{
	for(auto system: systems)
	{
		system.second->RemoveEntitiyFromSystem(entity);
	}
}

void Registry::RemoveEntityGroup(Entity entity)
{
	auto groupedEntity = groupPerEntity.find(entity.GetId());
	if(groupedEntity != groupPerEntity.end()) 
	{
		auto group = entitiesPerGroup.find(groupedEntity->second);
		if(group != entitiesPerGroup.end())
		{
			auto entityInGroup = group->second.find(entity);
			if(entityInGroup != group->second.end())
			{
				group->second.erase(entityInGroup);
			}
		}
		groupPerEntity.erase(groupedEntity);
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
		
		// Remove entity from component pool
		for (auto pool: componentPools)
		{
			pool->RemoveEntityFromPool(entity.GetId());
		}

		// make the entity id available to be reused
		freeIds.push_back(entity.GetId());

		// Remove any traces of that entity from the tag/group maps
		RemoveEntityTag(entity);
		RemoveEntityGroup(entity);
	}

	entitiesToBeRemoved.clear();
}

void Registry::TagEntity(Entity entity, const TagsIdEnum tag)
{
	entityPerTag.emplace(tag, entity);
	tagPerEntity.emplace(entity.GetId(), tag);
}

bool Registry::EntityHasTag(Entity entity, const TagsIdEnum tag) const
{
	if(tagPerEntity.find(entity.GetId()) == tagPerEntity.end())
	{
		return false;
	}
	return entityPerTag.find(tag)->second == entity;
}

Entity Registry::GetEntitiesByTag(const TagsIdEnum tag) const
{
	return entityPerTag.at(tag);
}

void Registry::RemoveEntityTag(Entity entity)
{
	auto taggedEntity = tagPerEntity.find(entity.GetId());
	if (taggedEntity != tagPerEntity.end())
	{
		auto tag = taggedEntity->second;
		entityPerTag.erase(tag);
		tagPerEntity.erase(taggedEntity);
	}
}

void Registry::GroupEntity(Entity entity, const GroupsIdEnum group)
{
	entitiesPerGroup.emplace(group, std::set<Entity>());
	entitiesPerGroup[group].emplace(entity);
	groupPerEntity.emplace(entity.GetId(), group);
}

bool Registry::EntityBelongsToGroup(Entity entity, const GroupsIdEnum group) const
{
	auto groupEntities = entitiesPerGroup.at(group);
	return groupEntities.find(entity.GetId()) != groupEntities.end();
}
