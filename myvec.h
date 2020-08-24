#pragma once

#include <cmath>

struct vec3f {
	float x = 0; 
	float y = 0; 
	float z = 0; 

	//vec3f operator+(const vec3f& op2) const; 
	//vec3f addVec3f(const vec3f& op2) const; 

	//vec3f operator-(const vec3f& op2) const; 
	//vec3f subVec3f(const vec3f& op2) const; 

	//float operator*(const vec3f& op2) const; 
	//float dotProduct(const vec3f& op2) const; 

	//void operator=(const vec3f& op2); 

	//vec3f crossProduct(const vec3f& op2) const; 

	//float magnitude() const; 
	//
	//vec3f normalizeVec() const; 




// Implementations
public: 

	/* component-wise addition ------------------------ */
	vec3f operator+(const vec3f& op2) const {
		
		vec3f temp; 
		
		temp.x = this->x + op2.x; 
		temp.y = this->y + op2.y; 
		temp.z = this->z + op2.z; 

		return temp; 
	}

	vec3f addVec3f(const vec3f& op2) const{

		return (*this) + op2; 
	}
	

	
	/* component-wise subtration ---------------------- */
	vec3f operator-(const vec3f& op2) const {

		vec3f temp;

		temp.x = this->x - op2.x;
		temp.y = this->y - op2.y;
		temp.z = this->z - op2.z;

		return temp;
	}

	vec3f subVec3f(const vec3f& op2) const{

		return (*this) - op2;
	}


	/* dot product ------------------------------------ */
	float operator*(const vec3f& op2) const {
		
		return (this->x * op2.x) + (this->y * op2.y) + (this->z * op2.z); 
	}

	float dotProduct(const vec3f& op2) const {
	
		return (*this) * op2; 
	}

	/* assigning a vector using the = operator */
	void operator=(const vec3f& op2) {
	
		this->x = op2.x; 
		this->y = op2.y; 
		this->z = op2.z; 
	}


	/* cross product - calling vector matters (order for cross product matters) ----- */
	vec3f crossProduct(const vec3f& op2) const {
	
		vec3f temp;

		temp.x = (this->y * op2.z) - (this->z * op2.y); 
		temp.y = (this->z * op2.x) - (this->x * op2.z);  // -1 * [ (this->x * op2.z) - (this->z * op2.x) ] 
		temp.z = (this->x * op2.y) - (this->y * op2.x);

		return temp; 
	}

	/* returns the magnitude of the calling vector */
	float magnitude() const {
		return std::sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z)); 
	}

	/* returns a normalized vector with same direction */
	vec3f normalizeVec() const {
	
		float mag = (*this).magnitude(); 
		
		vec3f ret; 

		ret.x = this->x / mag; 
		ret.y = this->y / mag; 
		ret.z = this->z / mag; 

		return ret; 
	}

};





/* Structure for a 4 vector */

struct vec4f {
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1; 

	/*vec4f operator+(const vec4f& op2) const;
	vec4f addVec4f(const vec4f& op2) const;

	vec4f operator-(const vec4f& op2) const;
	vec4f subVec4f(const vec4f& op2) const;

	void operator=(const vec4f& op2);*/



// Implemenations 
public:

	/* component-wise addition ------------------------ */
	vec4f operator+(const vec4f& op2) const {

		vec4f temp;

		temp.x = this->x + op2.x;
		temp.y = this->y + op2.y;
		temp.z = this->z + op2.z;
		temp.w = this->w + op2.w; 

		return temp;
	}

	vec4f addVec4f(const vec4f& op2) const {

		return (*this) + op2;
	}



	/* component-wise subtration ---------------------- */
	vec4f operator-(const vec4f& op2) const {

		vec4f temp;

		temp.x = this->x - op2.x;
		temp.y = this->y - op2.y;
		temp.z = this->z - op2.z;
		temp.w = this->w - op2.w; 

		return temp;
	}

	vec4f subVec4f(const vec4f& op2) const {

		return (*this) - op2;
	}

	
	vec3f normalize() {
		
		if (this->w != 0.0f) {
			return { this->x / this->w, this->y / this->w, this->z / this->w }; 
		}

		return { this->x, this->y, this->z }; 
	}


	/* assigning a vector using the = operator */
	void operator=(const vec4f& op2) {

		this->x = op2.x;
		this->y = op2.y;
		this->z = op2.z;
		this->w = op2.w; 
	}

}; 