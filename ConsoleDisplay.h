#pragma once

#include <windows.h>
#include <windowsx.h>

#ifdef __cplusplus
extern "C"
{
#endif



	typedef struct DisplaySettings DisplaySettings;

#define ENCODE_DISPLAY_CHARACTER(fg,bg,character,charflag) {\
 ((unsigned short)((((unsigned short)fg&0xFF0)>>4)|(((unsigned short)bg&0xFF0)<<4)))\
,(unsigned short)character\
,((unsigned char)(((unsigned short)fg&0xF)|(((unsigned short)bg&0xF)<<4)))\
,(unsigned char)charflag}

	// encode extended colors ( colors + background / foreground... flags )
	typedef struct DisplayCharacter
	{
		unsigned short	mFlags;				// packed fg and bg flags
		unsigned short	mCharacter;			// unicode character
		unsigned char	mColors;			// packed fg and bg colors
		unsigned char	mCharactersFlag;	// flags for characters
	}DisplayCharacter;

	typedef enum CharacterFlags
	{
		NO_FLAG = 0,
		PRINT_IF_EMPTY = 1,			// print only if previous character is ' ' or NO_CHARACTER 
		NO_CHARACTER = 2			// Use previous character
	}CharacterFlags;

	// list of console colors + flags
	typedef enum ConsoleColors
	{
		BLACK = 0,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		WHITE,
		BRIGHT_BLACK,
		BRIGHT_RED,
		BRIGHT_GREEN,
		BRIGHT_YELLOW,
		BRIGHT_BLUE,
		BRIGHT_MAGENTA,
		BRIGHT_CYAN,
		BRIGHT_WHITE,			//15
		BACKGROUND = 16,		// use previous background 
		FOREGROUND = 32,		// use previous foreground
		OR_COLOR = 64,		// Logical OR ( must have BACKGROUND or FOREGROUND set and a color ) 
		AND_COLOR = 128,		// Logical AND ( must have BACKGROUND or FOREGROUND set  and a color ) 
		XOR_COLOR = 64 | 128,	// Logical exclusive OR ( must have BACKGROUND or FOREGROUND set  and a color ) 
		DARKER = 256,		// take resulting color and make it darker
		CLEARER = 512,		// take resulting color and make it clearer
		NO_BLEND = 1024,		// just copy input to output		
	}ConsoleColors;

	void	CodeColors(DisplayCharacter* color, ConsoleColors FG, ConsoleColors BG);

	// manage a "bitmap" or a "Window" 
	// where display is done. DisplayZone have to be flushed on display at the end with FlushDisplayZone
	typedef struct DisplayZone
	{
		int					mPosX;
		int					mPosY;
		unsigned int		mSizeX;
		unsigned int		mSizeY;
		unsigned int		mSquaredPixels;

		DisplayCharacter* mBuffer;

	}DisplayZone;

	// create a DisplaySettings structure and initialize display with given size ( in characters )
	DisplaySettings* InitDisplay(unsigned int sx, unsigned int sy, unsigned int newFontW, unsigned int squaredRasterFont);
	// free all memory used by DisplaySettings structure and free structure ( display should be set to NULL after calling this function )
	void				CloseDisplay(DisplaySettings* display);

	// init a display zone at given coordinates, with given size
	// if last parameter squaredPixels is not 0, then caracters are duplicated to give squared pixels ( as font is 8x16 )
	// so a squaredpixels zone of size 50x50 will be flushed on a zone of 100x50 in the console.
	void	InitDisplayZone(DisplayZone* zone, int px, int py, unsigned int sx, unsigned int sy, unsigned int squaredPixels);
	// close (un init) the display zone
	void	CloseDisplayZone(DisplayZone* zone);
	// print given string at given pos with given caracteristics in display zone
	void	PrintInDisplayZone(DisplayZone* zone, ConsoleColors FG, ConsoleColors BG, int px, int py, const char* str, int gotonextline, unsigned char characterFlag);
	// print given DisplayCharacter at given pos in display zone
	void	PrintDisplayCharacterInDisplayZone(DisplayZone* zone, DisplayCharacter c, int px, int py);
	// copy src zone at position px,py in dest zone
	void	CopyDisplayZone(DisplayZone* dest, DisplayZone* src, int px, int py);
	// copy part of src zone at position px, py in dest zone
	// px, py => position in destination
	// srcpx, srcpy => bloc to copy position in src
	// srcsx, srcsy => bloc size to copy 
	void	CopyDisplayZonePart(DisplayZone* dest, DisplayZone* src, int px, int py, int srcpx, int srcpy, int srcsx, int srcsy);

	// set zone pos ( upper left corner )
	void	MoveDisplayZone(DisplayZone* zone, int newposx, int newposy);
	// clear the whole display zone with given foreground and background colors ( with flags )
	void	ClearDisplayZone(DisplayZone* zone, ConsoleColors FG, ConsoleColors BG, unsigned short character, unsigned char characterFlag);
	// copy the display zone in the display buffer
	void	FlushDisplayZone(DisplaySettings* settings, DisplayZone* zone);

	// show the current display buffer
	void	SwapBuffer(DisplaySettings* settings);
	// clear the current display buffer
	void	ClearBuffer(DisplaySettings* settings, ConsoleColors FG, ConsoleColors BG);

#ifdef __cplusplus
}// extern C
#endif

