#include "ConsoleDisplay.h"
#include <stdio.h>
#include <WinCon.h>
#include "TimeManagement.h"

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004

// global data for console display management
struct DisplaySettings
{
	HANDLE						mConsoleHandle;
	HWND						mConsoleHWND;
	unsigned int				mSizeX;
	unsigned int				mSizeY;
	unsigned int				mSquaredFont;
	CONSOLE_CURSOR_INFO			mBackupCursorState;
	struct DisplayCharacter*	mDrawBuffer;
	unsigned int				mDisplayFPS;
};

const int OLD_CONSOLE_COLORS[16] = { 0 // BLACK
									,4 //RED
									,2 //GREEN
									,6 //YELLOW
									,1 //BLUE
									,5//MAGENTA
									,3//CYAN
									,7//WHITE
									,8//BRIGHT_BLACK
									,12//BRIGHT_RED
									,10//BRIGHT_GREEN
									,14//BRIGHT_YELLOW
									,9//BRIGHT_BLUE
									,13//BRIGHT_MAGENTA
									,11//BRIGHT_CYAN
									,15//BRIGHT_WHITE
									};

void	SetColor(DisplaySettings* settings,unsigned char FG, unsigned char BG)
{
	FG = OLD_CONSOLE_COLORS[FG];
	BG = OLD_CONSOLE_COLORS[BG];
	SetConsoleTextAttribute(settings->mConsoleHandle, (FG | (BG << 4)));
}

void HideCursor(DisplaySettings* settings)  // Hide the cursor 
{
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(settings->mConsoleHandle, &cursor_info);
}

const int DARKERCOLORS[16] = {	BLACK,
								BLACK,
								BLACK,
								BLACK,
								BLACK,
								RED,
								BLUE,
								BRIGHT_BLACK,
								BLACK,
								RED,
								GREEN,
								YELLOW,
								BLUE,
								MAGENTA,
								CYAN,
								WHITE
};

const int CLEARERCOLORS[16] = { BRIGHT_BLACK,
								BRIGHT_RED,
								BRIGHT_GREEN,
								BRIGHT_YELLOW,
								BRIGHT_BLUE,
								BRIGHT_MAGENTA,
								BRIGHT_CYAN,
								BRIGHT_WHITE,
								WHITE,
								BRIGHT_RED,
								BRIGHT_CYAN,
								BRIGHT_WHITE,
								BRIGHT_CYAN,
								BRIGHT_RED,
								BRIGHT_YELLOW,
								BRIGHT_WHITE
};

int MIXED_COLORS[256];

void EnableVirtualTerminal(DisplaySettings* display)
{
	DWORD consoleMode;
	GetConsoleMode(display->mConsoleHandle, &consoleMode);
	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(display->mConsoleHandle, consoleMode);
}

unsigned int getRows(DisplaySettings* display)
{
	CONSOLE_SCREEN_BUFFER_INFO	info;
	GetConsoleScreenBufferInfo(display->mConsoleHandle, &info);
	return info.dwSize.Y;
}

unsigned int getCols(DisplaySettings* display)
{
	CONSOLE_SCREEN_BUFFER_INFO	info;
	GetConsoleScreenBufferInfo(display->mConsoleHandle, &info);
	return info.dwSize.X;
}

void	CodeColors(DisplayCharacter* color,ConsoleColors FG, ConsoleColors BG)
{
	color->mFlags = ((unsigned short)((((unsigned short)FG & 0xFF0) >> 4) | (((unsigned short)BG & 0xFF0) << 4)));
	color->mColors = ((unsigned char)(((unsigned short)FG & 0xF) | (((unsigned short)BG & 0xF) << 4)));
}


inline void			DecodeColors(DisplayCharacter* color, unsigned int* FG, unsigned int* BG)
{
	*FG = ((unsigned short)color->mColors & 0xF)|((color->mFlags&0xFF)<<4);
	*BG = (((unsigned short)color->mColors & 0xF0)>>4) | ((color->mFlags & 0xFF00) >> 4); 
}

