#pragma once 

#include <Windows.h> 
#include <string> 
#include <thread>
#include <iostream>


class ConsoleEngine {

public:
	ConsoleEngine() {

		screenWidth = 80;
		screenHeight = 80;
		consoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		consoleIn = GetStdHandle(STD_INPUT_HANDLE);

		consoleActive = false;

		windowName = L"Default Name";
	}

	~ConsoleEngine() {
		delete[] screenBuffer; 
	}

	// MUST OVERRIDE
	virtual bool onUserCreate() {
		return false;
	}

	virtual bool onUserUpdate(float elapsedTime){
		return false;
	}



	int createConsole(int width, int height, int fontw, int fonth) {

		if (consoleOut == INVALID_HANDLE_VALUE)
			return onError(L"ConsoleOut");
		

		screenWidth = width; 
		screenHeight = height; 

		rectWindow = {0, 0, 1, 1}; 
		SetConsoleWindowInfo(consoleOut, TRUE, &rectWindow); 


		// Set the size of the screen buffer
		COORD coord = { (short)screenWidth, (short)screenHeight }; 
		if (!SetConsoleScreenBufferSize(consoleOut, coord))
			return onError(L"ConsoleBufferSize");

		

		// Assign screen buffer to the console
		if (!SetConsoleActiveScreenBuffer(consoleOut))
			return onError(L"ActiveBu");

		//CONSOLE_FONT_INFOEX cur; 

		// Set the font size now that the screen buffer has been assigned to the console
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = (short)fontw;
		cfi.dwFontSize.Y = (short)fonth;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		

		wcscpy_s(cfi.FaceName, L"Consolas");
		if (!SetCurrentConsoleFontEx(consoleOut, FALSE, &cfi))
			return onError(L"SetCurrentConsoleFontEx");

		//COORD max = GetLargestConsoleWindowSize(consoleOut);

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(consoleOut, &csbi))
			return onError(L"GetConsoleScreenBufferInfo");

		if (screenHeight > csbi.dwMaximumWindowSize.Y)
			return onError(L"Screen Height / Font Height Too Big");
		if (screenWidth > csbi.dwMaximumWindowSize.X)
			return onError(L"Screen Width / Font Width Too Big");


		rectWindow = { 0, 0, (short)screenWidth - 1, (short)screenHeight - 1};
		if (!SetConsoleWindowInfo(consoleOut, TRUE, &rectWindow))
			return onError(L"SetConsoleWindowInfo"); 

		screenBuffer = new CHAR_INFO[screenWidth * screenHeight]; 
		memset(screenBuffer, 0, sizeof(CHAR_INFO) * screenWidth * screenHeight); 

		return 1; 
	}

	// sets the desired location in the buffer to be unicode character and with specific color
	void drawPoint(int x, int y, short c = 0x2588, short col = 0x000F) {
		if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
			screenBuffer[y * screenWidth + x].Char.UnicodeChar = c;
			screenBuffer[y * screenWidth + x].Attributes = col;
		}
	}

	
	void drawLine(int x0, int y0, int x1, int y1, short c = 0x2588, short col = 0x000F) {
	
		int dx, dy, dxAbs, dyAbs, xIter, yIter, xEnd, yEnd, px, py;
		dx = x1 - x0; 
		dy = y1 - y0; 
		dxAbs = abs(dx); 
		dyAbs = abs(dy); 
		px = 2 * dyAbs - dxAbs; 
		py = 2 * dxAbs - dyAbs; 

		/*	IF:		0 <= m <= 1
					That is, the line is below y = x 
					Use x as forced step. Then compute if you keep y the same or add one to it 
			ELSE: 
					
		*/
		if (dyAbs <= dxAbs) {
			
			// sets the correct start and end points
			if (dx >= 0) {
				xIter = x0; yIter = y0; xEnd = x1; 
			}
			else {
				xIter = x1; yIter = y1; xEnd = x0;
			}

			drawPoint(xIter, yIter, c, col); 
			
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

				drawPoint(xIter, yIter, c, col); 
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

			drawPoint(xIter, yIter, c, col);

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

				drawPoint(xIter, yIter, c, col);
			}

		}
	}

	// draw a hollow triangle - that is just draw 3 connected lines
	void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F) {
		
		drawLine(x0, y0, x1, y1, c, col);
		drawLine(x1, y1, x2, y2, c, col);
		drawLine(x2, y2, x0, y0, c, col);
	}
	

	// fills a rectangular region with one color
	void rectFill(int x0, int y0, int x1, int y1, short c = 0x2588, short col = 0x000F) {
	
		for (int x = x0; x < x1; x++) {
			for (int y = y0; y < y1; y++) {
				drawPoint(x, y, c, col); 
			}
		}
	}


private:
	int onError(const std::wstring text) {
		std::wcout << text << std::endl; 
		return -1; 
	}


	void gameThread() {
		
		// calls on user create once
		if (!onUserCreate())
			consoleActive = false;

		

		auto t1 = std::chrono::system_clock::now();
		auto t2 = std::chrono::system_clock::now();

		consoleActive = true; 

		while (consoleActive) {
			t1 = std::chrono::system_clock::now();
			std::chrono::duration<float> eTime = t1 - t2; 
			t2 = t1; 

			float time = eTime.count(); 

			if (!onUserUpdate(time))
				consoleActive = false; 

			wchar_t s[256]; 
			swprintf_s(s, 256, L"Console Engine - %s - FPS: %.2f", windowName.c_str(), 1.0f / time); 
			SetConsoleTitle(s); 
			WriteConsoleOutput(consoleOut, screenBuffer, { (short)screenWidth, (short)(screenHeight) }, { 0,0 }, &rectWindow);
		}

	}


public: 

	// start the console and run the game loop with a new thread
	void startConsole() {

		std::thread t(&ConsoleEngine::gameThread, this);
		t.join();
	}


protected: 

	// Vars for view - can just output without any input and everything will be good! 
	int screenWidth; 
	int screenHeight; 

	HANDLE consoleOut; 
	HANDLE consoleIn;

	CHAR_INFO* screenBuffer; 

	SMALL_RECT rectWindow; 
	bool consoleActive; 

	/* input - add later for movement and stuff */


public:
	std::wstring windowName;

};