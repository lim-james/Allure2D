#ifndef STATE_CONTAINER_H
#define STATE_CONTAINER_H

#include "Component.h"

#include <string>

struct StateContainer : Component {

	using base_type = StateContainer;

	std::string queuedState;
	std::string currentState;

	StateContainer();

	void Initialize() override;
	Component* Clone() const override;
	void SetActive(bool const& state) override;

};

#endif