void	ManageColorFlags(unsigned int sourceColor, unsigned int* finalColor, unsigned int DFG, unsigned int DBG)
{
	unsigned int RGBSourceColor = sourceColor & 0xF;
	unsigned int* secondColor = &RGBSourceColor;
	*finalColor = sourceColor;

	if (sourceColor & NO_BLEND)
	{
		*finalColor = sourceColor- NO_BLEND;
		return;
	}

	if (sourceColor & BACKGROUND)
	{
		if (DBG > 15) // background already has flags, just or DBG and current color, can be undefined behavior
		{
			*finalColor = DBG | sourceColor;
			return;
		}
		secondColor = &DBG;
		*finalColor = DBG;
	}
	else if (sourceColor & FOREGROUND)
	{
		if (DFG > 15) // foreground already has flags, just or DFG and current color, can be undefined behavior
		{
			*finalColor = DFG | sourceColor;
			return;
		}
		secondColor = &DFG;
		*finalColor = DFG;
	}

	if ((sourceColor & XOR_COLOR)== OR_COLOR)
	{
		*finalColor = (*secondColor) | (sourceColor & 0xF);
	}
	else if ((sourceColor & XOR_COLOR)==AND_COLOR)
	{
		*finalColor = (*secondColor) & (sourceColor & 0xF);
	}
	else if (sourceColor & XOR_COLOR)
	{
		*finalColor = (*secondColor) ^ (sourceColor & 0xF);
	}

	if (sourceColor & MIX)
	{
		*finalColor = MIXED_COLORS[(((*secondColor)&0xF)<<4) | (sourceColor & 0xF)];
	}

	if (sourceColor & DARKER)
	{
		if (((sourceColor & HALF) && ((*finalColor) & 15) >= 8) || !(sourceColor & HALF))
		{
			*finalColor = DARKERCOLORS[(*finalColor) & 15];
		}
	}
	if (sourceColor & CLEARER)
	{
		if (((sourceColor & HALF) && ((*finalColor) & 15) < 8) || !(sourceColor & HALF))
		{
			*finalColor = CLEARERCOLORS[(*finalColor) & 15];
		}
	}
}

void	Blender(DisplayCharacter* src, DisplayCharacter* dst, DisplayCharacter* blender)
{
	unsigned int SFG, SBG;
	DecodeColors(src, &SFG, &SBG);
	unsigned int DFG, DBG;
	DecodeColors(dst, &DFG, &DBG);

	unsigned int finalFG = SFG;
	unsigned int finalBG = SBG;

	// fast check if special case is needed
	if (SFG > 15)
	{
		ManageColorFlags(SFG, &finalFG, DFG, DBG);
	}
	if (SBG > 15)
	{
		ManageColorFlags(SBG, &finalBG, DFG, DBG);
	}

	if (src->mCharactersFlag & NO_CHARACTER) // just leave previous character
	{
		blender->mCharacter = dst->mCharacter;
	}
	else if(src->mCharactersFlag & PRINT_IF_EMPTY)
	{
		blender->mCharacter = ((dst->mCharacter==' ')||(dst->mCharactersFlag&NO_CHARACTER))?src->mCharacter: dst->mCharacter;
	}
	else
	{
		blender->mCharacter = src->mCharacter;
	}
	CodeColors(blender,finalFG, finalBG);
	blender->mCharactersFlag = src->mCharactersFlag;
}

void	changeDisplaySize(DisplaySettings* displaysettings, unsigned int sx, unsigned int sy)
{
	displaysettings->mSizeX = sx;
	displaysettings->mSizeY = sy;

	COORD currentSize;
	currentSize.X = getCols(displaysettings);
	currentSize.Y = getRows(displaysettings);
	COORD newSize = { sx + 1,sy + 1 };
	SMALL_RECT rect = { 0, 0, sx , sy };

	if (currentSize.Y <= newSize.Y)
	{
		if (currentSize.X <= newSize.X)
		{
			SetConsoleScreenBufferSize(displaysettings->mConsoleHandle, newSize);
			SetConsoleWindowInfo(displaysettings->mConsoleHandle, TRUE, &rect);
		}
		else
		{
			SMALL_RECT tmp = { 0, 0, sx, currentSize.Y - 1 };
			SetConsoleWindowInfo(displaysettings->mConsoleHandle, TRUE, &tmp);
			SetConsoleScreenBufferSize(displaysettings->mConsoleHandle, newSize);
			SetConsoleWindowInfo(displaysettings->mConsoleHandle, TRUE, &rect);
		}
	}
	else
	{
		if (currentSize.X <= newSize.X)
		{
			SMALL_RECT tmp = { 0, 0, currentSize.X - 1, sy };
			SetConsoleWindowInfo(displaysettings->mConsoleHandle, TRUE, &tmp);
			SetConsoleScreenBufferSize(displaysettings->mConsoleHandle, newSize);
			SetConsoleWindowInfo(displaysettings->mConsoleHandle, TRUE, &rect);
		}
		else
		{
			SetConsoleWindowInfo(displaysettings->mConsoleHandle, TRUE, &rect);
			SetConsoleScreenBufferSize(displaysettings->mConsoleHandle, newSize);
		}
	}
	CONSOLE_FONT_INFOEX currentFontInfo;
	currentFontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(displaysettings->mConsoleHandle, FALSE, &currentFontInfo);

	// hard coded windows pixel size ( with borders ), given chararcters row / columns 
	// this is really not a good way to do it 
	MoveWindow(displaysettings->mConsoleHWND, 0, 0, (sx + 1) * currentFontInfo.dwFontSize.X + 16, (sy + 1) * currentFontInfo.dwFontSize.Y + 48, TRUE);

}

