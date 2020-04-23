#ifndef SPRITE_DEFAULT_MATERIAL_H
#define SPRITE_DEFAULT_MATERIAL_H

#include "Material.h"

namespace Material {
	struct SpriteDefault: Base {
		SpriteDefault();
	private:
		void SetAttributes() override;
	};
}

#endif
