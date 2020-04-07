#include "SpriteAnimation.h"

#include <Events/EventsManager.h>

SpriteAnimation::SpriteAnimation()
	: currentAnimation("") {}

void SpriteAnimation::Initialize() {
	animations = {};
	currentAnimation = "";
}

Component * SpriteAnimation::Clone() const {
	return new SpriteAnimation(*this);
}

void SpriteAnimation::SetActive(bool const& state) {
	Component::SetActive(state);
	Events::EventsManager::GetInstance()->Queue("SPRITE_ANIMATION_ACTIVE", new Events::AnyType<SpriteAnimation*>(this));
}

