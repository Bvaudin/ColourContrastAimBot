// SmartAimBot.cpp : Defines the entry point for the application.

//#include "scan.h"
#include "iostream"
#include "Windows.h"

using namespace std;

POINT a, b;
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
int width = 160;
int height = 160;

RGBQUAD * capture(POINT a, POINT b) {
	// copy screen to bitmap
	HDC     hScreen = GetDC(NULL);
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, abs(b.x - a.x), abs(b.y - a.y));
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, abs(b.x - a.x), abs(b.y - a.y), hScreen, a.x, a.y, SRCCOPY); // BitBlt does the copying

	/*
	// save bitmap to clipboard
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();
	*/

	// Array conversion:
	RGBQUAD* pixels = new RGBQUAD[width * height];

	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biWidth = width;
	bmi.biHeight = -height;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;// 3 * ScreenX * ScreenY;
	
	GetDIBits(hDC, hBitmap, 0, height, pixels, (BITMAPINFO*)& bmi, DIB_RGB_COLORS);

	// clean up
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	DeleteObject(hBitmap);
	return pixels;
}

void Aim() {
	RGBQUAD * pixels;
	POINT targetPos; // centered at top left corner of capture zone
	
	// bright colour thermals
	// int sampleR = 255, sampleG = 170, sampleB = 80; // nc infravision
	// int sampleR = 255, sampleG = 145, sampleB = 57; // vs infravision
	// int sampleR = 180, sampleG = 160, sampleB = 130; // corona ir values
	// int sampleR = 145, sampleG = 145, sampleB = 145; // tr irnv values

	// int targetR = 250, targetG = 250, targetB = 250; // bf4 2x irnv

	int targetR = 180, targetG = 186, targetB = 65; // r6 glaz green highlight

	int sampleCount = 16;
	double radius, angle;
	int x, y, index; // 0 indexed from top left
	int red, green, blue;

	bool targetAcquired = false;
	//bool evadeCrosshairColour = false;
	// int brightest = 0;
	while (true) {
		angle = 2 * 3.141592654 / 8;
		radius = 2;
		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0 && !(GetKeyState(VK_SHIFT) & 0x8000)) { // while rmb pressed, shift not pressed
			pixels = capture(a, b);
			targetAcquired = false;
			//evadeCrosshairColour = false;
			for (int i = 0; i < sampleCount * width; i++) { // need to evade 6x6 grid in center of screen, accept 55x, 44y
				x = (int)(radius * cos(angle) + width / 2);
				y = (int)(radius * sin(angle) + height / 2);
				if (i % sampleCount == 0) { // if ring is complete
					radius++; // increment radius
				}
				angle += 2 * 3.141592654 / sampleCount; // increment angle per iteration
				index = y * width + x; // get 1d array index
				if (x < 0 || x > 399 || y < 0 || y > 399) { // boundary check
					break;
				}
				//if (radius > 45 && (abs(width / 2 - x) <= 2 || abs(height / 2 - y) <= 2)) { 
				//	evadeCrosshairColour = true;
				//}
				red = (int)pixels[index].rgbRed;
				green = (int)pixels[index].rgbGreen;
				blue = (int)pixels[index].rgbBlue;

				if ((abs(red - targetR) < 30 && abs(green - targetG) < 30 && abs(blue - targetB) < 30) ) {// if within target colour range
					// brightest = red + green + blue;
					targetPos.x = index % width;
					targetPos.y = index / width;
					targetAcquired = true;
				}

				//cout << angle / 2 / 3.1415 << " " << radius << endl;
				//cout << red << " " << green << " " << blue << endl;
				//cout << abs(red - sampleR) << " " << abs(green - sampleG) << " " << abs(blue - sampleB) << endl;
				//cout << " " << endl;

				if (i % sampleCount == 0 && targetAcquired) { // if ring is complete and targetAcquired
					mouse_event(MOUSEEVENTF_MOVE, (targetPos.x - width / 2)/2, (targetPos.y - height / 2)/2, 0, 0); // x and y are deltas, not abs coordinates
					break;
				}
			}
			delete[] pixels;
		}
		Sleep(1);
	}

/*
	while (true) {
		angle = 2 * 3.141592654 / 8;
		radius = 2;
		// if ((GetKeyState(0x39) & 0x100) != 0) { // while key 9 pressed
			// sample colors
			// cout << "reached" << endl;
		// }
		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0 && !(GetKeyState(VK_SHIFT) & 0x8000)) { // while rmb pressed
			pixels = capture(a, b);

			for (int i = 0; i < sampleCount * width; i++) {


				if (i % 16 == 0) { // found outline
					mouse_event(MOUSEEVENTF_MOVE, (targetPos.x - width / 2), (targetPos.y - height / 2), 0, 0); // x and y are deltas, not abs coordinates
					//cout << "BREAK" << endl;
					break;
				}
			}
			delete[] pixels;
		}
		Sleep(1);
	}
*/
}
void updateResolution() {
	while (1) {
		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);
		a.x = screenWidth / 2 - width / 2;
		a.y = screenHeight / 2 - height / 2;
		b.x = screenWidth / 2 + width / 2;
		b.y = screenHeight / 2 + height / 2;
		Sleep(10000);
	}
}

int main() {
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) updateResolution, 0, 0, 0);
	Aim();
	return 0;
}
