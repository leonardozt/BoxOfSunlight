#pragma once

#include "Camera.h"
#include "Cubemap.h"

namespace BOSL
{
	class Scene
	{
	public:
		Scene();
		
		Camera camera;
		Cubemap cubemap;
	};
}