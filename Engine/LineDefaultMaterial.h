#ifndef LINE_DEFAULT_MATERIAL_H
#define LINE_DEFAULT_MATERIAL_H

#include "Material.h"

namespace Material {
	struct LineDefault: Base {
		LineDefault();
	private:
		void SetAttributes() override;
	};
}

#endif
