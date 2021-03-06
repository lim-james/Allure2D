#include "Physics.h"

#include <Events/EventsManager.h>

Physics::Physics()
	: drag(0.f)
	, angularDrag(0.5f)
	, useGravity(true)
	, interpolate(true)
	, velocity(0.f) 
	, angularVelocity(0.f)
	, mass(1.f)
	, invMass(1.f)
	, netForce(0.f)
	, torque(0.f) {}

void Physics::Initialize() {
	drag = 0.f;
	angularDrag = 0.5f;
	useGravity = true;
	interpolate = true;

	velocity = 0.f;
	angularVelocity = 0.f;

	SetMass(1.f);
	
	netForce = 0.f;
	torque = 0.f;
}

Component * Physics::Clone() const {
	return new Physics(*this);
}

void Physics::SetActive(bool const & state) {
	Component::SetActive(state);
	EventsManager::Get()->Trigger("PHYSICS_ACTIVE", new Events::AnyType<Physics*>(this));
}

float const & Physics::GetMass() const {
	return mass;
}

float const & Physics::GetInverseMass() const {
	return invMass;
}

void Physics::SetMass(float const & value) {
	mass = value;
	invMass = 1.f / value;
}

void Physics::AddForce(vec3f const & force) {
	netForce += force;
}
