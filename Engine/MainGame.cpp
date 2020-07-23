#include "MainGame.h"

#include "RenderSystem.h"
#include "Camera.h"
#include "SpriteRender.h"
#include "SpriteAnimationSystem.h"
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
#include "Vignette.h"
// scripts
#include "GameManager.h"
#include "FPSCounter.h"
#include "CameraFollow.h"
#include "ScreenShake.h"
#include "PlayerMovement.h"
#include "PlayerCombat.h"
#include "PlayerLife.h"
#include "BeatController.h"
#include "CrosshairController.h"
#include "EnemyLife.h"
#include "EnemyTarget.h"
#include "EnemyManager.h"
#include "ScoreController.h"
// states
#include "ChaseState.h"
#include "DeadState.h"
// Utils
#include "ProjectDefines.h"
#include "Layers.h"
#include "LoadTexture.h"
#include "LoadFNT.h"
#include "LoadSAD.h"

void MainGame::Awake() {
	Scene::Awake();
	
	systems->Subscribe<AudioSystem>(0);
	systems->Subscribe<PhysicsSystem>(0);
	systems->Subscribe<ParticleSystem>(1);
	systems->Subscribe<AnimationSystem>(1);
	systems->Subscribe<LayoutSystem>(1);
	systems->Subscribe<StateMachine>(2);
	systems->Subscribe<SpriteAnimationSystem>(2);

	systems->SubscribeFixed<PhysicsSystem>();
	systems->SubscribeFixed<ColliderSystem>();
	systems->SubscribeFixed<StateMachine>();

	StateMachine* const stateMachine = systems->Get<StateMachine>();
	stateMachine->AttachState<States::Chase>("CHASE");
	stateMachine->AttachState<States::Dead>("DEAD");

	background = new Material::Background;
	background->speed = 75.f;
	background->spread = 10.f;
	background->viewSize = 10.f;
	//background->spreadTint = vec3f(0.5f, 0.0f, 0.25f);
	background->spreadTint = COLOR_PURPLE;
	background->indicatorTint = vec3f(0.5f);
	background->thresholdTint = vec3f(10.f, 0.2f, 0.2f);

	transparentSprite = new Material::SpriteDefault;
	transparentSprite->flags += TRANSPARENT;

	transparentSprite = new Material::SpriteDefault;
	transparentSprite->flags += TRANSPARENT;

	circle = new Material::Circle;
	circle->borderWeight = 0.25f;

	indicatorLabel = new IndicatorLabel;
	indicatorLabel->Initialize(entities);

	sfxEmitter = new SFXEmitter;
	sfxEmitter->Initialize(entities);

	explosionArea = new ExplosionArea;
	explosionArea->Initialize(entities);

	basicBullet = new BasicBullet;
	basicBullet->Initialize(entities);

	longBullet = new LongBullet;
	longBullet->Initialize(entities);

	explosiveBullet = new ExplosiveBullet;
	explosiveBullet->Initialize(entities);
	explosiveBullet->explosionPrefab = explosionArea;

	// guns

	laserPath = new LaserPath;
	laserPath->Initialize(entities);

	pistol = new Pistol;
	pistol->Initialize(entities);
	pistol->bulletPrefab = basicBullet;

	automatic = new Automatic;
	automatic->Initialize(entities);
	automatic->bulletPrefab = longBullet;

	shotgun = new Shotgun;
	shotgun->Initialize(entities);
	shotgun->bulletPrefab = longBullet;

	sniper = new Sniper;
	sniper->Initialize(entities);
	sniper->bulletPrefab = longBullet;

	grenadeLauncher = new GrenadeLauncher;
	grenadeLauncher->Initialize(entities);
	grenadeLauncher->bulletPrefab = explosiveBullet;

	laser = new Laser;
	laser->Initialize(entities);
	laser->bulletPrefab = laserPath;
	
	// enemies

	basicEnemy = new BasicEnemy;
	basicEnemy->Initialize(entities);

	batEnemy = new BatEnemy;
	batEnemy->Initialize(entities);

	fireElementalEnemy = new FireElementalEnemy;
	fireElementalEnemy->Initialize(entities);

	iceElementalEnemy = new IceElementalEnemy;
	iceElementalEnemy->Initialize(entities);

	cyclopsEnemy = new CyclopsEnemy;
	cyclopsEnemy->Initialize(entities);
}

