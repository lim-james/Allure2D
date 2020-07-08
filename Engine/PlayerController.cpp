#include "PlayerController.h"

#include "InputEvents.h"
#include "ProjectDefines.h"

#include <Events/EventsManager.h>
#include <GLFW/glfw3.h>

void PlayerController::SetShieldTransform(Transform * const t) {
	shieldTransform = t;
	shieldRender = entities->GetComponent<SpriteRender>(t->entity);
}

void PlayerController::Awake() {
	EventsManager::Get()->Subscribe("KEY_INPUT", &PlayerController::KeyHandler, this);
	EventsManager::Get()->Subscribe("MOUSE_BUTTON_INPUT", &PlayerController::MouseButtonHandler, this);
	EventsManager::Get()->Subscribe("CURSOR_POSITION_INPUT", &PlayerController::CursorPositionHandler, this);
}

void PlayerController::KeyHandler(Events::Event * event) {
	Events::KeyInput* input = static_cast<Events::KeyInput*>(event);

	void(PlayerController::*press)() = nullptr;
	void(PlayerController::*hold)() = nullptr;
	void(PlayerController::*release)() = nullptr;

	switch (input->key) {
	case GLFW_KEY_Z:
	case GLFW_KEY_X:
		press = &PlayerController::BeatPress;
		hold = &PlayerController::BeatHold;
		release = &PlayerController::BeatRelease;
		break;
	case GLFW_KEY_SPACE:
		press = &PlayerController::WavePress;
		hold = &PlayerController::WaveHold;
		release = &PlayerController::WaveRelease;
		break;
	default:
		return;
	}

	switch (input->action) {
	case GLFW_PRESS:
		(this->*press)();
		break;
	case GLFW_REPEAT:
		(this->*hold)();
		break;
	case GLFW_RELEASE:
		(this->*release)();
		break;
	default:
		break;
	}
}

void PlayerController::MouseButtonHandler(Events::Event * event) {
	Events::MouseButtonInput* input = static_cast<Events::MouseButtonInput*>(event);
}

void PlayerController::CursorPositionHandler(Events::Event * event) {
	Events::CursorPositionInput* input = static_cast<Events::CursorPositionInput*>(event);
	const vec2f position = view->ScreenToWorldPosition(input->position);
	shieldTransform->SetLocalRotation(vec3f(0.f, 0.f, atan2f(position.y, position.x) * Math::toDeg));
}

void PlayerController::BeatPress() {
	shieldRender->tint = COLOR_BLACK;
}

void PlayerController::BeatHold() {

}

void PlayerController::BeatRelease() {
	shieldRender->tint = COLOR_GREY;
}

void PlayerController::WavePress() {
	indicatorRender->tint = COLOR_BLACK;
}

void PlayerController::WaveHold() {

}

void PlayerController::WaveRelease() {
	indicatorRender->tint = COLOR_LIGHT_GREY;
}
