#include "InputController.h"

#include "InputEvents.h"

#include <Events/EventsManager.h>
#include <Logger/Logger.h>
#include <GLFW/glfw3.h>

#include <map>

InputController::InputController()
	: context(nullptr)
	, sensitivity(1.0f)
	, cursorPosition(0.0f) {}

void InputController::Initialize(GLFWwindow* current) {
	context = current;

	glfwSetKeyCallback(context, KeyCallback);
	glfwSetCharCallback(context, CharCallback);
	glfwSetCursorPosCallback(context, CursorPosCallback);
	glfwSetMouseButtonCallback(context, MouseButtonCallback);
	glfwSetScrollCallback(context, ScrollCallback);
	glfwSetDropCallback(context, DropCallback);
	glfwSetJoystickCallback(JoystickCallback);

	sensitivity = 1.0f;

	EventsManager* const em = EventsManager::Get();
	em->Subscribe("CURSOR_POS_UPDATE", &InputController::OnEvent, this);
	em->Subscribe("CURSOR_SENSITIVITY", &InputController::OnEvent, this);
	em->Subscribe("INPUT_MODE_CHANGE", &InputController::OnEvent, this);
}

void InputController::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Events::KeyInput* data = new Events::KeyInput(key, scancode, action, mods);
	EventsManager::Get()->Trigger("KEY_INPUT", data);
}

void InputController::CharCallback(GLFWwindow* window, unsigned codepoint) {
	Events::TextInput* data = new Events::TextInput(static_cast<char>(codepoint));
	EventsManager::Get()->Trigger("TEXT_INPUT", data);
}

void InputController::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	const vec2f position(static_cast<float>(xpos), static_cast<float>(ypos));
	Events::AnyType<vec2f>* data = new Events::AnyType<vec2f>(position);
	EventsManager::Get()->Trigger("CURSOR_POS_UPDATE", data);
}

void InputController::MouseButtonCallback(GLFWwindow* window, int key, int action, int mods) {
	Events::MouseButtonInput* data = new Events::MouseButtonInput(key, action, mods);
	EventsManager::Get()->Trigger("MOUSE_BUTTON_INPUT", data);
}

void InputController::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	Events::ScrollInput* data = new Events::ScrollInput(
		vec2f(static_cast<float>(xOffset), static_cast<float>(yOffset))
	);
	EventsManager::Get()->Trigger("SCROLL_INPUT", data);
}

void InputController::DropCallback(GLFWwindow * window, int count, const char ** paths) {
	Events::DropInput* data = new Events::DropInput(count, paths);
	EventsManager::Get()->Trigger("DROP_INPUT", data);
}

void InputController::JoystickCallback(int id, int event) {
	Events::JoystickState* data = new Events::JoystickState(id, event);
	EventsManager::Get()->Trigger("JOYSTICK_STATE", data);
}

void InputController::OnEvent(Events::Event* event) {
	if (event->name == "CURSOR_POS_UPDATE") {
		const auto update = static_cast<Events::AnyType<vec2f>*>(event);
		vec2f const& position = update->data;
		const vec2f delta = cursorPosition - position;
		Events::CursorPositionInput* input = new Events::CursorPositionInput(position, delta * sensitivity);
		EventsManager::Get()->Trigger("CURSOR_POSITION_INPUT", input);
		cursorPosition = position;
	} else if (event->name == "CURSOR_SENSITIVITY") {
		const auto input = static_cast<Events::AnyType<float>*>(event);
		sensitivity = input->data;
	} else if (event->name == "INPUT_MODE_CHANGE") {
		const auto input = static_cast<Events::InputMode*>(event);
		glfwSetInputMode(context, input->mode, input->value);
	}

}

