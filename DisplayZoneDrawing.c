#include <math.h>
#include "DisplayZoneDrawing.h"

#define ABS_INT(a) ((a<0)?(-(a)):(a))


unsigned char	greyRamp[] = " .:-=+*#@";
unsigned int	RGB_ConsoleColorsValues[16] = { 0x0C0C0C,0xC50F1F,0x13A1E0,0xC19C00,0x0037DA,0x881798,0x3A96DD,0xCCCCCC,
												0x767676,0xE74856,0x16C60C,0xF9F1A5,0x3B78FF,0xB4009E,0x61D6D6,0xF2F2F2 };


rgbVect	RGB_ConsoleColors[16];

extern void	Blender(DisplayCharacter* src, DisplayCharacter* dst, DisplayCharacter* blender);

// draw a filled rectangle with given pos, size, border and fill DisplayCharacter
void	DrawRectangleInDisplayZoneDisplayCharacter(DisplayZone* zone, int px, int py, int sx, int sy, DisplayCharacter* fill, DisplayCharacter* border)
{
	if (!zone->mBuffer)
	{
		return;
	}

	DisplayCharacter* dstPixels = zone->mBuffer;
	for (int j = 0; j < sy; j++)
	{
		DisplayCharacter* currentLineDisplay = fill;
		if ((j == 0) || (j == (sy - 1)))
		{
			currentLineDisplay = border;
		}

		int dstPy = py + j;

		if ((dstPy >= 0) && (dstPy < zone->mSizeY))
		{
			for (int i = 0; i < sx; i++)
			{
				DisplayCharacter* currentDisplay = currentLineDisplay;
				if ((i == 0) || (i == (sx - 1)))
				{
					currentDisplay = border;
				}
				int dstPx = px + i;
				if ((dstPx >= 0) && (dstPx < zone->mSizeX))
				{
					DisplayCharacter* currentPixel = dstPixels + dstPx + dstPy * zone->mSizeX;

					DisplayCharacter blended;

					Blender(currentDisplay, currentPixel, &blended);

					*currentPixel = blended;
				}

			}
		}
	}
}

// draw a filled rectangle with given pos, size, border and fill colors and character
void	DrawRectangleInDisplayZoneEx(DisplayZone* zone, int px, int py, int sx, int sy, ConsoleColors fillFG, ConsoleColors fillBG, char fillCharacter, ConsoleColors borderFG, ConsoleColors borderBG, char borderCharacter)
{
	if (!zone->mBuffer)
	{
		return;
	}

	DisplayCharacter fillPixel;
	CodeColors(&fillPixel, fillFG, fillBG);
	fillPixel.mCharacter = fillCharacter;
	fillPixel.mCharactersFlag = NO_FLAG;
	DisplayCharacter borderPixel;
	CodeColors(&borderPixel,borderFG, borderBG);
	borderPixel.mCharacter = borderCharacter;
	borderPixel.mCharactersFlag = NO_FLAG;

	DrawRectangleInDisplayZoneDisplayCharacter(zone, px, py, sx, sy, &fillPixel, &borderPixel);
}

// draw a rectangle with given pos, size, colors and character
void	DrawRectangleInDisplayZone(DisplayZone* zone, int px, int py, int sx, int sy, ConsoleColors FG, ConsoleColors BG, char character)
{
	DrawRectangleInDisplayZoneEx(zone, px, py, sx, sy, FG, BG, character, FG, BG, character);
}


void	DrawDiscInDisplayZoneDisplayCharacter(DisplayZone* zone, int cx, int cy, int r, DisplayCharacter* fill, DisplayCharacter* border)
{
	if (!zone->mBuffer)
	{
		return;
	}

	float fr = (float)r;

	DisplayCharacter* dstPixels = zone->mBuffer;
	int lastStartX = 0;
	for (int j = -r; j <= 0; j++)
	{
		int dstPy[2];
		dstPy[0] = cy + j;
		dstPy[1] = cy - j;
		float fj = ((float)ABS_INT(j)) - 0.5f;
		int startX = (int)sqrtf(fr * fr - fj * fj);

		for (int i = -startX; i <= startX; i++)
		{
			int dstPx = cx + i;
			if ((dstPx >= 0) && (dstPx < zone->mSizeX))
			{
				DisplayCharacter* currentLineDisplay = fill;
				if (ABS_INT(i) >= lastStartX)
				{
					currentLineDisplay = border;
				}

				int symLimit = (dstPy[0] == dstPy[1]) ? 1 : 2;
				for (int sym = 0; sym < symLimit; sym++)
				{
					if ((dstPy[sym] >= 0) && (dstPy[sym] < zone->mSizeY))
					{
						DisplayCharacter* currentPixel = dstPixels + dstPx + dstPy[sym] * zone->mSizeX;
						DisplayCharacter blended;
						Blender(currentLineDisplay, currentPixel, &blended);
						*currentPixel = blended;
					}
				}
			}
		}

		lastStartX = startX;
	}
}


