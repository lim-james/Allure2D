#ifndef EXPAND_SCRIPT_H
#define EXPAND_SCRIPT_H

#include "Script.h"

struct ExpandScript : Script {

	using base_type = ExpandScript;

	float duration;
	vec3f targetSize;

private:

	float et;

	void Start() override;
	void Update() override;

};

#endif
