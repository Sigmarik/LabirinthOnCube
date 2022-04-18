#include "tform.h"

glm::vec3 tform::extractLocation(glm::mat4 matrix) {
	return glm::vec3(matrix[3]) / matrix[3][3];
}

glm::mat4 tform::centered(glm::mat4 matrix) {
	return glm::mat4(
		glm::vec4(glm::vec3(matrix[0]), 0.0),
		glm::vec4(glm::vec3(matrix[1]), 0.0),
		glm::vec4(glm::vec3(matrix[2]), 0.0),
		glm::vec4(0.0, 0.0, 0.0, matrix[3][3])
	);
}

std::ostream& operator<<(std::ostream& stream, glm::mat4 matrix) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			stream << matrix[i][j] << ' ';
		}
		stream << '\n';
	}
	return stream;
}