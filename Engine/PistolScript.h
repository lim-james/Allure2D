#ifndef PISTOL_SCRIPT_H
#define PISTOL_SCRIPT_H

#include "WeaponBase.h"

struct PistolScript : WeaponBase {

	using base_type = WeaponBase;

	void Trigger() override;
	void Hold(float const& dt) override;
	void Release() override;
	vec3f HoldOffset() const override;
	std::string Name() const override;

private:

	void CreateBullet(bool const& onBeat, vec2f const& direction) const;

};

#endif
