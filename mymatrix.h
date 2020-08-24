#pragma once

#include "myvec.h"
#include "mymesh.h"



struct mat4x4 {

	float mat[4][4] = {0}; 
	
	/*vec4f rightMultiplyVec(const vec4f& vec) const; 
	mat4x4 rightMultiplyMatrix(const mat4x4& mat2) const; */

// Implementations
public: 

	// right multiply the matrix by a 4 vector
	vec4f rightMultiplyVec(const vec4f& vec) const {
		
		vec4f temp;
		
		temp.x = (this->mat[0][0] * vec.x) + (this->mat[0][1] * vec.y) + (this->mat[0][2] * vec.z) + (this->mat[0][3] * vec.w); 
		temp.y = (this->mat[1][0] * vec.x) + (this->mat[1][1] * vec.y) + (this->mat[1][2] * vec.z) + (this->mat[1][3] * vec.w);
		temp.z = (this->mat[2][0] * vec.x) + (this->mat[2][1] * vec.y) + (this->mat[2][2] * vec.z) + (this->mat[2][3] * vec.w);
		temp.w = (this->mat[3][0] * vec.x) + (this->mat[3][1] * vec.y) + (this->mat[3][2] * vec.z) + (this->mat[3][3] * vec.w);
																								 
		return temp; 
	}

	// right multiply parameter matrix by the calling matrix 
	mat4x4 rightMultiplyMatrix(const mat4x4& m) const {
	
		mat4x4 temp; 

		for (int row = 0; row < 4; row++) {
			for (int col = 0; col < 4; col++) {
				temp.mat[row][col] = (this->mat[row][0] * m.mat[0][col]) + (this->mat[row][1] * m.mat[1][col]) + (this->mat[row][2] * m.mat[2][col]) + (this->mat[row][3] * m.mat[3][col]); 
			}
		}

		return temp; 	
	}

};