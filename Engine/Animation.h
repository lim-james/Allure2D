#ifndef ANIMATION_H
#define ANIMATION_H

#include "Component.h"

#include <Handler/Handler.h>

#include <vector>
#include <map>

class AnimationBase {

protected:

	bool loop;
	float duration;
	float delay;

	Handler<void, void> completion;

	float et;

public:

	AnimationBase(
		const bool& loop = false,
		const float& duration = 0.f,
		const float& delay = 0.f,
		Handler<void, void> completion = nullptr);

	virtual ~AnimationBase() {}

	virtual void Update(const float& dt);
	const bool IsActive() const;
};

template<typename T>
class AnimationData : public AnimationBase {

private:

	T* target;
	T outcome;
	T original;

public:

	AnimationData() {}
	~AnimationData() override {}

	AnimationData(AnimationBase base, T* target, const T& outcome)
		: AnimationBase(base)
		, target(target)
		, outcome(outcome)
		, original(*target) {}


	void Update(const float& dt) override {
		AnimationBase::Update(dt);

		if (et >= duration) {
			if (loop) {
				*target = original;
				et = 0.f;
			} else {
				*target = outcome;
				completion();
			}
		} else if (et > 0.f) {
			const T interval = outcome - *(target);
			const float ratio = dt / (duration - et);
			*(target) += interval * ratio;
		}
	}
};

struct Animation : Component {

	std::map<int, AnimationBase*> animations;

	Animation();
	~Animation() override;

	void Initialize() override;
	Component* Clone() const override;
	void SetActive(const bool& state) override;

	template<typename T>
	void Animate(const AnimationBase& base, T* target, const T& outcome) {
		const int t = (int)target;
	
		if (animations[t])
			delete animations[t];

		animations[t] = new AnimationData<T>(base, target, outcome);
		//animations.push_back(new AnimationData<T>(base, &target, outcome));
	}

	void Clear();

};

#endif