// draw disc with given center, radius, border and fill colors and character
void	DrawDiscInDisplayZoneEx(DisplayZone* zone, int cx, int cy, int r, ConsoleColors fillFG, ConsoleColors fillBG, char fillCharacter, ConsoleColors borderFG, ConsoleColors borderBG, char borderCharacter)
{
	DisplayCharacter fillPixel;
	CodeColors(&fillPixel, fillFG, fillBG);
	fillPixel.mCharacter = fillCharacter;
	fillPixel.mCharactersFlag = NO_FLAG;
	DisplayCharacter borderPixel;
	CodeColors(&borderPixel, borderFG, borderBG);
	borderPixel.mCharacter = borderCharacter;
	borderPixel.mCharactersFlag = NO_FLAG;

	DrawDiscInDisplayZoneDisplayCharacter(zone, cx, cy, r, &fillPixel, &borderPixel);
}

// draw disc with given center, radius, colors and character
void	DrawDiscInDisplayZone(DisplayZone* zone, int cx, int cy, int r, ConsoleColors FG, ConsoleColors BG, char character)
{
	DrawDiscInDisplayZoneEx(zone, cx, cy, r, FG, BG, character, FG, BG, character);
}



rgbVect getRGBFromConsoleIndex(unsigned int index)
{
	rgbVect	result;
	result.R = (int)(RGB_ConsoleColorsValues[index] >> 16);
	result.G = (int)((RGB_ConsoleColorsValues[index] >> 8) & 0xFF);
	result.B = (int)(RGB_ConsoleColorsValues[index] & 0xFF);
	return result;
}
void initRGBConsoleColor()
{
	for (int i = 0; i < 16; i++)
	{
		RGB_ConsoleColors[i] = getRGBFromConsoleIndex(i);
	}
}
rgbVect getRGBFromValues(unsigned char R, unsigned char G, unsigned char B)
{
	rgbVect	result;
	result.R = (int)R;
	result.G = (int)G;
	result.B = (int)B;
	return result;
}

rgbVect	getRGBDirection(rgbVect start, rgbVect end)
{
	rgbVect	result;
	result.R = end.R - start.R;
	result.G = end.G - start.G;
	result.B = end.B - start.B;
	return result;
}

void RGBMult(rgbVect* c, float f)
{
	c->R *= f;
	c->G *= f;
	c->B *= f;
}

void RGBIntDiv(rgbVect* c, int d)
{
	c->R /= d;
	c->G /= d;
	c->B /= d;
}

void RGBIntMult(rgbVect* c, int m)
{
	c->R *= m;
	c->G *= m;
	c->B *= m;
}

// add v2 to v1
void RGBAdd(rgbVect* v1, rgbVect* v2)
{
	v1->R += v2->R;
	v1->G += v2->G;
	v1->B += v2->B;
}

// sub v2 from v1
void RGBSub(rgbVect* v1, rgbVect* v2)
{
	v1->R -= v2->R;
	v1->G -= v2->G;
	v1->B -= v2->B;
}

void RGBNeg(rgbVect* v1)
{
	v1->R = -v1->R;
	v1->G = -v1->G;
	v1->B = -v1->B;
}

void RGBABS(rgbVect* v1)
{
	v1->R = (v1->R < 0) ? (-v1->R) : v1->R;
	v1->G = (v1->G < 0) ? (-v1->G) : v1->G;
	v1->B = (v1->B < 0) ? (-v1->B) : v1->B;
}

unsigned int	getRGBSquaredNorm(rgbVect c)
{
	return c.R * c.R + c.G * c.G + c.B * c.B;
}

int	getRGBDot(rgbVect c1,rgbVect c2)
{
	return c1.R * c2.R + c1.G * c2.G + c1.B * c2.B;
}

// 3D distance, don't take visual distance into account
unsigned int	getNearestConsoleColor(rgbVect input)
{
	unsigned int bestSquaredDist = 3*255*255;
	unsigned int bestIndex = -1;
	for (int i = 0; i < 16; i++)
	{
		rgbVect diff = getRGBDirection(RGB_ConsoleColors[i], input);

		unsigned int squaredDist = getRGBSquaredNorm(diff);
		if (squaredDist < bestSquaredDist)
		{
			bestSquaredDist = squaredDist;
			bestIndex = i;
		}
	}
	return bestIndex;
}

