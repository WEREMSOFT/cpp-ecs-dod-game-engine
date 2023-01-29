#pragma once

#include "../ECS/ECS.h"
#include "../Components/KeyboardControllerComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

class KeyboardControllerSystem: public System
{
	public:
		KeyboardControllerSystem()
		{
			RequireComponent<KeyboardControllerComponent>();
			RequireComponent<SpriteComponent>();
			RequireComponent<RigidBodyComponent>();
		}

		void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
		{
			eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControllerSystem::OnKeyPressed);
		}

		void OnKeyPressed(KeyPressedEvent& event)
		{
			for(auto entity: GetSystemEntities())
			{
				const auto keyboardController = entity.GetComponent<KeyboardControllerComponent>();
				auto& sprite = entity.GetComponent<SpriteComponent>();
				auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

				switch(event.code)
				{
					case SDLK_UP:
						rigidBody.velocity = keyboardController.upVelocity;
						sprite.srcRect.y = sprite.height * 0;
						break;
					case SDLK_RIGHT:
						rigidBody.velocity = keyboardController.rightVelocity;
						sprite.srcRect.y = sprite.height * 1;
						break;
					case SDLK_DOWN:
						rigidBody.velocity = keyboardController.downVelocity;
						sprite.srcRect.y = sprite.height * 2;
						break;
					case SDLK_LEFT:
						rigidBody.velocity = keyboardController.leftVelocity;
						sprite.srcRect.y = sprite.height * 3;
						break;

				}
			}
		}

		void Update()
		{

		}
};