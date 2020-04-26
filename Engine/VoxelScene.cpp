#include "VoxelScene.h"

// component
#include "Camera.h"
#include "Light.h"
#include "EditorCamera.h"
#include "VoxelRenderer.h"
#include "SpriteRenderer.h"
// post processing
#include "CurveDisplay.h"
#include "Bloom.h"
// UI components
#include "Panel.h"
// Systems
#include "LayoutSystem.h"
#include "ColliderSystem.h"
// Utils
#include "Layers.h"
#include "VoxelUnlitMaterial.h"

#include <Math/Random.hpp>

void VoxelScene::Awake() {
	Scene::Awake();

	systems->Subscribe<LayoutSystem>(1);
	systems->Subscribe<ColliderSystem>(2);
}

void VoxelScene::Create() {
	Scene::Create();

	Camera* const camera = entities->GetComponent<Camera>(mainCamera);
	camera->SetSize(12.f);
	camera->shouldClear = false;
	camera->SetMatch(1.f);
	camera->projection = ORTHOGRAPHIC;
	camera->cullingMask = UI;

	// post processing voluem
	{
		const unsigned volume = entities->Create();
		entities->AddComponent<Bloom>(volume)->SetActive(true);
	}

	// editor view
	CreateEditor();

	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		Transform* const transform = entities->GetComponent<Transform>(entity);
		//transform->translation.x = -10.f;
		//transform->scale = vec3f(4.f, 9.f, 0.f);

		Layout* const layout = entities->AddComponent<Layout>(entity);
		layout->SetActive(true);		
		layout->AddConstraint(Constraint(LEFT, nullptr, LEFT, 1, 0, camera));
		layout->AddConstraint(Constraint(TOP, nullptr, TOP, 1, 0, camera));
		layout->AddConstraint(Constraint(BOTTOM, nullptr, BOTTOM, 1, 0, camera));
		layout->AddConstraint(Constraint(WIDTH, nullptr, NA, 1, 2, camera));

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);
		render->tint = vec4f(1.f, 0.f, 0.f, 1.f);

		Panel* const panel = entities->AddComponent<Panel>(entity);
		panel->SetActive(true);
		panel->camera = camera;
	}

	{
		const unsigned entity = entities->Create();
		entities->SetLayer(entity, UI);

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->scale = 0.2f;

		SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
		render->SetActive(true);
		render->tint = vec4f(0.f, 1.f, 0.f, 1.f);

		Panel* const panel = entities->AddComponent<Panel>(entity);
		panel->SetActive(true);
		panel->camera = camera;
	}
}

void VoxelScene::CreateEditor() {
	EditorCamera* editorControls;
	// editor camera
	{
		const unsigned entity = entities->Create();

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->translation.z = 10.f;

		Camera* const camera = entities->AddComponent<Camera>(entity);
		camera->SetActive(true);
		camera->SetSize(10.f);
		camera->SetDepth(-1.f);
		camera->clearColor = 0.f;
		camera->cullingMask -= UI;

		editorControls= entities->AddComponent<EditorCamera>(entity);
		editorControls->SetActive(true);
	}

	// directional light
	{
		const unsigned entity = entities->Create();

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->rotation = vec3f(50.f, -30.f, 0.f);
		transform->UpdateAxes();

		Light* const light = entities->AddComponent<Light>(entity);
		light->SetActive(true);
		light->intensity = 0.5f;
		light->color = vec4f(253.f / 255.f, 251.f / 255.f, 211.f / 255.f, 1.0f);
		light->type = LIGHT_DIRECTIONAL;
	}

	{
		const unsigned entity = entities->Create();

		Transform* const transform = entities->GetComponent<Transform>(entity);
		transform->translation = vec3f(0, 2.f, 0);
		transform->SetDynamic(false);

		Light* const light = entities->AddComponent<Light>(entity);
		light->SetActive(true);
		light->color = 1.f;
		light->intensity = 10.f;
		light->type = LIGHT_POINT;

		VoxelRender* const render = entities->AddComponent<VoxelRender>(entity);
		render->SetActive(true);
		render->tint = vec4f(1.f);
		render->SetMaterial(new Material::VoxelUnlit);
		render->SetDynamic(false);

		BoxCollider* const collider = entities->AddComponent<BoxCollider>(entity);
		collider->SetActive(true);
	}

	const float size = 10.f;

	for (float x = -size; x <= size; ++x) { 
		for (float z = -size; z <= size; ++z) {
			const unsigned entity = entities->Create();

			Transform* const transform = entities->GetComponent<Transform>(entity);
			transform->translation = vec3f(x, -1.f, z);
			transform->SetDynamic(false);

			VoxelRender* const render = entities->AddComponent<VoxelRender>(entity);
			render->SetActive(true);
			render->tint = vec4f(Math::RandValue(), Math::RandValue(), Math::RandValue(), 1.f);
			render->SetDynamic(false);

			BoxCollider* const collider = entities->AddComponent<BoxCollider>(entity);
			collider->SetActive(true);
		}
	}
}