extern void initRGBConsoleColor();


DisplaySettings* InitDisplay(unsigned int sx, unsigned int sy,unsigned int newFontW,unsigned int squaredRasterFont)
{
	initRGBConsoleColor();

	for (int i = 0; i < 256; i++)
	{
		MIXED_COLORS[i]= MixColors((i & 0xF0)>>4,(i&0xF));
	}


	DisplaySettings* result =(DisplaySettings*) malloc(sizeof(DisplaySettings));
	result->mDisplayFPS = 0;
	result->mConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	result->mConsoleHWND = GetConsoleWindow();
	result->mSquaredFont = 0;
	if (newFontW < 8)
	{
		newFontW = 8;
	}

	CONSOLE_FONT_INFOEX currentFontInfo;
	currentFontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(result->mConsoleHandle, FALSE, &currentFontInfo);
	if (squaredRasterFont)
	{
		currentFontInfo.dwFontSize.X = newFontW;
		currentFontInfo.dwFontSize.Y = newFontW;
		currentFontInfo.FontFamily = FF_DONTCARE| RASTER_FONTTYPE;
		currentFontInfo.FontWeight = FW_NORMAL;
		wcscpy_s(currentFontInfo.FaceName,32, L"Terminal");
		result->mSquaredFont = 1;
	}
	else
	{
		currentFontInfo.dwFontSize.X = newFontW;
		currentFontInfo.dwFontSize.Y = newFontW * 2;
		currentFontInfo.FontFamily = FF_DONTCARE;
		currentFontInfo.FontWeight = FW_NORMAL;
		wcscpy_s(currentFontInfo.FaceName,32, L"Consolas");
	}
	
	SetCurrentConsoleFontEx(result->mConsoleHandle, FALSE, &currentFontInfo);

	changeDisplaySize(result, sx, sy);

	EnableVirtualTerminal(result);

	// init draw buffer
	result->mDrawBuffer = malloc(sx * sy * sizeof(DisplayCharacter));
	
	// black screen
	SetColor(result, 0, 0);

	ClearBuffer(result, BLACK, BLACK);
	
	GetConsoleCursorInfo(result->mConsoleHandle, &result->mBackupCursorState);
	HideCursor(result);

	return result;
}


void	CloseDisplay(DisplaySettings* display)
{
	SetConsoleCursorInfo(display->mConsoleHandle, &display->mBackupCursorState);

	SetColor(display, WHITE, BLACK);

	free(display->mDrawBuffer);

	free(display);
}

void	InitDisplayZone(DisplayZone* zone, int px, int py, unsigned int sx, unsigned int sy, unsigned int squaredPixels)
{
	zone->mBuffer = malloc(sizeof(DisplayCharacter) * sx * sy);
	zone->mPosX = px;
	zone->mPosY = py;
	zone->mSizeX = sx;
	zone->mSizeY = sy;
	zone->mSquaredPixels = squaredPixels;

	ClearDisplayZone(zone, FOREGROUND, BACKGROUND,' ',NO_FLAG);
}
void	CloseDisplayZone(DisplayZone* zone)
{
	free(zone->mBuffer);
	zone->mBuffer = NULL;
	zone->mPosX = 0;
	zone->mPosY = 0;
	zone->mSizeX = 0;
	zone->mSizeY = 0;
}

