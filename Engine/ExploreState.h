#ifndef EXPLORE_STATE_H
#define EXPLORE_STATE_H

#include "State.h"
#include "Team.h"

namespace States {

	struct Explore : State {

		void Enter(const unsigned& target, EntityManager * const entities) override;
		void Update(const unsigned& target, const float& dt, EntityManager * const entities) override;
		void Exit(const unsigned& target, EntityManager * const entities) override;

	private:

		void Scan(Team * const team);

	};

}

#endif
