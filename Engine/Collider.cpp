#include "Collider.h"

#include <Events/EventsManager.h>

Collider::Collider() : tag("") {
	for (auto& h : handlers) {
		h.UnbindAll();
	}
}

void Collider::Initialize() {
	tag = "";
	ignore.clear();

	for (auto& h : handlers) {
		h.UnbindAll();
	}
}

Component * Collider::Clone() const {
	return new Collider(*this);
}

void Collider::SetActive(bool const& state) {
	Component::SetActive(state);
	Events::EventsManager::GetInstance()->Trigger("COLLIDER_ACTIVE", new Events::AnyType<Collider*>(this));
}

