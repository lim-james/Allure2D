#ifndef PLAYER_LIFE_H
#define PLAYER_LIFE_H

#include "Script.h"

#include "SpriteRender.h"
#include "Text.h"
#include "Prefab.h"

struct PlayerLife : public Script {

	float maxHealth;
	float inviDuration;
	float flashDuration;

	float* barMultiplier;
	Text* healthText;

	SpriteRender* flashOverlay;
	Prefab* indicatorLabel;

	void OnCollisionEnter(unsigned target);

private:

	float health;
	float inviT; // invincibility time
	float ft; // flash time

	SpriteRender* render;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Hit(vec3f const& position, vec3f const& direction);
	void UpdateBar();

};

#endif