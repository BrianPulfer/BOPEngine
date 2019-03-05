#include "bopEngine.h"

bool equals(glm::mat4 mat1, glm::mat4 mat2) {
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			if (mat1[row][col] != mat2[row][col])
				return false;
		}
	}

	return true;
}

void printMatrix(glm::mat4 mat) {
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			std::cout << mat[row][col] << "  ";
		}
		std::cout << std::endl;
	}
}