void MainGame::Create() {
	Scene::Create();

	// color scheme
	//const vec4f BG		= Helpers::GetColor("1E1E3E");
	//const vec4f blue	= Helpers::GetColor("285BB5");
	//const vec4f red		= Helpers::GetColor("C53533");
	//const vec4f green	= Helpers::GetColor("73D540");
	//const vec4f yellow	= Helpers::GetColor("FAE74D");
	//const vec4f purple	= Helpers::GetColor("644AF5");
	//const vec4f pink	= Helpers::GetColor("E84471");
	//const vec4f cyan	= Helpers::GetColor("5EC3C6");
	//const vec4f grey	= Helpers::GetColor("C7C7C7");

	Font* const courierNew = Load::FNT("Files/Fonts/CourierNew.fnt", "Files/Fonts/CourierNew.tga");
	Font* const vcrMono = Load::FNT("Files/Fonts/vcr_ocd_mono.fnt", "Files/Fonts/vcr_ocd_mono.png");

	const unsigned playerSheet = Load::Texture2D("Files/Sprites/NTlikeTDSSprites.png");
	const unsigned ukGunSheet = Load::Texture2D("Files/Sprites/UK.png");

	Camera* const camera = entities->GetComponent<Camera>(mainCamera);
	camera->SetSize(30.f);
	camera->projection = ORTHOGRAPHIC;
	camera->cullingMask = DEFAULT | PLAYER | ENEMY | WEAPON | EFFECT_AREA | BULLET | BONUS_BULLET | ENEMY_BULLET;
	camera->clearColor = COLOR_BACKGROUND;

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
		shake->magnitude = 0.25f;

		AudioListener* const listener = entities->AddComponent<AudioListener>(mainCamera);
		listener->SetActive(true);

		TextureData tData;
		tData.level = 0;
		tData.internalFormat = GL_RGB16F;
		tData.border = 0;
		tData.format = GL_RGBA;
		tData.type = GL_UNSIGNED_BYTE;
		tData.attachment = GL_COLOR_ATTACHMENT0;
		tData.parameters.push_back({ GL_TEXTURE_MIN_FILTER, GL_LINEAR });
		tData.parameters.push_back({ GL_TEXTURE_MAG_FILTER, GL_LINEAR });
		tData.parameters.push_back({ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE });
		tData.parameters.push_back({ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE });

		RenderBufferData rbData;
		rbData.internalFormat = GL_DEPTH24_STENCIL8;
		rbData.attachmentFormat = GL_DEPTH_STENCIL_ATTACHMENT;

		Framebuffer* const fb = new Framebuffer(1, 1);
		fb->Initialize(vec2u(1600, 900), { tData }, { rbData });

		camera->SetFramebuffer(fb);
	}
	
	// ui camera
	Camera* uiCamera = nullptr;
	{
		const unsigned entity = entities->Create();

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->SetLocalTranslation(vec3f(0.f, 0.f, 10.f));

		uiCamera = entities->AddComponent<Camera>(entity);
		uiCamera->SetActive(true);
		uiCamera->SetSize(15.f);
		uiCamera->SetDepth(1);
		uiCamera->shouldClear = false;
		uiCamera->projection = ORTHOGRAPHIC;
		uiCamera->cullingMask = UI;
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

	// prompt label
	Text* promptLabel = nullptr;
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		Layout* const layout = entities->AddComponent<Layout>(entity);
		layout->SetActive(true);
		layout->AddConstraint(Constraint(CENTER_X_ANCHOR, nullptr, CENTER_X_ANCHOR, 1.f, 0.f, uiCamera));
		layout->AddConstraint(Constraint(BOTTOM_ANCHOR, nullptr, BOTTOM_ANCHOR, 1.f, 2.f, uiCamera));

		promptLabel = entities->AddComponent<Text>(entity);
		promptLabel->SetActive(true);
		promptLabel->SetFont(vcrMono);
	}

	// Game manager
	GameManager* gameManager = nullptr;
	{
		const unsigned entity = entities->Create();

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->SetDynamic(false);

		gameManager = entities->AddComponent<GameManager>(entity);
		gameManager->SetActive(true);
		gameManager->fadeInDuration = 100.f;
		gameManager->endDelay = 1.f;
		gameManager->sceneTexture = camera->GetFramebuffer()->GetTexture();
	}

	// Score controller
	ScoreController* scoreController = nullptr;
	{
		const unsigned entity = entities->Create();

		scoreController = entities->AddComponent<ScoreController>(entity);
		scoreController->SetActive(true);
		scoreController->indicatorPrefab = indicatorLabel;

		gameManager->scoreController = scoreController;
	}

	// post processing volume
	{
		const unsigned volume = entities->Create();
		Vignette* const vignette = entities->AddComponent<Vignette>(volume);
		vignette->SetActive(true);
		vignette->tint = COLOR_PURPLE; // vec3f(0.5f, 0.f, 0.5f);
		scoreController->vfx = vignette;
		//entities->AddComponent<CurveDisplay>(volume)->SetActive(true);
	}

	// Total score label
	Transform* scoreTransform = nullptr;
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		scoreTransform = entities->AddComponent<Transform>(entity);

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

		scoreController->totalScoreLabel = text;
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

		scoreController->buildScoreLabel = text;
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
		text->scale = 1.25f;
		text->text = "x8";
		text->color = COLOR_YELLOW; // vec4f(1.f, 0.749f, 0.f, 1.f);
		text->paragraphAlignment = PARAGRAPH_RIGHT;
		text->verticalAlignment = ALIGN_TOP;

		scoreController->multiplierLabel = text;
	}

	// health bar holder
	Transform* healthBarHolder = nullptr;
	Text* healthText = nullptr;
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		healthBarHolder = entities->GetComponent<Transform>(entity);

		Layout* const layout = entities->AddComponent<Layout>(entity);
		layout->SetActive(true);
		layout->AddConstraint(Constraint(RIGHT_ANCHOR, nullptr, RIGHT_ANCHOR, 1.f, -2.f, uiCamera));
		layout->AddConstraint(Constraint(BOTTOM_ANCHOR, nullptr, BOTTOM_ANCHOR, 1.f, 1.5f, uiCamera));
		layout->AddConstraint(Constraint(WIDTH, nullptr, NA, 1.f, 8.f, uiCamera));
		layout->AddConstraint(Constraint(HEIGHT, nullptr, NA, 1.f, 0.1f, uiCamera));

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);

		healthText = entities->AddComponent<Text>(entity);
		healthText->SetActive(true);
		healthText->SetFont(vcrMono);
		healthText->offset = vec3f(0.f, 0.75f, 0.f);
	}

	// health bar
	float* healthBarWidth = nullptr;
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		Layout* const layout = entities->AddComponent<Layout>(entity);
		layout->SetActive(true);
		layout->AddConstraint(Constraint(RIGHT_ANCHOR, healthBarHolder, RIGHT_ANCHOR, 1.f, 0.f, uiCamera));
		layout->AddConstraint(Constraint(BOTTOM_ANCHOR, healthBarHolder, TOP_ANCHOR, 1.f, 0.25f, uiCamera));
		layout->AddConstraint(Constraint(WIDTH, healthBarHolder, WIDTH, 1.f, 0.f, uiCamera));
		healthBarWidth = &(layout->GetConstraint(WIDTH)->multiplier);

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);
		render->tint = vec4f(1.f, 0.f, 0.f, 1.f);
	}

	// flash
	SpriteRender* flashOverlay = nullptr;
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		Layout* const layout = entities->AddComponent<Layout>(entity);
		layout->SetActive(true);
		layout->AddConstraint(Constraint(CENTER_X_ANCHOR, nullptr, CENTER_X_ANCHOR, 1.f, 0.f, uiCamera));
		layout->AddConstraint(Constraint(CENTER_Y_ANCHOR, nullptr, CENTER_Y_ANCHOR, 1.f, 0.f, uiCamera));
		layout->AddConstraint(Constraint(WIDTH, nullptr, WIDTH, 1.f, 0.f, uiCamera));
		layout->AddConstraint(Constraint(HEIGHT, nullptr, HEIGHT, 1.f, 0.f, uiCamera));

		flashOverlay = entities->AddComponent<SpriteRender>(entity);
		flashOverlay->SetActive(true);
		flashOverlay->tint = vec4f(1.f, 1.f, 1.f, 0.f);
	}

	// raw image frame
	//{
	//	const unsigned entity = entities->Create();

	//	auto transform = entities->GetComponent<Transform>(entity);
	//	transform->SetLocalTranslation(vec3f(12.f, 0.f, 1.f));
	//	transform->SetScale(vec3f(16.f, 9.f, 0.f));

	//	auto render = entities->AddComponent<SpriteRender>(entity);
	//	render->SetActive(true);
	//	render->SetSprite(camera->GetFramebuffer()->GetTexture());
	//}

	// energy meter
	float* meterHeight = nullptr;
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		Layout* const layout = entities->AddComponent<Layout>(entity);
		layout->SetActive(true);
		layout->AddConstraint(Constraint(LEFT_ANCHOR, nullptr, LEFT_ANCHOR, 1.f, 2.f, uiCamera));
		layout->AddConstraint(Constraint(BOTTOM_ANCHOR, nullptr, BOTTOM_ANCHOR, 1.f, 2.f, uiCamera));
		layout->AddConstraint(Constraint(HEIGHT, nullptr, NA, 1.f, 1.f, uiCamera));
		meterHeight = &(layout->GetConstraint(HEIGHT)->constant);

		//meterHeight = &(layout->GetConstraint(2).constant);

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);
	}

	// background
	AudioSource* backgroundAudio = nullptr;
	{
		const unsigned entity = entities->Create();

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->SetLocalTranslation(vec3f(0.f, 0.f, -5.f));
		transform->SetScale(vec3f(160.0f, 90.0f, 1.0f));
		transform->SetDynamic(false);

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);
		render->SetSprite(Load::TGA("Files/Textures/tile.tga"));
		render->SetMaterial(background);
		//render->SetCellRect(0, 0, 32, 18);
		render->SetCellRect(0, 0, 12, 6.75);
		render->tint.a = 0.15f;

		backgroundAudio = entities->AddComponent<AudioSource>(entity);
		backgroundAudio->SetActive(true);
		backgroundAudio->audioClip = song.path;
		backgroundAudio->loop = true;
	}

	// crosshair
	{
		const unsigned entity = entities->Create();

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->SetLocalTranslation(vec3f(0.f, 0.f, 5.f));
		transform->SetScale(1.25f);
		follow->crosshair = transform;

		TransformAnimator* const animator = entities->AddComponent<TransformAnimator>(entity);
		animator->SetActive(true);

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);
		render->SetMaterial(circle);
		//render->SetSprite(Load::TGA("Files/Textures/circle.tga"));

		CrosshairController* const controller = entities->AddComponent<CrosshairController>(entity);
		controller->SetActive(true);
		controller->view = camera;
	}

	// pickup label
	Text* pickupLabel = nullptr;
	{
		const unsigned entity = entities->Create();

		pickupLabel = entities->AddComponent<Text>(entity);
		pickupLabel->SetActive(true);
		pickupLabel->SetFont(vcrMono);
		pickupLabel->scale = 2.f;
		pickupLabel->text = "";
	}

	automatic->CreateAt(vec3f(15.f, 0.f, 0.f));
	laser->CreateAt(vec3f(-15.f, 0.f, 0.f));
	shotgun->CreateAt(vec3f(0.f, 15.f, 0.f));
	pistol->CreateAt(vec3f(0.f, -15.f, 0.f));
	sniper->CreateAt(vec3f(15.f, 15.f, 0.f));
	grenadeLauncher->CreateAt(vec3f(-15.f, -15.f, 0.f));

	// gun holder 
	Transform* weaponHolderTransform = nullptr;
	{
		const unsigned entity = entities->Create();
		weaponHolderTransform = entities->GetComponent<Transform>(entity);
	}

	// player
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, PLAYER);

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->SetScale(4.f);

		follow->player = transform;
		weaponHolderTransform->SetParent(transform);

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);
		render->SetSprite(playerSheet);
		render->SetTilemapSize(8, 17);

		SpriteAnimation* const spriteAnimation = entities->AddComponent<SpriteAnimation>(entity);
		spriteAnimation->SetActive(true);
		spriteAnimation->animations = Load::SAD("Files/Sprites/MainCharacter.sad");
		spriteAnimation->currentAnimation = "IDLE";

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
		collider->ignoreMask = BULLET | BONUS_BULLET;

		AudioSource* const audio = entities->AddComponent<AudioSource>(entity);
		audio->SetActive(true);

		PlayerMovement* const movement = entities->AddComponent<PlayerMovement>(entity);
		movement->SetActive(true);
		movement->speed = 400.f;
		movement->dash = 30.f;
		movement->bounds = vec3f(80.f, 45.f, 1.f);

		PlayerCombat* const combat = entities->AddComponent<PlayerCombat>(entity);
		combat->SetActive(true);
		combat->weaponHolder = weaponHolderTransform;
		combat->sfxPrefab = sfxEmitter;
		combat->pickupLabel = pickupLabel;
		combat->SetCrosshair(follow->crosshair);
		combat->SetHolder(weaponHolderTransform);

		collider->handlers[COLLISION_ENTER].Bind(&PlayerCombat::OnCollisionEnter, combat);
		collider->handlers[COLLISION_EXIT].Bind(&PlayerCombat::OnCollisionExit, combat);

		PlayerLife* const life = entities->AddComponent<PlayerLife>(entity);
		life->SetActive(true);
		life->maxHealth = 4.f;
		life->inviDuration = 3.f;
		life->flashDuration = 0.1f;
		life->barMultiplier = healthBarWidth;
		life->healthText = healthText;
		life->flashOverlay = flashOverlay;
		life->indicatorLabel = indicatorLabel;
		collider->handlers[COLLISION_ENTER].Bind(&PlayerLife::OnCollisionEnter, life);

		BeatController* const beat = entities->AddComponent<BeatController>(entity);
		beat->SetActive(true);
		// audio
		beat->source = backgroundAudio;
		beat->startFrequency = 20;
		beat->endFrequency = 2000;
		beat->audioDuration = 1.f;
		beat->frequencyBands = 50;
		beat->maxHeight = 10.f;
		beat->meterHeight = meterHeight;
		// references
		beat->promptLabel = promptLabel;
		beat->indicatorPrefab = indicatorLabel;
		beat->background = background;
		// tempo
		//beat->threshold = 0.2f;
		beat->goodThreshold = 0.3f;
		beat->greatThreshold = 0.15f;
		beat->perfectThreshold = 0.075f;
		beat->SetTempo(song.tempo);

		gameManager->beatController = beat;
	}

	// enemy manager
	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, ENEMY);

		EnemyManager* const manager = entities->AddComponent<EnemyManager>(entity);
		//manager->SetActive(true);
		manager->boundary = vec2f(80.f, 45.f);
		manager->player = follow->player;
		manager->sfxPrefab = sfxEmitter;

		const TargetStyle trackPlayer = { TARGET_LOCKON, MOVEMENT_CONSTANT, 100.f, 30.f };
		const TargetStyle slowTrack = { TARGET_LOCKON, MOVEMENT_CONSTANT, 50.f, 20.f };
		const TargetStyle dashPlayer = { TARGET_LOCKON, MOVEMENT_CONSTANT, 300.f, 25.f };
		const TargetStyle dash = { TARGET_DASH, MOVEMENT_CONSTANT, 400.f, 25.f };
		const TargetStyle avoidPlayer = { TARGET_LOCKON, MOVEMENT_CONSTANT, -200.f, 20.f };
		const TargetStyle roam = { TARGET_RANDOM, MOVEMENT_CONSTANT, 150.f, 30.f };
		const TargetStyle slowRoam = { TARGET_RANDOM, MOVEMENT_CONSTANT, 0.f, 30.f };
		
		manager->AddEnemy(EnemyData{ basicEnemy, shotgun, 1.f, 0, 5, 5, true, trackPlayer, avoidPlayer, true, 80, 15, 1 });
		manager->AddEnemy(EnemyData{ basicEnemy, pistol, 1.f, 0, 10, 5, true, trackPlayer, avoidPlayer, true, 30, 10, 1 });
		manager->AddEnemy(EnemyData{ fireElementalEnemy, nullptr, 1.f, 0, 1, 5, true, roam, dash, true, 2, 5, 1 });
		manager->AddEnemy(EnemyData{ cyclopsEnemy, nullptr, 1.f, 0, 10, 5, false, slowRoam, slowTrack, false, 100, 2, 1 });

		//manager->AddEnemy(EnemyData{ basicEnemy, shotgun, 1.f, 0, 5, 5, true, trackPlayer, avoidPlayer, true, 170, 30, 1 });
		//manager->AddEnemy(EnemyData{ basicEnemy, pistol, 1.f, 0, 10, 5, true, trackPlayer, avoidPlayer, true, 60, 20, 1 });
		//manager->AddEnemy(EnemyData{ fireElementalEnemy, nullptr, 1.f, 0, 1, 5, true, roam, dash, true, 5, 10, 5 });
		//manager->AddEnemy(EnemyData{ cyclopsEnemy, nullptr, 1.f, 0, 10, 5, false, slowRoam, slowTrack, false, 200, 0, 1 });
		//manager->AddEnemy(EnemyData{ batEnemy, nullptr, 1.f, 0, 1, 5, true, trackPlayer, dashPlayer, true, 2, 20, 3 });
		//manager->AddEnemy(EnemyData{ iceElementalEnemy, nullptr, 1.f, 0, 1, 5, true, roam, dash, true, 2, 15, 3 });

		//manager->AddEnemy(EnemyData{ basicEnemy, pink, 0, 1, 5, true, roam, dashPlayer, RISK_LOW, 1, 3, 5, 1 });
		//manager->AddEnemy(EnemyData{ basicEnemy, orange, 0, 1, 5, true, roam, dash, RISK_LOW, 1, 4, 8, 2 });
		//manager->AddEnemy(EnemyData{ basicEnemy, yellow, 0, 1, 5, TARGET_PLAYER, MOVEMENT_CONSTANT, 200.f, 300.f, 20.f, RISK_LOW, 1, 10, 5 });
		//manager->AddEnemy(EnemyData{ basicEnemy, pink, 0, 1, 5, TARGET_PLAYER, MOVEMENT_CONSTANT, 200.f, 300.f, 20.f, RISK_LOW, 1, 10, 5 });
		//manager->AddEnemy(EnemyData{ basicEnemy, orange, 0, 1, 5, TARGET_PLAYER, MOVEMENT_CONSTANT, 200.f, 300.f, 20.f, RISK_LOW, 1, 10, 5 });
		//manager->AddEnemy(EnemyData{ batEnemy, RISK_LOW, 1, 2, 1 });
	}
}

void MainGame::Destroy() {
	Scene::Destroy();

	delete background;

	delete indicatorLabel;
	delete sfxEmitter;
	
	delete explosionArea;
	delete basicBullet;
	delete longBullet;
	delete explosiveBullet;

	delete pistol;
	delete automatic;
	delete shotgun;
	delete sniper;
	delete grenadeLauncher;
	delete laser;

	delete basicEnemy;
	delete batEnemy;
}
