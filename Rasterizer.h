#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <string>
#include <fstream>


#include "ConsoleEngine.h"
#include "mymatrix.h"
#include "myvec.h"
#include "mymesh.h"
#include "mycolors.h"



class Rasterizer : public ConsoleEngine {

public:

	Rasterizer() {

		sourceFile = "";
		windowName = L"Rasterizer";
		fov = 90.0f;
		cameraPos = { -1.5, 0, -1.5};
		ogCamPos = cameraPos;
	}

	Rasterizer(std::string filename) {

		sourceFile = filename;
		windowName = L"Rasterizer";
		fov = 90.0f;
		cameraPos = { 0,0,-5};
		cameraDir = { 0,0,1 };
	}

	~Rasterizer() {
		delete[] zBuffer;
	}

private:

	//temp
	vec3f ogCamPos;

	float fov; // in degrees
	vec3f cameraPos; // camera position - not stationary
	vec3f cameraDir; // camera direction 
	mesh wholeMesh;

	float* zBuffer; // depth buffer 

	std::string sourceFile;

	float rotateObj;
	float cameraAngle;
	vec3f lightDir;


public:

	bool onUserCreate() override {

		if (sourceFile != "") {
			wholeMesh.parseObjFile(sourceFile);
		}
		else {
			// unit cube
			wholeMesh.triangles = {

				//SOUTH
				{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

				// EAST                                                      
				{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

				// NORTH                                                     
				{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

				// WEST                                                      
				{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

				// TOP                                                       
				{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

				// BOTTOM                                                    
				{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
				{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
			};
		}

		

		// define the depth buffer and set all of the depths to 0
		zBuffer = new float[screenHeight * screenWidth];
		for (int i = 0; i < screenHeight * screenWidth; i++)
			zBuffer[i] = 0.0;


		//making a backlight behind the player loooking down the positive z axis 
		lightDir = { 0.0, 0.0, 1.0 };
		lightDir = lightDir.normalizeVec(); 
		rotateObj = 0.0f; 

		return true;
	}

	bool onUserUpdate(float elapsedTime) override {

		vec3f target = { 0, 0, 0 };


		rectFill(0, 0, screenWidth, screenHeight, 0x2588, 0x0); 

		for (int i = 0; i < screenWidth * screenHeight; i++)
			zBuffer[i] = 100000000;

		rotateObj += 1.0f * elapsedTime; 
		//cameraAngle += 1.0f * elapsedTime;

		mat4x4 rotateX, rotateZ, rotate, rotateY;


		// rotation matrix around x axis
		rotateX = { 0 };

		rotateX.mat[0][0] = 1;
		rotateX.mat[1][1] = cosf(rotateObj);
		rotateX.mat[2][2] = rotateX.mat[1][1];
		rotateX.mat[1][2] = -sinf(rotateObj);
		rotateX.mat[2][1] = -rotateX.mat[1][2];
		rotateX.mat[3][3] = 1;


		rotateZ = { 0 };
		// rotation matrix around z axis
		rotateZ.mat[0][0] = cosf(rotateObj * 0.5f);
		rotateZ.mat[1][1] = rotateZ.mat[0][0];
		rotateZ.mat[0][1] = -sinf(rotateObj * 0.5);
		rotateZ.mat[1][0] = -1 * rotateZ.mat[0][1];
		rotateZ.mat[2][2] = 1;
		rotateZ.mat[3][3] = 1;


		//rotateY = { 0 };
		//rotateY.mat[0][0] = cosf(cameraAngle);
		//rotateY.mat[1][1] = 1;
		//rotateY.mat[2][2] = rotateY.mat[0][0];
		//rotateY.mat[0][2] = sinf(cameraAngle);
		//rotateY.mat[2][0] = -rotateY.mat[0][2];
		//rotateY.mat[3][3] = 1;

		//vec4f newCamPos = { ogCamPos.x, ogCamPos.y, ogCamPos.z, 1 };
		//cameraPos = (rotateY.rightMultiplyVec(newCamPos)).normalize();
		//cameraDir = target.subVec3f(cameraPos).normalizeVec();
		//lightDir = cameraDir; 



		// find the matrix together
		rotate = rotateX.rightMultiplyMatrix(rotateZ);

		// for each triangle in the cube's mesh
		for (tri triangle : wholeMesh.triangles) {


			tri rotated; 

			vec3f v1 = rotate.rightMultiplyVec({ triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, 1 }).normalize();
			vec3f v2 = rotate.rightMultiplyVec({ triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, 1 }).normalize();
			vec3f v3 = rotate.rightMultiplyVec({ triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, 1 }).normalize();

			rotated = {v1, v2, v3};


			fillTriangleZBuffer(rotated); 

			//drawTriangleZBuffer(imgSpace1.x, imgSpace1.y, imgSpace2.x, imgSpace2.y, imgSpace3.x, imgSpace3.y, 100, 0x2588, FG_RED); 
			//fillTriangleZBuffer(first, second, third); 
			//drawTriangle(imgSpace1.x, imgSpace1.y, imgSpace2.x, imgSpace2.y, imgSpace3.x, imgSpace3.y); 

			//std::cout << "{" << vec1.x << ", " << vec1.y << ", " << vec1.z << "}";
			//std::cout << "\t{" << vec2.x << ", " << vec2.y << ", " << vec2.z << "}";
			//std::cout << "\t{" << vec3.x << ", " << vec3.y << ", " << vec3.z << "}" << std::endl;
		}


		return true;
	}

private:

	// 
	void fillTriangleZBuffer(tri triangle) {

		// just the vectors 
		vec3f v1 = triangle.points[0]; 
		vec3f v2 = triangle.points[1];
		vec3f v3 = triangle.points[2];



		// lighting value
		float lightVal = getLighting(triangle); 

		//std::cout << lightVal << std::endl; 

		CHAR_INFO charVal = GetColorInfo(lightVal); 



		// projection matrix 
		mat4x4 projectMat = makePerspectiveMatrix();
		mat4x4 lookAt = makeLookAtMatrix();
		mat4x4 pointAt = makePointAtMatrix(); 

		// temp 4 vectors for projection 
		vec4f vec1 = { v1.x, v1.y, v1.z, 1 };
		vec4f vec2 = { v2.x, v2.y, v2.z, 1 };
		vec4f vec3 = { v3.x, v3.y, v3.z, 1 };

		// vectors in the camera sapce 
		vec4f camSpace1 = lookAt.rightMultiplyVec(vec1);
		vec4f camSpace2 = lookAt.rightMultiplyVec(vec2);
		vec4f camSpace3 = lookAt.rightMultiplyVec(vec3);

		// projected vectors
		vec3f projected1 = (projectMat.rightMultiplyVec(camSpace1)).normalize();
		vec3f projected2 = (projectMat.rightMultiplyVec(camSpace2)).normalize();
		vec3f projected3 = (projectMat.rightMultiplyVec(camSpace3)).normalize();


		// image / canvas space 
		vec3f imgSpace1 = projected1;
		vec3f imgSpace2 = projected2;
		vec3f imgSpace3 = projected3;


		// points need to be rescaled to the screen size; from [-1, 1] -> [0, screenWidth] and same for y
		imgSpace1.x = (imgSpace1.x + 1) * 0.5f * screenWidth;
		imgSpace2.x = (imgSpace2.x + 1) * 0.5f * screenWidth;
		imgSpace3.x = (imgSpace3.x + 1) * 0.5f * screenWidth;

		imgSpace1.y = (imgSpace1.y + 1) * 0.5f * screenHeight;
		imgSpace2.y = (imgSpace2.y + 1) * 0.5f * screenHeight;
		imgSpace3.y = (imgSpace3.y + 1) * 0.5f * screenHeight;

		float maxZ = max(camSpace3.z, max(camSpace2.z, camSpace1.z)); 


		vec3f vt1, vt2, vt3; 

		// find the three points sorted vertically 
		if (imgSpace1.y >= imgSpace2.y) {
		
			if (imgSpace2.y >= imgSpace3.y) {
				vt1 = imgSpace3;
				vt2 = imgSpace2;
				vt3 = imgSpace1; 
			}
			else if (imgSpace1.y >= imgSpace3.y) {
				vt1 = imgSpace2; 
				vt2 = imgSpace3;
				vt3 = imgSpace1; 
			}	 
			else {
				vt1 = imgSpace2;
				vt2 = imgSpace1;
				vt3 = imgSpace3; 
			}
		}
		else {

			if (imgSpace1.y >= imgSpace3.y) {
				vt1 = imgSpace3;
				vt2 = imgSpace1;
				vt3 = imgSpace2;
			}
			else if (imgSpace2.y >= imgSpace3.y) {
				vt1 = imgSpace1;
				vt2 = imgSpace3;
				vt3 = imgSpace2;
			}
			else {
				vt1 = imgSpace1;
				vt2 = imgSpace2;
				vt3 = imgSpace3;
			}
		}



		vec3f vt4 = {0};
		vt4.x = vt1.x + ((vt2.y - vt1.y) / (vt3.y - vt1.y)) * (vt3.x - vt1.x); 
		vt4.y = vt2.y; 


		
		if ((int)vt2.y == (int)vt3.y) {
			drawFlatBottomTriZBuffer(vt1.x, vt1.y, vt2.x, vt2.y, vt3.x, vt3.y, maxZ, charVal.Char.UnicodeChar, charVal.Attributes);
		}
		else if ((int)vt1.y == (int)vt2.y) {
			drawFlatTopTriZBuffer(vt1.x, vt1.y, vt2.x, vt2.y, vt3.x, vt3.y, maxZ, charVal.Char.UnicodeChar, charVal.Attributes);
		}
		else {
			drawFlatBottomTriZBuffer(vt1.x, vt1.y, vt2.x, vt2.y, vt4.x, vt4.y, maxZ, charVal.Char.UnicodeChar, charVal.Attributes);
			drawFlatTopTriZBuffer(vt2.x, vt2.y, vt4.x, vt4.y, vt3.x, vt3.y, maxZ, charVal.Char.UnicodeChar, charVal.Attributes);
		}
	}


	// returns the floating point dot product between the light direction and the triangle
	float getLighting(tri triangle) {

		vec3f normal = triangle.calcNormal().normalizeVec();
		return normal.dotProduct(lightDir);
	}


	// only draws the point if the distance to the point is closer 
	void drawPointZBuffer(int x, int y, float distance, short c = 0x2588, short col = 0x000F) {

		if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
			if (zBuffer[y * screenWidth + x] >= distance) {

				screenBuffer[y * screenWidth + x].Char.UnicodeChar = c;
				screenBuffer[y * screenWidth + x].Attributes = col;

				zBuffer[y * screenWidth + x] = distance;
			}
		}
	}


	void drawLineZBuffer(int x0, int y0, int x1, int y1, float distance, short c = 0x2588, short col = 0x000F) {
	
		int dx, dy, dxAbs, dyAbs, xIter, yIter, xEnd, yEnd, px, py; 

		dx = x1 - x0; 
		dy = y1 - y0; 
		dxAbs = abs(dx);
		dyAbs = abs(dy);
		px = 2 * dyAbs - dxAbs;
		py = 2 * dxAbs - dyAbs;


		if (dyAbs <= dxAbs) {

			// sets the correct start and end points
			if (dx >= 0) {
				xIter = x0; yIter = y0; xEnd = x1;
			}
			else {
				xIter = x1; yIter = y1; xEnd = x0;
			}

			drawPointZBuffer(xIter, yIter, distance, c, col);

			while (xIter < xEnd) {

				xIter++;

				if (px < 0) {
					px = px + 2 * dyAbs;
				}
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						yIter = yIter + 1;
					else
						yIter = yIter - 1;

					px = px + 2 * (dyAbs - dxAbs);
				}

				drawPointZBuffer(xIter, yIter, distance, c, col);
			}


		}
		else {

			if (dy >= 0)
			{
				xIter = x0; yIter = y0; yEnd = y1;
			}
			else
			{
				xIter = x1; yIter = y1; yEnd = y0;
			}

			drawPointZBuffer(xIter, yIter, distance, c, col);

			while (yIter < yEnd) {
				yIter++;
				if (py <= 0) {
					py = py + 2 * dxAbs;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						xIter = xIter + 1;
					else
						xIter = xIter - 1;
					py = py + 2 * (dxAbs - dyAbs);
				}

				drawPointZBuffer(xIter, yIter, distance, c, col);
			}

		}
	
	
	}

	void drawTriangleZBuffer(int x0, int y0, int x1, int y1, int x2, int y2, float distance, short c = 0x2588, short col = 0x000F) {

		drawLineZBuffer(x0, y0, x1, y1, distance, c, col);
		drawLineZBuffer(x0, y0, x2, y2, distance, c, col);
		drawLineZBuffer(x1, y1, x2, y2, distance, c, col);
	}


	// 
	void drawFlatTopTriZBuffer(int x0, int y0, int x1, int y1, int x2, int y2, float distance, short c = 0x2588, short col = 0x000F) {
		
		float invSlope1 = 0, invSlope2 = 0; 

		if (y2 - y0 != 0) 
			invSlope1 = (float)(x2 - x0) / (float)(y2 - y0);

		if (y2 - y1 != 0)
			invSlope2 = (float)(x2 - x1) / (float)(y2 - y1);

		float curx1 = x2; 
		float curx2 = x2; 

		for (int yVal = y2; yVal > y0; yVal--) {
			drawLineZBuffer((int)curx1, yVal, (int)curx2, yVal, distance, c, col); 	
			curx1 -= invSlope1; 
			curx2 -= invSlope2; 
		}
	}

	// 
	void drawFlatBottomTriZBuffer(int x0, int y0, int x1, int y1, int x2, int y2, float distance, short c = 0x2588, short col = 0x000F) {

		float invSlope1 = 0, invSlope2 = 0;

		if (y1 - y0 != 0)
			invSlope1 = (float)(x1 - x0) / (float)(y1 - y0);

		if (y2 - y0 != 0)
			invSlope2 = (float)(x2 - x0) / (float)(y2 - y0);

		float curx1 = x0;
		float curx2 = x0;

		for (int yVal = y0; yVal <= y1; yVal++) {
			drawLineZBuffer((int)curx1, yVal, (int)curx2, yVal, distance, c, col);
			curx1 += invSlope1;
			curx2 += invSlope2;
		}
	}

	// 
	void drawFlatTopTriZBuffer(tri &camSpace, tri &proj, short c = 0x2588, short col = 0x000F) {
		
		



	}




	// 
	void drawLineZBuffer(vec3f p0, vec3f p1, short c = 0x2588, short col = 0x00F) {

		int x0 = (int)p0.x; 
		int y0 = (int)p0.y; 

		int x1 = (int)p1.x; 
		int y1 = (int)p1.y; 

		float z0 = p0.z; 
		float z1 = p1.z; 

		float deltaZ = z1-z0; 

	
		int dx, dy, dxAbs, dyAbs, xIter, yIter, xEnd, yEnd, px, py;
		int xStart, yStart; 

		dx = x1 - x0;
		dy = y1 - y0;
		dxAbs = abs(dx);
		dyAbs = abs(dy);
		px = 2 * dyAbs - dxAbs;
		py = 2 * dxAbs - dyAbs;

		float len = sqrt(dx * dx + dy * dy); 
		float depth = z0; 

		// if the line is just a point
		if (len == 0) {
			drawPointZBuffer(x0, y0, min(z0,z1), c, col); 
			return; 
		}



		if (dyAbs <= dxAbs) {

			// sets the correct start and end points
			if (dx >= 0) {
				xIter = x0; yIter = y0; xEnd = x1; xStart = x0; yStart = y0; 
			}
			else {
				xIter = x1; yIter = y1; xEnd = x0; xStart = x1; yStart = y0; 
			}

			drawPointZBuffer(xIter, yIter, depth, c, col);

			while (xIter < xEnd) {

				xIter++;

				if (px < 0) {
					px = px + 2 * dyAbs;
				}
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						yIter = yIter + 1;
					else
						yIter = yIter - 1;

					px = px + 2 * (dyAbs - dxAbs);
				}

				depth = sqrt((xIter - xStart) * (xIter - xStart) + (yIter - yStart) * (yIter - yStart)) / len * deltaZ + z0; 

				drawPointZBuffer(xIter, yIter, depth, c, col);
			}


		}
		else {

			if (dy >= 0)
			{
				xIter = x0; yIter = y0; yEnd = y1; xStart = x0; yStart = y0; 
			}
			else
			{
				xIter = x1; yIter = y1; yEnd = y0; xStart = x1; yStart = y1; 
			}

			drawPointZBuffer(xIter, yIter, depth, c, col);

			while (yIter < yEnd) {
				yIter++;
				if (py <= 0) {
					py = py + 2 * dxAbs;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						xIter = xIter + 1;
					else
						xIter = xIter - 1;
					py = py + 2 * (dxAbs - dyAbs);
				}

				depth = sqrt((xIter - xStart) * (xIter - xStart) + (yIter - yStart) * (yIter - yStart)) / len * deltaZ + z0;

				drawPointZBuffer(xIter, yIter, depth, c, col);
			}

		}
	
	}

	

	// creates the matrix to look at the
	mat4x4 makeLookAtMatrix() {

		vec3f up = { 0.0, 1.0, 0.0 };

		// gets the camera's unit direction, right, and up
		vec3f camDir = cameraDir.normalizeVec();
		vec3f camRight = (up.crossProduct(camDir)).normalizeVec();
		vec3f camUp = camDir.crossProduct(camRight);

		// populate the first matrix 
		mat4x4 m1 = { 0 };
		m1.mat[0][0] = camRight.x;
		m1.mat[0][1] = camRight.y;
		m1.mat[0][2] = camRight.z;

		m1.mat[1][0] = camUp.x;
		m1.mat[1][1] = camUp.y;
		m1.mat[1][2] = camUp.z;

		m1.mat[2][0] = camDir.x;
		m1.mat[2][1] = camDir.y;
		m1.mat[2][2] = camDir.z;

		m1.mat[3][3] = 1.0;

		// populate the second matrix
		mat4x4 m2 = { 0 };
		m2.mat[0][0] = 1.0;
		m2.mat[1][1] = 1.0;
		m2.mat[2][2] = 1.0;
		m2.mat[3][3] = 1.0;

		m2.mat[0][3] = -cameraPos.x;
		m2.mat[1][3] = -cameraPos.y;
		m2.mat[2][3] = -cameraPos.z;

		// this idea is taken from: https://learnopengl.com/Getting-started/Camera


		return m1.rightMultiplyMatrix(m2);
	}

	mat4x4 makePointAtMatrix() {
	
		mat4x4 ret = {0};

		vec3f up = { 0.0, 1.0, 0.0 };

		// gets the camera's unit direction, right, and up
		vec3f camDir = cameraDir.normalizeVec();
		vec3f camRight = (up.crossProduct(camDir)).normalizeVec();
		vec3f camUp = camDir.crossProduct(camRight);

		ret.mat[0][0] = camRight.x; 
		ret.mat[1][0] = camRight.y;
		ret.mat[2][0] = camRight.z;

		ret.mat[0][1] = camUp.x;
		ret.mat[1][1] = camUp.y;
		ret.mat[2][1] = camUp.z;

		ret.mat[0][2] = camDir.x;
		ret.mat[1][2] = camDir.y;
		ret.mat[2][2] = camDir.z;


		ret.mat[0][3] = cameraPos.x; 
		ret.mat[1][3] = cameraPos.y;
		ret.mat[2][3] = cameraPos.z;

		ret.mat[3][3] = 1; 

		return ret; 
	}














	
	// creates the perspective matrix based on the fov and aspect ratio
	mat4x4 makePerspectiveMatrix() {


		float a = (float)screenHeight / (float)screenWidth; 
		float r = tanf(fov * M_PI / 180.0f / 2.0f); 

		float zFar = 1000.0f; 
		float zNear = 0.1f; 



		mat4x4 ret = { 0 };




		ret.mat[0][0] = a / r; 
		ret.mat[1][1] = 1.0f / r; 
		ret.mat[2][2] = -1 * (zFar + zNear) / (zFar - zNear); 
		ret.mat[2][3] = (zFar * zNear * 2) / (zNear - zFar);  
		ret.mat[3][2] = -1; 

		return ret; 
	}




	CHAR_INFO GetColorInfo(float luminosity) {

		short bg, fg;
		wchar_t symbol;

		int pixel_bw = (int)(13.0f * luminosity) % 13;

		switch (pixel_bw) {

		case 0: bg = BG_BLACK; fg = FG_BLACK; symbol = PIXEL_SOLID; break;

		case 1: bg = BG_BLACK; fg = FG_DARK_GREY; symbol = PIXEL_QUARTER; break;

		case 2: bg = BG_BLACK; fg = FG_DARK_GREY; symbol = PIXEL_HALF; break;

		case 3: bg = BG_BLACK; fg = FG_DARK_GREY; symbol = PIXEL_THREEFOURTHS; break;

		case 4: bg = BG_BLACK; fg = FG_DARK_GREY; symbol = PIXEL_SOLID; break;


		case 5: bg = BG_DARK_GREY; fg = FG_GREY; symbol = PIXEL_QUARTER; break;

		case 6: bg = BG_DARK_GREY; fg = FG_GREY; symbol = PIXEL_HALF; break;

		case 7: bg = BG_DARK_GREY; fg = FG_GREY; symbol = PIXEL_THREEFOURTHS; break;

		case 8: bg = BG_DARK_GREY; fg = FG_GREY; symbol = PIXEL_SOLID; break;

		case 9:  bg = BG_GREY; fg = FG_WHITE; symbol = PIXEL_QUARTER; break;

		case 10: bg = BG_GREY; fg = FG_WHITE; symbol = PIXEL_HALF; break;

		case 11: bg = BG_GREY; fg = FG_WHITE; symbol = PIXEL_THREEFOURTHS; break;

		case 12: bg = BG_GREY; fg = FG_WHITE; symbol = PIXEL_SOLID; break;

		default: bg = BG_BLACK; fg = FG_BLACK; symbol = PIXEL_SOLID;

		}

		CHAR_INFO c;

		c.Attributes = bg | fg;
		c.Char.UnicodeChar = symbol;

		return c;

	}



};





