#pragma once
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"

class CollisionSystem: public System
{
	public:
	CollisionSystem()
	{
		RequireComponent<BoxColliderComponent>();
		RequireComponent<TransformComponent>();
	}

	bool CheckAABBCollision(TransformComponent trA, 
							BoxColliderComponent colA, 
							TransformComponent trB, 
							BoxColliderComponent colB)
	{
		if( trA.position.x + colA.offset.x < trB.position.x + colB.offset.x + colB.with &&
			trA.position.x + colA.offset.x + colA.with > trB.position.x + colB.offset.x &&
			trA.position.y + colA.offset.y < trB.position.y + colB.offset.y + colB.height &&
			trA.position.y + colA.offset.y + colA.height > trB.position.y + colB.offset.y)
		{
			return true;					
		}
		return false;
	}

	void Update()
	{
		auto entities = GetSystemEntities();

		for(auto i = entities.begin(); i != entities.end(); i++)
		{
			auto a = *i;
			auto transformA = a.GetComponent<TransformComponent>();
			auto collisionA = a.GetComponent<BoxColliderComponent>();
			for(auto j = i; j != entities.end(); j++)
			{
				auto b = *j;

				if(a == b)
					continue;

				auto transformB = b.GetComponent<TransformComponent>();
				auto collisionB = b.GetComponent<BoxColliderComponent>();

				bool collisionHappened = CheckAABBCollision(transformA, collisionA, transformB, collisionB);

				if(collisionHappened)
				{
					Logger::Log("Entity " + std::to_string(a.GetId()) + " collided with entity " + std::to_string(b.GetId()) );
				}
			}
		}
	}
};