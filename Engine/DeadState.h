#ifndef DEAD_STATE_H
#define DEAD_STATE_H

#include "State.h"

namespace States {

	struct Dead : State {
		void Enter(unsigned const& target, EntityManager * const entities) override;
		void Update(unsigned const& target, float const& dt, EntityManager * const entities) override;
		void FixedUpdate(unsigned const& target, float const& dt, EntityManager * const entities) override;
		void Exit(unsigned const& target, EntityManager * const entities) override;
	};

}

#endif