// 3D oriented distance, don't take visual distance into account
unsigned int	getOrientedNearestConsoleColor(rgbVect input,unsigned int nearestIndex)
{
	rgbVect direction = getRGBDirection(RGB_ConsoleColors[nearestIndex], input);

	unsigned int bestIndex = 16;

	int sqrnorm = getRGBSquaredNorm(direction);
	if (sqrnorm)
	{
		unsigned int bestSquaredDist = 10000000;
		
		for (int i = 0; i < 16; i++)
		{
			if (i == nearestIndex)
				continue;

			rgbVect colorV = getRGBDirection(input, RGB_ConsoleColors[i]);

			int dot = 256*getRGBDot(colorV, direction);
			if (dot > 0.0)
			{
				dot /= sqrnorm;
				rgbVect proj = direction;
				proj.R *= dot;
				proj.R /= 256;
				proj.G *= dot;
				proj.G /= 256;
				proj.B *= dot;
				proj.B /= 256;

				rgbVect perp = getRGBDirection(proj, colorV);
				perp.R *= 4;
				perp.G *= 4;
				perp.B *= 4;

				unsigned int dist = getRGBSquaredNorm(proj) + getRGBSquaredNorm(perp);
				
				if (dist < bestSquaredDist)
				{
					bestSquaredDist = dist;
					bestIndex = i;
				}
			}
			
		}
	}
	return bestIndex;
}

// 4 pixels : 
// 
// 0 1
// 2 3

unsigned char blendRamp[4] = { ' ',176,177,178 };

DisplayCharacter	GetDisplayCharacterFrom4Pixels(RGBPixel* FourPixels)
{
	rgbVect inputs[4];

	for (int i = 0; i < 4; i++)
	{
		inputs[i]= getRGBFromValues(FourPixels[i].R, FourPixels[i].G, FourPixels[i].B);
	}

	rgbVect	moyennes[4];
	// moyenne 0,1
	moyennes[0] = inputs[0];
	RGBAdd(&(moyennes[0]),&inputs[1]);
	RGBIntDiv(&(moyennes[0]), 2);
	// moyenne 2,3
	moyennes[1] = inputs[2];
	RGBAdd(&(moyennes[1]), &inputs[3]);
	RGBIntDiv(&(moyennes[1]), 2);
	// moyenne 0,2
	moyennes[2] = inputs[0];
	RGBAdd(&(moyennes[2]), &inputs[2]);
	RGBIntDiv(&(moyennes[2]), 2);
	// moyenne 1,3
	moyennes[3] = inputs[1];
	RGBAdd(&(moyennes[3]), &inputs[3]);
	RGBIntDiv(&(moyennes[3]), 2);

	rgbVect	diffs[2];
	unsigned int sqrnorm[2];
	diffs[0] = moyennes[0];
	RGBSub(&diffs[0],&moyennes[1]);
	sqrnorm[0] = getRGBSquaredNorm(diffs[0]);
	unsigned int maxNorm = 0;

	diffs[1] = moyennes[2];
	RGBSub(&diffs[1], &moyennes[3]);
	sqrnorm[1] = getRGBSquaredNorm(diffs[1]);
	if (sqrnorm[1] > sqrnorm[0])
	{
		maxNorm = 1;
	}
	
	DisplayCharacter result;
	// if high contrast 
	if (sqrnorm[maxNorm] > 8192)
	{
		if (maxNorm == 0)
		{
			unsigned int up = getNearestConsoleColor(moyennes[0]);
			unsigned int down = getNearestConsoleColor(moyennes[1]);
			result=ENCODE_DISPLAY_CHARACTER(up, down, 223, 0);
		}
		else
		{
			unsigned int left = getNearestConsoleColor(moyennes[2]);
			unsigned int right = getNearestConsoleColor(moyennes[3]);
			result = ENCODE_DISPLAY_CHARACTER(left, right, 221, 0);
		}
	}
	else
	{
		moyennes[0] = inputs[0];
		RGBAdd(&(moyennes[0]), &inputs[1]);
		RGBAdd(&(moyennes[0]), &inputs[2]);
		RGBAdd(&(moyennes[0]), &inputs[3]);
		RGBIntDiv(&(moyennes[0]), 4);

		unsigned int nearest = getNearestConsoleColor(moyennes[0]);
		unsigned int opposite = getOrientedNearestConsoleColor(moyennes[0], nearest);

		if (opposite != 16)
		{
			rgbVect	v1 = getRGBDirection(RGB_ConsoleColors[nearest], RGB_ConsoleColors[opposite]);
			rgbVect	v2 = getRGBDirection(RGB_ConsoleColors[nearest], moyennes[0]);

			int	dot = 65536 * getRGBDot(v1, v2);
			dot /= getRGBSquaredNorm(v1);

			// dot is in [0,0.5]

			int blendIndex = (3 * dot) / 32768;

			DisplayCharacter blend = ENCODE_DISPLAY_CHARACTER(opposite, nearest, blendRamp[blendIndex], 0);
			result = blend;
		}
		else
		{
			DisplayCharacter blend = ENCODE_DISPLAY_CHARACTER(BLACK, nearest, ' ', 0);
			result = blend;
		}

	}

	return result;
}

