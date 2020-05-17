#include "MainGame.h"

#include "Camera.h"
#include "SpriteRender.h"
#include "ParticleSystem.h"
#include "PhysicsSystem.h"
#include "ColliderSystem.h"
#include "AudioSystem.h"
#include "AnimationSystem.h"
#include "LayoutSystem.h"
#include "StateMachine.h"
// post vfx
#include "Bloom.h"
#include "CurveDisplay.h"
// scripts
#include "FPSCounter.h"
#include "CameraFollow.h"
#include "ScreenShake.h"
#include "PlayerMovement.h"
#include "PlayerCombat.h"
#include "CrosshairController.h"
#include "EnemyLife.h"
#include "EnemyTarget.h"
#include "EnemyManager.h"
#include "AudioController.h"
#include "BeatController.h"
// states
#include "ChaseState.h"
// Utils
#include "Layers.h"
#include "LoadTexture.h"
#include "LoadFNT.h"

void MainGame::Awake() {
	Scene::Awake();
	
	systems->Subscribe<AudioSystem>(0);
	systems->Subscribe<PhysicsSystem>(0);
	systems->Subscribe<ParticleSystem>(1);
	systems->Subscribe<AnimationSystem>(1);
	systems->Subscribe<LayoutSystem>(1);
	systems->Subscribe<StateMachine>(2);

	systems->SubscribeFixed<PhysicsSystem>();
	systems->SubscribeFixed<ColliderSystem>();
	systems->SubscribeFixed<StateMachine>();

	StateMachine* const stateMachine = systems->Get<StateMachine>();
	stateMachine->AttachState<States::Chase>("CHASE");
}

