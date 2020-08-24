#pragma once

#include <sstream>
#include <vector>
#include "myvec.h"

// just represents 3 points in space 
struct tri {
	
	vec3f points[3]; 
	
	/*vec3f calcNormal() const; */


// Implementations
public: 

	// finds normal to triangle - assumes clockwise windings to points
	vec3f calcNormal() const {
		vec3f t1 = points[1] - points[0]; 
		vec3f t2 = points[2] - points[0]; 

		return t2.crossProduct(t1); 
	}
};



struct mesh {

	std::vector<tri> triangles; 

	// just leave this here for now
	bool parseObjFile(std::string filename) {

		std::ifstream f(filename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		std::vector<vec3f> verts;
		std::string str;

		while (std::getline(f, str))
		{	 

			std::stringstream stream(str);

			char junk;

			if (str[0] == 'v')
			{
				vec3f v;
				stream >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (str[0] == 'f')
			{
				int f[3];
				stream >> junk >> f[0] >> f[1] >> f[2];
				triangles.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}
		f.close();

		return true;
	}
};