DisplayCharacter	GetAsciiArtFromRGB(unsigned char R, unsigned char G, unsigned char B)
{
	rgbVect input = getRGBFromValues(R, G, B);

	unsigned int nearest = getNearestConsoleColor(input);
	unsigned int opposite = getOrientedNearestConsoleColor(input, nearest);

	DisplayCharacter result;
	if(opposite != 16)
	{
		rgbVect	v1 = getRGBDirection(RGB_ConsoleColors[nearest], RGB_ConsoleColors[opposite]);
		rgbVect	v2 = getRGBDirection(RGB_ConsoleColors[nearest], input);

		int	dot = 65536*getRGBDot(v1, v2);
		dot /= getRGBSquaredNorm(v1);

		// dot is in [0,0.5]

		int blendIndex = (9 * dot) / 32768;

		DisplayCharacter blend =  ENCODE_DISPLAY_CHARACTER(opposite,nearest,greyRamp[blendIndex],0) ;
		result = blend;
	}
	else
	{
		DisplayCharacter blend = ENCODE_DISPLAY_CHARACTER(BLACK, nearest, ' ', 0);
		result = blend;
	}
	return result;
}

DisplayCharacter	GetAsciiArtFromRGBExt(unsigned char R, unsigned char G, unsigned char B, rgbVect* error)
{
	rgbVect input = getRGBFromValues(R, G, B);

	unsigned int nearest = getNearestConsoleColor(input);
	unsigned int opposite = getOrientedNearestConsoleColor(input, nearest);

	DisplayCharacter result;


	if (opposite != 16)
	{
		rgbVect	v1 = getRGBDirection(RGB_ConsoleColors[nearest], RGB_ConsoleColors[opposite]);
		rgbVect	v2 = getRGBDirection(RGB_ConsoleColors[nearest], input);

		int	dot = 65536 * getRGBDot(v1, v2);
		dot /= getRGBSquaredNorm(v1);

		// dot is in [0,0.5]

		int blendIndex = (9 * dot) / 32768;

		DisplayCharacter blend = ENCODE_DISPLAY_CHARACTER(opposite, nearest, greyRamp[blendIndex], 0);
		result = blend;

		*error = v1;
		(*error).R *= dot;
		(*error).R /= 65536*8;
		(*error).R *= 8;
		(*error).G *= dot;
		(*error).G /= 65536*8;
		(*error).G *= 8;
		(*error).B *= dot;
		(*error).B /= 65536*8;
		(*error).B *= 8;

		(*error).R += RGB_ConsoleColors[nearest].R;
		(*error).G += RGB_ConsoleColors[nearest].G;
		(*error).B += RGB_ConsoleColors[nearest].B;
	}
	else
	{
		DisplayCharacter blend = ENCODE_DISPLAY_CHARACTER(BLACK, nearest, ' ', 0);
		result = blend;

		(*error) = RGB_ConsoleColors[nearest];

	}

	(*error).R = input.R - (*error).R;
	(*error).G = input.G - (*error).G;
	(*error).B = input.B - (*error).B;
	return result;
}

void	clamp_0_255(int* toClamp)
{
	if (*toClamp < 0)
	{
		*toClamp = 0;
	}
	else if (*toClamp > 255)
	{
		*toClamp = 255;
	}
}

void clamp(rgbVect* toclamp)
{
	clamp_0_255(&(toclamp->R));
	clamp_0_255(&(toclamp->G));
	clamp_0_255(&(toclamp->B));
}