void MainGame::Create() {
	Scene::Create();

	Font* const courierNew = Load::FNT("Files/Fonts/CourierNew.fnt", "Files/Fonts/CourierNew.tga");
	Font* const vcrMono = Load::FNT("Files/Fonts/vcr_ocd_mono.fnt", "Files/Fonts/vcr_ocd_mono.png");

	background = new Material::Background;
	background->speed = 75.f;
	background->spread = 10.f;
	background->spreadTint = vec3f(0.5f, 0.0f, 0.25f);
	background->indicatorTint = vec3f(0.5f);
	background->thresholdTint = vec3f(10.f, 0.2f, 0.2f);

	indicatorLabel = new IndicatorLabel;
	indicatorLabel->Initialize(entities);

	basicBullet = new BasicBullet;
	basicBullet->Initialize(entities);

	demoGun = new DemoGun;
	demoGun->bulletPrefab = basicBullet;
	demoGun->Initialize(entities);

	basicEnemy = new BasicEnemy;
	basicEnemy->Initialize(entities);

	background->viewSize = 10.f;

	Camera* const camera = entities->GetComponent<Camera>(mainCamera);
	camera->SetSize(20.f);
	camera->projection = ORTHOGRAPHIC;
	camera->cullingMask = DEFAULT | PLAYER | ENEMY | BULLET;

	CameraFollow* const follow = entities->AddComponent<CameraFollow>(mainCamera);
	follow->SetActive(true);
	follow->offset.z = 10.f;
	follow->stick = false;
	follow->speed = 10.f;
	follow->jutDistance = 4.f;

	{
		ScreenShake* const shake = entities->AddComponent<ScreenShake>(mainCamera);
		shake->SetActive(true);
		shake->duration = 0.25f;
		shake->magnitude = 0.075f;

		AudioListener* const listener = entities->AddComponent<AudioListener>(mainCamera);
		listener->SetActive(true);
	}
	
	// ui camera

	Camera* uiCamera = nullptr;
	{
		const unsigned entity = entities->Create();

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->translation.z = 10.f;

		uiCamera = entities->AddComponent<Camera>(entity);
		uiCamera->SetActive(true);
		uiCamera->SetSize(10.f);
		uiCamera->SetDepth(1);
		uiCamera->shouldClear = false;
		uiCamera->projection = ORTHOGRAPHIC;
		uiCamera->cullingMask = UI;
	}

	// post processing volume
	{
		//const unsigned volume = entities->Create();
		//entities->AddComponent<Bloom>(volume)->SetActive(true);
		//entities->AddComponent<CurveDisplay>(volume)->SetActive(true);
	}

	// FPS counter
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		Layout* const layout = entities->AddComponent<Layout>(entity);
		layout->SetActive(true);
		layout->AddConstraint(Constraint(CENTER_X_ANCHOR, nullptr, CENTER_X_ANCHOR, 1.f, 0.f, uiCamera));
		layout->AddConstraint(Constraint(TOP_ANCHOR, nullptr, TOP_ANCHOR, 1.f, -2.f, uiCamera));

		Text* const text = entities->AddComponent<Text>(entity);
		text->SetActive(true);
		text->SetFont(vcrMono);

		FPSCounter* const fps = entities->AddComponent<FPSCounter>(entity);
		fps->SetActive(true);
	}

	// background
	{
		const unsigned entity = entities->Create();

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->translation.z = -1.f;
		transform->scale = vec3f(80.0f, 45.0f, 1.0f);

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);
		render->SetSprite(Load::TGA("Files/Textures/tile.tga"));
		render->SetMaterial(background);
		render->SetCellRect(0, 0, 16, 9);
		render->tint.a = 0.15f;

		AudioSource* const audio = entities->AddComponent<AudioSource>(entity);
		audio->SetActive(true);
		audio->audioClip = "Files/Media/LOUD - Thoughts.wav";
		audio->loop = true;
		audio->Play();
		audio->volume = 0.f;

		AudioController* const controller = entities->AddComponent<AudioController>(entity);
		controller->SetActive(true);
		controller->material = background;
	}

	// crosshair
	{
		const unsigned entity = entities->Create();

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->scale = 0.5f;
		follow->crosshair = transform;

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);
		render->tint = vec4f(1.f, 0.f, 0.f, 1.f);

		CrosshairController* const controller = entities->AddComponent<CrosshairController>(entity);
		controller->SetActive(true);
		controller->view = camera;
	}

	// Total score label
	Transform* scoreTransform = nullptr;
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		scoreTransform= entities->AddComponent<Transform>(entity);

		Layout* const layout = entities->AddComponent<Layout>(entity);
		layout->SetActive(true);
		layout->AddConstraint(Constraint(LEFT_ANCHOR, nullptr, LEFT_ANCHOR, 1.f, 2.f, uiCamera));
		layout->AddConstraint(Constraint(TOP_ANCHOR, nullptr, TOP_ANCHOR, 1.f, -2.f, uiCamera));

		Text* const text = entities->AddComponent<Text>(entity);
		text->SetActive(true);
		text->SetFont(vcrMono);
		text->scale = 0.75f;
		text->text = "100";
		text->paragraphAlignment = PARAGRAPH_LEFT;
		text->verticalAlignment = ALIGN_TOP;
	}

	// Build score label
	Transform* buildScoreTransform = nullptr;
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		buildScoreTransform = entities->AddComponent<Transform>(entity);

		Layout* const layout = entities->AddComponent<Layout>(entity);
		layout->SetActive(true);
		layout->AddConstraint(Constraint(RIGHT_ANCHOR, nullptr, RIGHT_ANCHOR, 1.f, -2.f, uiCamera));
		layout->AddConstraint(Constraint(TOP_ANCHOR, nullptr, TOP_ANCHOR, 1.f, -2.f, uiCamera));

		Text* const text = entities->AddComponent<Text>(entity);
		text->SetActive(true);
		text->SetFont(vcrMono);
		text->scale = 0.75f;
		text->text = "100";
		text->paragraphAlignment = PARAGRAPH_RIGHT;
		text->verticalAlignment = ALIGN_TOP;
	}

	// multiplier label
	Transform* multiplierTransform = nullptr;
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		multiplierTransform = entities->AddComponent<Transform>(entity);

		Layout* const layout = entities->AddComponent<Layout>(entity);
		layout->SetActive(true);
		layout->AddConstraint(Constraint(RIGHT_ANCHOR, nullptr, RIGHT_ANCHOR, 1.f, -2.f, uiCamera));
		layout->AddConstraint(Constraint(TOP_ANCHOR, buildScoreTransform, BOTTOM_ANCHOR, 1.f, -0.1f, uiCamera));

		Text* const text = entities->AddComponent<Text>(entity);
		text->SetActive(true);
		text->SetFont(vcrMono);
		text->scale = 1.2f;
		text->text = "x8";
		text->paragraphAlignment = PARAGRAPH_RIGHT;
		text->verticalAlignment = ALIGN_TOP;
	}

	// player
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, PLAYER);

		follow->player = entities->GetComponent<Transform>(entity);

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);

		ParticleEmitter* const emitter = entities->AddComponent<ParticleEmitter>(entity);
		emitter->SetActive(true);
		emitter->duration = 0.1f;
		emitter->spawnInterval = 0.1f;
		emitter->lifetime = 0.7f;
		emitter->lifetimeRange = 0.2f;
		emitter->loop = false;
		emitter->speed = 0.f;
		emitter->burstAmount = 15;
		emitter->positionRange.xy = 0.8f;
		emitter->startSize = 0.7f;
		emitter->startSizeRange = 0.3f;
		emitter->endSize = 0.0f;
		emitter->startColor = 0.4f;
		emitter->endColor = 0.4f;

		Physics* const physics = entities->AddComponent<Physics>(entity);
		physics->SetActive(true);
		physics->useGravity = false;
		physics->drag = 10.f;

		SphereCollider* const collider = entities->AddComponent<SphereCollider>(entity);
		collider->SetActive(true);
		collider->ignoreMask = BULLET;

		AudioSource* const audio = entities->AddComponent<AudioSource>(entity);
		audio->SetActive(true);

		PlayerMovement* const movement = entities->AddComponent<PlayerMovement>(entity);
		movement->SetActive(true);
		movement->speed = 400.f;
		movement->dash = 30.f;
		movement->bounds = vec3f(40.f, 22.5f, 1.f);

		PlayerCombat* const combat = entities->AddComponent<PlayerCombat>(entity);
		combat->SetActive(true);
		combat->SetCrosshair(follow->crosshair);
		combat->SetWeapon(demoGun);

		BeatController* const beat = entities->AddComponent<BeatController>(entity);
		beat->SetActive(true);
		beat->SetTempo(100);
		beat->indicatorPrefab = indicatorLabel;
		beat->material = background;
		beat->threshold = 0.15f;

		combat->fire.Bind(&BeatController::Hit, beat);
	}

	// enemy manager
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, ENEMY);

		EnemyManager* const manager = entities->AddComponent<EnemyManager>(entity);
		manager->SetActive(true);
		manager->player = follow->player;
	}

	// test enemy
	{
		Transform* const transform = basicEnemy->Create();
		
		unsigned const& entity = transform->entity;

		EnemyTarget* const target = entities->GetComponent<EnemyTarget>(entity);
		target->speed = 200.f;
		target->player = follow->player;
	}
}

void MainGame::Destroy() {
	Scene::Destroy();

	delete background;

	delete indicatorLabel;
	delete basicBullet;
	delete demoGun;

	delete basicEnemy;
}
