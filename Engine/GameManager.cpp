#include "GameManager.h"

#include "InputEvents.h"
#include "PauseScene.h"

#include <Events/EventsManager.h>
#include <GLFW/glfw3.h>

void GameManager::Awake() {
	EventsManager::Get()->Subscribe("KEY_INPUT", &GameManager::KeyHanlder, this);
}

void GameManager::KeyHanlder(Events::Event * event) {
	const auto input = static_cast<Events::KeyInput*>(event);

	if (input->key == GLFW_KEY_ESCAPE && input->action == GLFW_PRESS) {
		EventsManager::Get()->Trigger("PRESENT_SCENE", new Events::PresentScene(new PauseScene));
	}
}
