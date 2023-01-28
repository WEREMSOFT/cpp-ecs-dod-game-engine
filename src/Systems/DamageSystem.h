#pragma once
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Events/CollisionEvent.h"
#include "../EventBus/EventBus.h"
#include "../Logger/Logger.h"

class DamageSystem: public System
{
	public:
		DamageSystem()
		{
			RequireComponent<BoxColliderComponent>();
		}

		void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
		{
			eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
		}

		void onCollision(CollisionEvent &event)
		{
			Logger::Log("Collision Event triggered!!");
			event.a.Kill();
			event.b.Kill();
		}

		void Update()
		{
		}
};