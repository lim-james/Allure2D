#include "FlyingEnemyPrefab.h"

#include "SpriteRender.h"
#include "SpriteAnimation.h"
#include "SphereCollider.h"
// utilities
#include "LoadTexture.h"
#include "LoadSAD.h"

FlyingEnemyPrefab::FlyingEnemyPrefab() {
	spriteSheet = Load::Texture2D("Files/SpritePack/Monsters/flying enemy/flying-enemy-sheet.png");
	spriteData = Load::SAD("Files/SpritePack/Monsters/flying enemy/flying-enemy-sheet.sad");
}

Transform * FlyingEnemyPrefab::Create() {
	const unsigned entity = entities->Create();

	Transform* const transform = entities->GetComponent<Transform>(entity);

	SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
	render->SetActive(true);
	render->SetSprite(spriteSheet);

	SpriteAnimation* const animation = entities->AddComponent<SpriteAnimation>(entity);
	animation->SetActive(true);
	animation->animations = spriteData;
	animation->currentAnimation = "IDLE";

	SphereCollider* const collider = entities->AddComponent<SphereCollider>(entity);
	collider->SetActive(true);

	return transform;
}
