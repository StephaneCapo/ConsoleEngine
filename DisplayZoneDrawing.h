#pragma once
#include "ConsoleDisplay.h"
#include "bitmap.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct rgbVect
	{
		int R;
		int G;
		int B;

	}rgbVect;

	// some utility functions to draw geometry in Display Zone

	// draw a filled rectangle with given pos, size, colors and character
	void	DrawRectangleInDisplayZone(DisplayZone* zone, int px, int py, int sx, int sy, ConsoleColors FG, ConsoleColors BG, char character);

	// draw a filled rectangle with given pos, size, border and fill colors and character
	void	DrawRectangleInDisplayZoneEx(DisplayZone* zone, int px, int py, int sx, int sy, ConsoleColors fillFG, ConsoleColors fillBG, char fillCharacter, ConsoleColors borderFG, ConsoleColors borderBG, char borderCharacter);

	// draw a filled rectangle with given pos, size, border and fill DisplayCharacter
	void	DrawRectangleInDisplayZoneDisplayCharacter(DisplayZone* zone, int px, int py, int sx, int sy, DisplayCharacter* fill, DisplayCharacter* border);


	// draw disc with given center, radius, colors and character
	void	DrawDiscInDisplayZone(DisplayZone* zone, int cx, int cy, int r, ConsoleColors FG, ConsoleColors BG, char character);

	// draw disc with given center, radius, border and fill colors and character
	void	DrawDiscInDisplayZoneEx(DisplayZone* zone, int cx, int cy, int r, ConsoleColors fillFG, ConsoleColors fillBG, char fillCharacter, ConsoleColors borderFG, ConsoleColors borderBG, char borderCharacter);

	void	DrawDiscInDisplayZoneDisplayCharacter(DisplayZone* zone, int cx, int cy, int r, DisplayCharacter* fill, DisplayCharacter* border);

	// convert RGB values to DisplayCharacter
	DisplayCharacter	GetAsciiArtFromRGB(unsigned char R, unsigned char G, unsigned char B);

	// convert RGB values to DisplayCharacter and return "error"
	DisplayCharacter	GetAsciiArtFromRGBExt(unsigned char R, unsigned char G, unsigned char B,rgbVect* error);

	DisplayCharacter	GetDisplayCharacterFrom4Pixels(RGBPixel* FourPixels);

	void	convertImageWithDithering(BITMAPINFO* BitmapInfo, unsigned char* bmpdata, DisplayZone* zone);

	void	convertBufferWithDithering(RGBPixel* buffer, int imgSizeX, int imgSizeY, DisplayZone* zone);

	DisplayZone* CreateDisplayZoneFromBMP(const char* bmpname);

#ifdef __cplusplus
} // extern C
#endif