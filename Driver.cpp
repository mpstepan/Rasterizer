
#include <iostream> 
#include "Rasterizer.h"









int main() {
	Rasterizer demo("teapot.obj");

	if (demo.createConsole(800, 450, 2, 2) != -1)
		demo.startConsole(); 

	return 0;
}