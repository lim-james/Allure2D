#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include "System.h"

#include <MACROS.h>

#include <vector>
#include <map>
#include <typeindex>

class SystemManager {

	EntityManager * const manager;

	std::map<unsigned, System*> systems;
	std::vector<System*> fixedSystems;

	std::map<System*, float> frameDelta;
	std::vector<std::vector<System*>> frameLayout;
	std::vector<std::vector<System*>>::iterator frame;

public:

	SystemManager(EntityManager * const manager);
	~SystemManager();

	void Start();
	void Stop();

	template<typename SystemType>
	void Subscribe(unsigned const& frameIndex);
	template<typename SystemType>
	void SubscribeFixed();

	template<typename SystemType>
	SystemType* const Get();

	void Update(float const& dt);
	void FixedUpdate(float const& dt);

};

template<typename SystemType>
void SystemManager::Subscribe(unsigned const& frameIndex) {
	while (frameLayout.size() <= frameIndex) {
		frameLayout.push_back({});
	}

	const unsigned hash = hashof(SystemType);

	System* s = systems[hash];
	if (!s) {
		s = new SystemType;
		s->entities = manager;
		s->Initialize();
		systems[hash] = s;
		frameDelta[s] = 0.f;
	} 

	frameLayout[frameIndex].push_back(s);
}

template<typename SystemType>
void SystemManager::SubscribeFixed() {
	const unsigned hash = hashof(SystemType);

	System* s = systems[hash];
	if (!s) {
		s = new SystemType;
		s->entities = manager;
		s->Initialize();
		systems[hash] = s;
		frameDelta[s] = 0.f;
	} 

	fixedSystems.push_back(s);
}

template<typename SystemType>
SystemType * const SystemManager::Get() {
	return static_cast<SystemType*>(systems[hashof(SystemType)]);
}

#endif