void	PrintDisplayCharacterInDisplayZone(DisplayZone* zone, DisplayCharacter c, int px, int py)
{
	if (!zone->mBuffer)
		return;

	if (( px>=zone->mSizeX ) || (py >= zone->mSizeY) || (px < 0) || (py < 0) )
		return;

	DisplayCharacter* currentPos = zone->mBuffer + px + py * zone->mSizeX;

	DisplayCharacter	blended;
	Blender(&c, currentPos, &blended);
	(*currentPos) = blended;
}

void	PrintInDisplayZone(DisplayZone* zone, ConsoleColors FG, ConsoleColors BG, int px, int py, const char* str,int gotonextline,unsigned char characterFlag)
{
	if (!zone->mBuffer)
		return;

	if (py >= zone->mSizeY)
		return;

	DisplayCharacter* currentPos = zone->mBuffer + px + py * zone->mSizeX;

	DisplayCharacter	putPixel;
	CodeColors(&putPixel,FG, BG);
	putPixel.mCharactersFlag = characterFlag;

	while (*str)
	{
		putPixel.mCharacter = *str;
		if (px >= zone->mSizeX)
		{
			if (gotonextline == 0)
			{
				break;
			}
			else
			{
				++py;
				px = 0;
				currentPos = zone->mBuffer + px + py * zone->mSizeX;
				if (py >= zone->mSizeY)
				{
					break;
				}
			}
		}
		if ((px >= 0) && (py >= 0) && (px < zone->mSizeX) && (py < zone->mSizeY))
		{
			DisplayCharacter	blended;
			Blender(&putPixel, currentPos, &blended);
			(*currentPos) = blended;
		}
		++px;
		++currentPos;
		++str;

	}
}

void	ClearDisplayZone(DisplayZone* zone, ConsoleColors FG, ConsoleColors BG, unsigned short character,unsigned char characterFlag)
{
	if (!zone->mBuffer)
		return;

	DisplayCharacter* currentPos = zone->mBuffer;
	DisplayCharacter codeColor;
	CodeColors(&codeColor,FG, BG);
	codeColor.mCharacter = character;
	codeColor.mCharactersFlag = characterFlag;
	for (int j = 0; j < zone->mSizeY; j++)
	{
		for (int i = 0; i < zone->mSizeX; i++)
		{
			(*currentPos) = codeColor;
			++currentPos;
		}
	}
}

void	MoveDisplayZone(DisplayZone* zone,int newposx, int newposy)
{
	zone->mPosX = newposx;
	zone->mPosY = newposy;
}

// copy part of src zone at position px, py in dest zone
// px, py => position in destination
// srcpx, srcpy => bloc to copy position in src
// srcsx, srcsy => bloc size to copy 
void	CopyDisplayZonePart(DisplayZone* dest, DisplayZone* src, int px, int py, int srcpx, int srcpy, int srcsx, int srcsy)
{
	if ((!src->mBuffer) || (!dest->mBuffer))
	{
		return;
	}

	DisplayCharacter* srcPixels = src->mBuffer;
	DisplayCharacter* dstPixels = dest->mBuffer;
	for (int j = 0; j < srcsy; j++)
	{
		int dstPy = py + j;
		
		if ((dstPy >= 0) && (dstPy < dest->mSizeY))
		{
			int srcPy = srcpy + j;
			if ((srcPy >= 0) && (srcPy < src->mSizeY))
			{
				for (int i = 0; i < srcsx; i++)
				{
					int dstPx = px + i;
					
					if ((dstPx >= 0) && (dstPx < dest->mSizeX))
					{
						int srcPx = srcpx + i;
						if ((srcPx >= 0) && (srcPx < src->mSizeX))
						{
							DisplayCharacter* currentPixel = dstPixels + dstPx + dstPy * dest->mSizeX;
							DisplayCharacter* currentZonePos = srcPixels + srcPx + srcPy * src->mSizeX;

							DisplayCharacter blended;
							Blender(currentZonePos, currentPixel, &blended);

							*currentPixel = blended;
						}
					}
				}
			}
		}
	}

}

void	CopyDisplayZone(DisplayZone* dest, DisplayZone* src, int px, int py)
{
	CopyDisplayZonePart(dest, src, px, py, 0, 0, src->mSizeX, src->mSizeY);
}

void	ShowFPS(DisplaySettings* settings, int show)
{
	settings->mDisplayFPS = show;
}

