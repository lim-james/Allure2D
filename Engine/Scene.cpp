#include "Scene.h"

#include "TransformSystem.h"
#include "RenderSystem.h"

#include <Math/Vectors.hpp>
#include <Events/EventsManager.h>

int Scene::count = 0;

Scene::Scene() {
	entities = new EntityManager;
	systems = new SystemManager(entities);
}

Scene::~Scene() {
	delete entities;
	delete systems;
}

void Scene::Awake() {
	systems->Subscribe<TransformSystem>(0);
	systems->Subscribe<RenderSystem>(0);
}

void Scene::Create() {
	mainCamera = entities->Create();
	entities->GetComponent<Transform>(mainCamera)->translation.z = 1.f;
	Camera * camera = entities->AddComponent<Camera>(mainCamera);
	camera->SetActive(true);
	camera->clearColor.Set(0.f);
}

void Scene::Enter() {
	Events::EventsManager::GetInstance()->SubscribeContext(this);
	systems->Start();
}

void Scene::FixedUpdate(const float& dt) { }

void Scene::Update(const float& dt) {
	systems->Update(dt);
}

void Scene::Exit() {
	systems->Stop();
	Events::EventsManager::GetInstance()->UnsubscribeContext(this);
}

void Scene::Destroy() { }

void Scene::PrepareForSegue(Scene * destination) { }
