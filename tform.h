#ifndef GAME_COMPONENT_CLASS_H
#define GAME_COMPONENT_CLASS_H

#include <iostream>
#include <glm/glm.hpp>
#include <vector>

#define Transform glm::mat4

namespace tform {
	glm::vec3 extractLocation(glm::mat4 matrix);
	glm::mat4 centered(glm::mat4 matrix);
}

std::ostream& operator<<(std::ostream& stream, glm::mat4 tform);

#endif