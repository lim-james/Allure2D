#include "SpriteRender.h"

#include <Events/EventsManager.h>

SpriteRender::SpriteRender()
	: sprite(0)
	, uvRect(vec2f(0.f), vec2f(1.f))
	, tint(1.f)
	, tilemapUnit(1.f)
	, cellRect(vec2f(0.f), vec2f(1.f))
	, material(nullptr) {
}

void SpriteRender::Initialize() {
	SetSprite(0);
	uvRect = vec4f(vec2f(0.f), vec2f(1.f));
	tint = vec4f(1.f);
	tilemapUnit = vec2f(1.f);
	cellRect = vec4f(vec2f(0.f), vec2f(1.f));
	SetMaterial(nullptr);
}

Component * SpriteRender::Clone() const {
	return new SpriteRender(*this);
}

void SpriteRender::SetActive(bool const& state) {
	Component::SetActive(state);
	EventsManager::Get()->Trigger("SPRITE_RENDER_ACTIVE", new Events::AnyType<SpriteRender*>(this));
}

void SpriteRender::SetDynamic(bool const & state) {
	Component::SetDynamic(state);
	EventsManager::Get()->Trigger("SPRITE_RENDER_DYNAMIC", new Events::AnyType<SpriteRender*>(this));
}

unsigned const& SpriteRender::GetSprite() const {
	return sprite;
}

void SpriteRender::SetSprite(unsigned const& _sprite) {
	if (sprite != _sprite) {
		Events::SpriteChange* event = new Events::SpriteChange(sprite, this);
		sprite = _sprite;
		EventsManager::Get()->Trigger("SPRITE_CHANGE", event);
	}
}

void SpriteRender::SetTilemapSize(float const& width, float const& height) {
	tilemapUnit.x = 1.f / width;
	tilemapUnit.y = 1.f / height;

	uvRect = cellRect * vec4f(tilemapUnit, tilemapUnit);
}

void SpriteRender::SetCellRect(float const& x, float const& y, float const& width, float const& height) {
	cellRect = vec4f(x, y, width, height);
	uvRect = cellRect * vec4f(tilemapUnit, tilemapUnit);
}

Material::Base * const SpriteRender::GetMaterial() const {
	return material;
}

void SpriteRender::SetMaterial(Material::Base * _material) {
	Events::MaterialChange* event = new Events::MaterialChange(material, this);
	material = _material;
	EventsManager::Get()->Trigger("SPRITE_MATERIAL", event);
}