void	convertBufferWithDithering(RGBPixel* buffer, int imgSizeX, int imgSizeY, DisplayZone* zone)
{
	for (int j = 0; j < imgSizeY; j++)
	{
		int lineindex = j * imgSizeX;
		for (int i = 0; i < imgSizeX; i++)
		{

			rgbVect	error;
			DisplayCharacter color = GetAsciiArtFromRGBExt(buffer[i + lineindex].R, buffer[i + lineindex].G, buffer[i + lineindex].B, &error);
			PrintDisplayCharacterInDisplayZone(zone, color, i, j);

			// half error on right pixel, half on bottom

			rgbVect halferror = error;
			halferror.R /= 2;
			halferror.G /= 2;
			halferror.B /= 2;

			if (i < (imgSizeX - 1))
			{
				rgbVect reportX = error;
				reportX.R -= halferror.R;
				reportX.G -= halferror.G;
				reportX.B -= halferror.B;

				reportX.R += buffer[i + lineindex + 1].R;
				reportX.G += buffer[i + lineindex + 1].G;
				reportX.B += buffer[i + lineindex + 1].B;
				clamp(&reportX);
				buffer[i + lineindex + 1].R = reportX.R;
				buffer[i + lineindex + 1].G = reportX.G;
				buffer[i + lineindex + 1].B = reportX.B;
			}

			if (j < (imgSizeY - 1))
			{
				rgbVect reportY = halferror;
				reportY.R += buffer[i + lineindex + imgSizeX].R;
				reportY.G += buffer[i + lineindex + imgSizeX].G;
				reportY.B += buffer[i + lineindex + imgSizeX].B;
				clamp(&reportY);
				buffer[i + lineindex + imgSizeX].R = reportY.R;
				buffer[i + lineindex + imgSizeX].G = reportY.G;
				buffer[i + lineindex + imgSizeX].B = reportY.B;
			}
		}
	}
}


void	convertImageWithDithering(BITMAPINFO* BitmapInfo, unsigned char* bmpdata, DisplayZone* zone)
{
	int imgSizeX = BitmapInfo->bmiHeader.biWidth;
	int zoneX = zone->mSizeX;
	int imgSizeY = BitmapInfo->bmiHeader.biHeight;
	int zoneY = zone->mSizeY;

	RGBPixel* buffer = (RGBPixel*)malloc(zoneX * zoneY * sizeof(RGBPixel));
	RGBPixel* writeBuffer = buffer;
	// copy bitmap in buffer
	for (int j = 0; j < zoneY; j++)
	{
		if (j >= imgSizeY)
			break;

		writeBuffer = buffer + j * zoneX;
		for (int i = 0; i < zoneX; i++)
		{
			if (i >= imgSizeX)
				break;

			*writeBuffer = getBitmapRGB(BitmapInfo, bmpdata, i, j);
			writeBuffer++;
		}
	}
	convertBufferWithDithering(buffer, zoneX, zoneY, zone);

	free(buffer);
}

// no dithering, only block or blend done with pixel grid for sprites
DisplayZone* CreateDisplayZoneFromBMP(const char* bmpname)
{
	BITMAPINFO* BitmapInfo = NULL;
	unsigned char* bmpdata = LoadDIBitmap(bmpname, &BitmapInfo);
	if ((!BitmapInfo) || (!bmpdata))
	{
		// error
		return NULL;
	}

	int imgSizeX = BitmapInfo->bmiHeader.biWidth;
	int imgSizeY = BitmapInfo->bmiHeader.biHeight;

	DisplayZone*	resultZone =(DisplayZone *) malloc(sizeof(DisplayZone));
	InitDisplayZone(resultZone, 0, 0, imgSizeX / 2, imgSizeY / 2, 1);

	for (int j = 0; j < imgSizeY; j += 2)
	{
		for (int i = 0; i < imgSizeX; i += 2)
		{
			RGBPixel pixel[4];

			pixel[0] = getBitmapRGB(BitmapInfo, bmpdata, i, j);
			pixel[1] = getBitmapRGB(BitmapInfo, bmpdata, i + 1, j);
			pixel[2] = getBitmapRGB(BitmapInfo, bmpdata, i, j + 1);
			pixel[3] = getBitmapRGB(BitmapInfo, bmpdata, i + 1, j + 1);

			DisplayCharacter color = GetDisplayCharacterFrom4Pixels(pixel);
			PrintDisplayCharacterInDisplayZone(resultZone, color, i / 2, j / 2);
		}
	}
	free(BitmapInfo);
	free(bmpdata);

	return resultZone;
}