void	FlushDisplayZone(DisplaySettings* settings, DisplayZone* zone)
{
	if (!zone->mBuffer) 
		return;
	
	DisplayCharacter* displayPixel = settings->mDrawBuffer;

	for (int j = 0; j < zone->mSizeY; j++)
	{
		DisplayCharacter* currentLPos = zone->mBuffer + j*zone->mSizeX;

		int py = j + zone->mPosY;
	
		if( (py >= 0) && (py<settings->mSizeY))
		{
			for (int i = 0; i < zone->mSizeX; i++)
			{
				int px = i + zone->mPosX;
				if ((zone->mSquaredPixels) && (!settings->mSquaredFont))
				{
					px *= 2;
				}
				if ((px >= 0) && (px < settings->mSizeX))
				{
					DisplayCharacter* currentPixel = displayPixel + px + py * settings->mSizeX;
					DisplayCharacter* currentZonePos = currentLPos + i;
					
					DisplayCharacter blended;
					Blender(currentZonePos, currentPixel, &blended);
					*currentPixel = blended;
					if ((zone->mSquaredPixels) && (!settings->mSquaredFont))
					{
						++currentPixel;
						Blender(currentZonePos, currentPixel, &blended);
						*currentPixel = blended;
					}
				}
			}
		}
	}

}

void	SwapBuffer(DisplaySettings* settings)
{
	// if display fps is set, then display FPS here
	if (settings->mDisplayFPS)
	{

		// get previous loop time
		static double lastSwapTime = 0.0;

		static int fps = 0;
		static int frameCount = 0;
		frameCount++;

		if (frameCount == 20)
		{
			// get current time
			double currentTime = GetTime();
			double cumuldt = currentTime - lastSwapTime;

			fps = (int)(20.0 / cumuldt);

			lastSwapTime = currentTime;

			frameCount = 0;
			cumuldt = 0.0;
		}


		DisplayZone	fpsZone;
		InitDisplayZone(&fpsZone, 5, 2, 16, 1, 0);
		ClearDisplayZone(&fpsZone, FOREGROUND, BACKGROUND, ' ', NO_CHARACTER);
		char	fpsString[16];

		snprintf(fpsString, 32, "FPS:%d ", fps);
		PrintInDisplayZone(&fpsZone, BACKGROUND | BRIGHT_WHITE | XOR_COLOR, BACKGROUND, 0, 0, fpsString, 0, NO_FLAG);
		FlushDisplayZone(settings, &fpsZone);
	}


	DisplayCharacter* frontBuffer = settings->mDrawBuffer;

	CHAR_INFO* buffer=malloc(settings->mSizeY* settings->mSizeX * sizeof(CHAR_INFO));
	memset(buffer, 0, settings->mSizeY * settings->mSizeX * sizeof(CHAR_INFO));

	CHAR_INFO* wbuffer = buffer;

	for (int j = 0; j < settings->mSizeY; j++)
	{
		for (int i = 0; i < settings->mSizeX; i++)
		{
			(*wbuffer).Char.UnicodeChar = frontBuffer->mCharacter;

			int FG, BG;
			DecodeColors(frontBuffer, &FG, &BG);
			FG = OLD_CONSOLE_COLORS[FG&0xF];
			BG = OLD_CONSOLE_COLORS[BG&0xF];
			(*wbuffer).Attributes = (FG | (BG << 4));

			++wbuffer;
			++frontBuffer;

		}
	}

	COORD coord = { 0, 0 }, size = { settings->mSizeX, settings->mSizeY };
	SMALL_RECT rect = { 0, 0, settings->mSizeX, settings->mSizeY };
	WriteConsoleOutputA(settings->mConsoleHandle, buffer, size,coord , &rect);

	free(buffer);
}




void	ClearBuffer(DisplaySettings* settings, ConsoleColors FG, ConsoleColors BG)
{
	// black screen

	DisplayCharacter* drawB = settings->mDrawBuffer;
	DisplayCharacter colorToSet;
	CodeColors(&colorToSet,FG, BG);
	colorToSet.mCharacter = ' ';
	colorToSet.mCharactersFlag = NO_FLAG;
	for (int j = 0; j < settings->mSizeY; j++)
	{
		for (int i = 0; i < settings->mSizeX; i++)
		{
			*drawB = colorToSet;
			++drawB;
		}
	}
}

