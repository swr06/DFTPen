#pragma once
#include <iostream>
#include <glm/glm.hpp>

namespace Simulation {
	struct Sample {
		glm::vec2 Position;
		bool Active;
	};
}