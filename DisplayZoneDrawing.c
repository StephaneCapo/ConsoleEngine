#include <math.h>
#include "DisplayZoneDrawing.h"

#define ABS_INT(a) ((a<0)?(-(a)):(a))


unsigned char	greyRamp[] = " .:-=+*#";
unsigned int	RGB_ConsoleColorsValues[16] = { 0x0C0C0C,0xC50F1F,0x13A10E,0xC19C00,0x0037DA,0x881798,0x3A96DD,0xCCCCCC,
												0x767676,0xE74856,0x16C60C,0xF9F1A5,0x3B78FF,0xB4009E,0x61D6D6,0xF2F2F2 };



rgbVect	RGB_ConsoleColors[16];

//#define LAB_TESTS
#ifdef LAB_TESTS
rgbVect RGB_LABConsoleColors[16];
#endif

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

		if ((dstPy >= 0) && ((unsigned int)dstPy < zone->mSizeY))
		{
			for (int i = 0; i < sx; i++)
			{
				DisplayCharacter* currentDisplay = currentLineDisplay;
				if ((i == 0) || (i == (sx - 1)))
				{
					currentDisplay = border;
				}
				int dstPx = px + i;
				if ((dstPx >= 0) && ((unsigned int)dstPx < zone->mSizeX))
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
			if ((dstPx >= 0) && ((unsigned int)dstPx < zone->mSizeX))
			{
				DisplayCharacter* currentLineDisplay = fill;
				if (ABS_INT(i) >= lastStartX)
				{
					currentLineDisplay = border;
				}

				int symLimit = (dstPy[0] == dstPy[1]) ? 1 : 2;
				for (int sym = 0; sym < symLimit; sym++)
				{
					if ((dstPy[sym] >= 0) && ((unsigned int)dstPy[sym] < zone->mSizeY))
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

void	RGBSet(rgbVect* c, int v)
{
	c->R = v;
	c->G = v;
	c->B = v;
}

void	RGBMin(rgbVect* v1, rgbVect* v2)
{
	v1->R = (v1->R > v2->R) ? v2->R : v1->R;
	v1->G = (v1->G > v2->G) ? v2->G : v1->G;
	v1->B = (v1->B > v2->B) ? v2->B : v1->B;
}


void	RGBMax(rgbVect* v1, rgbVect* v2)
{
	v1->R = (v1->R < v2->R) ? v2->R : v1->R;
	v1->G = (v1->G < v2->G) ? v2->G : v1->G;
	v1->B = (v1->B < v2->B) ? v2->B : v1->B;
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

void	clamp(int* toClamp,int min,int max)
{
	if (*toClamp < min)
	{
		*toClamp = min;
	}
	else if (*toClamp > max)
	{
		*toClamp = max;
	}
}

void RGBClamp(rgbVect* toclamp)
{
	clamp_0_255(&(toclamp->R));
	clamp_0_255(&(toclamp->G));
	clamp_0_255(&(toclamp->B));
}

#ifdef LAB_TESTS
void LABClamp(rgbVect* toclamp)
{
	clamp(&(toclamp->R),0,100);
	clamp(&(toclamp->G),-86,98);
	clamp(&(toclamp->B),-107,94);
}

// the following functions are based off of the pseudocode
// found on www.easyrgb.com

rgbVect lab2rgb(rgbVect lab)
{

	float y = (lab.R + 16) / 116.0;
	float x = lab.G / 500.0 + y;
	float z = y - lab.B / 200.0;

	x = 0.95047f * ((x * x * x > 0.008856f) ? x * x * x : (x - 16.0f / 116.0f) / 7.787f);
	y = 1.00000f * ((y * y * y > 0.008856f) ? y * y * y : (y - 16.0f / 116.0f) / 7.787f);
	z = 1.08883f * ((z * z * z > 0.008856f) ? z * z * z : (z - 16.0f / 116.0f) / 7.787f);

	rgbVect result;
	float r = x * 3.2406f + y * -1.5372f + z * -0.4986f;
	float g = x * -0.9689f + y * 1.8758f + z * 0.0415f;
	float b = x * 0.0557f + y * -0.2040f + z * 1.0570f;

	r = (r > 0.0031308f) ? (1.055f * powf(r, 1.0f / 2.4f) - 0.055f) : 12.92f * r;
	g = (g > 0.0031308f) ? (1.055f * powf(g, 1.0f / 2.4f) - 0.055f) : 12.92f * g;
	b = (b > 0.0031308f) ? (1.055f * powf(b, 1.0f / 2.4f) - 0.055f) : 12.92f * b;

	result.R = r * 255.0f;
	result.G = g * 255.0f;
	result.B = b * 255.0f;


	RGBClamp(&result);

	return result;
}


rgbVect rgb2lab(rgbVect rgb) 
{
	float r = rgb.R / 255.0f;
	float g = rgb.G / 255.0f;
	float b = rgb.B / 255.0f;
	float x, y, z;

	r = (r > 0.04045f) ? powf((r + 0.055f) / 1.055f, 2.4f) : r / 12.92f;
	g = (g > 0.04045f) ? powf((g + 0.055f) / 1.055f, 2.4f) : g / 12.92f;
	b = (b > 0.04045f) ? powf((b + 0.055f) / 1.055f, 2.4f) : b / 12.92f;

	x = (r * 0.4124f + g * 0.3576f + b * 0.1805f) / 0.95047f;
	y = (r * 0.2126f + g * 0.7152f + b * 0.0722f) / 1.00000f;
	z = (r * 0.0193f + g * 0.1192f + b * 0.9505f) / 1.08883f;

	x = (x > 0.008856f) ? powf(x, 1.0f / 3.0f) : (7.787f * x) + 16.0f / 116.0f;
	y = (y > 0.008856f) ? powf(y, 1.0f / 3.0f) : (7.787f * y) + 16.0f / 116.0f;
	z = (z > 0.008856f) ? powf(z, 1.0f / 3.0f) : (7.787f * z) + 16.0f / 116.0f;

	rgbVect result;
	result.R = (116.0f * y) - 16;
	result.G = 500.0f * (x - y);
	result.B = 200.0 * (y - z);
	return result;
}

// calculate the perceptual distance between colors in CIELAB
// https://github.com/THEjoezack/ColorMine/blob/master/ColorMine/ColorSpaces/Comparisons/Cie94Comparison.cs

float deltaE(rgbVect labA, rgbVect labB) 
{
	rgbVect delta = getRGBDirection(labB, labA);
	
	float c1 = sqrtf(labA.G * labA.G + labA.B * labA.B);
	float c2 = sqrtf(labB.G * labB.G + labB.B * labB.B);
	float deltaC = c1 - c2;
	float deltaH = delta.G * delta.G + delta.B * delta.B - deltaC * deltaC;
	deltaH = deltaH < 0.0f ? 0.0f : sqrtf(deltaH);
	float sc = 1.0f + 0.045f * c1;
	float sh = 1.0f + 0.015f * c1;
	float deltaLKlsl = delta.R; // / (1.0);
	float deltaCkcsc = deltaC / (sc);
	float deltaHkhsh = deltaH / (sh);
	float i = deltaLKlsl * deltaLKlsl + deltaCkcsc * deltaCkcsc + deltaHkhsh * deltaHkhsh;
	return i < 0.0f ? 0.0f : sqrtf(i);
}

float squaredDeltaE(rgbVect labA, rgbVect labB)
{
	rgbVect delta = getRGBDirection(labB, labA);

	float c1 = sqrtf(labA.G * labA.G + labA.B * labA.B);
	float c2 = sqrtf(labB.G * labB.G + labB.B * labB.B);
	float deltaC = c1 - c2;
	float deltaH = delta.G * delta.G + delta.B * delta.B - deltaC * deltaC;
	deltaH = (deltaH < 0.0f) ? 0.0f : deltaH;
	float sc = 1.0f + 0.045f * c1;
	float sh = 1.0f + 0.015f * c1;
	float deltaLKlsl = delta.R; // / (1.0);
	float deltaCkcsc = deltaC / (sc);
	float deltaHkhsh = deltaH / (sh*sh);
	float i = deltaLKlsl * deltaLKlsl + deltaCkcsc * deltaCkcsc + deltaHkhsh;
	return (i < 0.0f) ? 0.0f : i;
}
#endif // LAB

void initRGBConsoleColor()
{

	for (int i = 0; i < 16; i++)
	{
		RGB_ConsoleColors[i] = getRGBFromConsoleIndex(i);
#ifdef LAB_TESTS
		RGB_LABConsoleColors[i] = rgb2lab(RGB_ConsoleColors[i]);
#endif
	}

}
// 3D distance
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

#ifdef LAB_TESTS
unsigned int	getNearestLABConsoleColor(rgbVect input)
{
	float bestSquaredDist = 255.0f*255.0f*3.0f;
	unsigned int bestIndex = -1;
	for (int i = 0; i < 16; i++)
	{
		float sqrdist = squaredDeltaE(RGB_LABConsoleColors[i], input);
		
		if (sqrdist < bestSquaredDist)
		{
			bestSquaredDist = sqrdist;
			bestIndex = i;
		}
	}
	return bestIndex;
}
#endif

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

			rgbVect colorV = getRGBDirection(RGB_ConsoleColors[nearestIndex], RGB_ConsoleColors[i]);

			int dot = 256*getRGBDot(colorV, direction);
			if (dot > 0)
			{
				int dot2 = dot;
				dot2 /= sqrnorm;

				if (dot2 > 256)
				{
					rgbVect proj = direction;
					RGBIntMult(&proj, dot2);
					RGBIntDiv(&proj, 256);
				
					rgbVect perp = getRGBDirection(proj, colorV);

					unsigned int dist = getRGBSquaredNorm(perp);

					if (dist < bestSquaredDist)
					{
						bestSquaredDist = dist;
						bestIndex = i;
					}
				}
			}
		}
	}
	return bestIndex;
}
#ifdef LAB_TESTS
unsigned int	getOrientedNearestLABConsoleColor(rgbVect input, unsigned int nearestIndex)
{
	rgbVect direction = getRGBDirection(RGB_LABConsoleColors[nearestIndex], input);
	unsigned int bestIndex = 16;

	int sqrnorm = getRGBSquaredNorm(direction);
	if (sqrnorm)
	{
		float bestSquaredDist = 10000000.0f;

		for (int i = 0; i < 16; i++)
		{
			if (i == nearestIndex)
				continue;

			rgbVect colorV = getRGBDirection(RGB_LABConsoleColors[nearestIndex], RGB_LABConsoleColors[i]);
			
			int dot = 256 * getRGBDot(colorV, direction);
			if (dot > 0)
			{
				int dot2 = dot;
				dot2 /= sqrnorm;

				if (dot2 > 256)
				{
					rgbVect proj = direction;
					RGBIntMult(&proj, dot2);
					RGBIntDiv(&proj, 256);

					float dist = squaredDeltaE(proj, colorV);

					if (dist < bestSquaredDist)
					{
						bestSquaredDist = dist;
						bestIndex = i;
					}
				}
			}
		}
	}
	return bestIndex;
}
#endif


int MixColors(int col1, int col2)
{
	rgbVect mixed1 = RGB_ConsoleColors[col1];
	rgbVect mixed2 = RGB_ConsoleColors[col2];

	RGBAdd(&mixed1, &mixed2);
	RGBIntDiv(&mixed1, 2);

	return getNearestConsoleColor(mixed1);
}

// 4 pixels : 
// 
// 0 1
// 2 3

unsigned char blendRamp[3] = { ' ',176,177 };

DisplayCharacter	GetDisplayCharacterFrom4Pixels(RGBAPixel* FourPixels)
{
	rgbVect inputs[4];

	for (int i = 0; i < 4; i++)
	{
		inputs[i]= getRGBFromValues(FourPixels[i].R, FourPixels[i].G, FourPixels[i].B);
	}

	rgbVect			moyennes[4];
	unsigned int	moyenneAlpha[4];
	// moyenne 0,1
	moyenneAlpha[0] = (FourPixels[0].A + FourPixels[1].A)/2;
	if (moyenneAlpha[0] < 86) // full transparent
	{
		 RGBSet(&moyennes[0],-255);
	}
	else
	{
		moyennes[0] = inputs[0];
		RGBAdd(&(moyennes[0]), &inputs[1]);
		RGBIntDiv(&(moyennes[0]), 2);
	}
	// moyenne 2,3
	moyenneAlpha[1] = (FourPixels[2].A + FourPixels[3].A) / 2;
	if (moyenneAlpha[1] < 86) // full transparent
	{
		RGBSet(&moyennes[1], -255);
	}
	else
	{
		moyennes[1] = inputs[2];
		RGBAdd(&(moyennes[1]), &inputs[3]);
		RGBIntDiv(&(moyennes[1]), 2);
	}
	// moyenne 0,2
	moyenneAlpha[2] = (FourPixels[0].A + FourPixels[2].A) / 2;
	if (moyenneAlpha[2] < 86) // full transparent
	{
		RGBSet(&moyennes[2], -255);
	}
	else
	{
		moyennes[2] = inputs[0];
		RGBAdd(&(moyennes[2]), &inputs[2]);
		RGBIntDiv(&(moyennes[2]), 2);
	}
	// moyenne 1,3
	moyenneAlpha[3] = (FourPixels[1].A + FourPixels[3].A) / 2;
	if (moyenneAlpha[3] < 86) // full transparent
	{
		RGBSet(&moyennes[3], -255);
	}
	else
	{
		moyennes[3] = inputs[1];
		RGBAdd(&(moyennes[3]), &inputs[3]);
		RGBIntDiv(&(moyennes[3]), 2);
	}

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
			if (moyenneAlpha[0]<86)
			{
				up = FOREGROUND;
			}
			else if (moyenneAlpha[0] < 172)
			{
				up |= FOREGROUND | MIX;
			}
			unsigned int down = getNearestConsoleColor(moyennes[1]);
			if (moyenneAlpha[1] < 86)
			{
				down = BACKGROUND;
			}
			else if (moyenneAlpha[1] < 172)
			{
				down |= BACKGROUND | MIX;
			}
			result=ENCODE_DISPLAY_CHARACTER(up, down, 223, 0);
		}
		else
		{
			unsigned int left = getNearestConsoleColor(moyennes[2]);
			if (moyenneAlpha[2] < 86)
			{
				left = FOREGROUND;
			}
			else if (moyenneAlpha[2] < 172)
			{
				left |= FOREGROUND | MIX;
			}
			unsigned int right = getNearestConsoleColor(moyennes[3]);
			if (moyenneAlpha[3] < 86)
			{
				right = BACKGROUND;
			}
			else if (moyenneAlpha[3] < 172)
			{
				right |= BACKGROUND | MIX;
			}
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

		moyenneAlpha[0] = (FourPixels[0].A + FourPixels[1].A + FourPixels[2].A + FourPixels[3].A) / 4;

		if (moyenneAlpha[0] < 86)
		{
			result = ENCODE_DISPLAY_CHARACTER(FOREGROUND, BACKGROUND, ' ', NO_CHARACTER);
		}
		else
		{
			unsigned int nearest = getNearestConsoleColor(moyennes[0]);
			unsigned int opposite = getOrientedNearestConsoleColor(moyennes[0], nearest);

			if (opposite != 16)
			{
				rgbVect	v1 = getRGBDirection(RGB_ConsoleColors[nearest], RGB_ConsoleColors[opposite]);
				rgbVect	v2 = getRGBDirection(RGB_ConsoleColors[nearest], moyennes[0]);

				int	dot = 65536 * getRGBDot(v1, v2);
				dot /= getRGBSquaredNorm(v1);

				// dot is in [0,0.5]

				int blendIndex = (2 * dot) / 32768;
				if (blendIndex >= 2)
					blendIndex = 2;

				if (moyenneAlpha[0] < 172)
				{
					result = ENCODE_DISPLAY_CHARACTER(FOREGROUND| MIX |opposite, BACKGROUND | MIX | nearest, blendRamp[blendIndex], 0);
				}
				else
				{
					result = ENCODE_DISPLAY_CHARACTER(opposite, nearest, blendRamp[blendIndex], 0);
				}
			}
			else
			{
				if (moyenneAlpha[0] < 172)
				{
					result = ENCODE_DISPLAY_CHARACTER(FOREGROUND, BACKGROUND | MIX | nearest, ' ', NO_CHARACTER);
				}
				else
				{
					result = ENCODE_DISPLAY_CHARACTER(BLACK, nearest, ' ', 0);
				}
			}
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

		int blendIndex = (8 * dot) / 32768;
		if (blendIndex > 7)
			blendIndex = 7;

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

DisplayCharacter	GetAsciiArtFromRGBExt(RGBPixel input, rgbVect* error)
{
	rgbVect RGBInput = getRGBFromValues(input.R, input.G, input.B);

	unsigned int nearest = getNearestConsoleColor(RGBInput);
	unsigned int opposite = getOrientedNearestConsoleColor(RGBInput, nearest);

	DisplayCharacter result;


	if (opposite != 16)
	{
		rgbVect	v1 = getRGBDirection(RGB_ConsoleColors[nearest], RGB_ConsoleColors[opposite]);
		rgbVect	v2 = getRGBDirection(RGB_ConsoleColors[nearest], RGBInput);

		int	dot = 65536 * getRGBDot(v1, v2);
		dot /= getRGBSquaredNorm(v1);

		// dot is in [0,0.5]

		int blendIndex = (8 * dot) / 32768;
		if (blendIndex > 7)
			blendIndex = 7;

		DisplayCharacter blend = ENCODE_DISPLAY_CHARACTER(opposite, nearest, greyRamp[blendIndex], 0);
		result = blend;

		rgbVect LABBlend1 = RGB_ConsoleColors[nearest];
		rgbVect LABBlend2 = RGB_ConsoleColors[opposite];
		RGBIntMult(&LABBlend1, (16 - blendIndex));
		RGBIntMult(&LABBlend2, blendIndex);
		RGBAdd(&LABBlend1, &LABBlend2);
		RGBIntDiv(&LABBlend1, 16);

		(*error) = LABBlend1;
	}
	else
	{
		DisplayCharacter blend = ENCODE_DISPLAY_CHARACTER(nearest, nearest, ' ', NO_CHARACTER);
		result = blend;

		(*error) = RGB_ConsoleColors[nearest];

	}

	(*error).R = input.R - (*error).R;
	(*error).G = input.G - (*error).G;
	(*error).B = input.B - (*error).B;
	return result;
}

#ifdef LAB_TESTS
DisplayCharacter	GetAsciiArtFromLABExt(RGBPixel input, rgbVect* error)
{
	rgbVect RGBInput = getRGBFromValues(input.R, input.G, input.B);
	rgbVect LABInput = rgb2lab(RGBInput);
	unsigned int nearest = getNearestLABConsoleColor(LABInput);
	unsigned int opposite = getOrientedNearestLABConsoleColor(LABInput, nearest);

	DisplayCharacter result;

	if (opposite != 16)
	{
		// then compute blend in LAB
		rgbVect	v1 = getRGBDirection(RGB_LABConsoleColors[nearest], RGB_LABConsoleColors[opposite]);
		rgbVect	v2 = getRGBDirection(RGB_LABConsoleColors[nearest], LABInput);

		int	dot = 65536 * getRGBDot(v1, v2);
		dot /= getRGBSquaredNorm(v1);

		// dot is in [0,0.5]

		int blendIndex = (8 * dot) / 32768;
		if (blendIndex > 7)
			blendIndex = 7;
	
		DisplayCharacter blend = ENCODE_DISPLAY_CHARACTER(opposite, nearest, greyRamp[blendIndex], 0);
		result = blend;

		rgbVect LABBlend1 = RGB_LABConsoleColors[nearest];
		rgbVect LABBlend2 = RGB_LABConsoleColors[opposite];
		RGBIntMult(&LABBlend1, (16 - blendIndex));
		RGBIntMult(&LABBlend2, blendIndex);
		RGBAdd(&LABBlend1, &LABBlend2);
		RGBIntDiv(&LABBlend1, 16);

		(*error) = lab2rgb(LABBlend1);

	}
	else
	{
		DisplayCharacter blend = ENCODE_DISPLAY_CHARACTER(nearest, nearest, ' ', NO_CHARACTER);
		result = blend;

		(*error) = RGB_ConsoleColors[nearest];

	}

	(*error).R = RGBInput.R - (*error).R;
	(*error).G = RGBInput.G - (*error).G;
	(*error).B = RGBInput.B - (*error).B;

	return result;
}
#endif

void	convertBufferWithDithering(RGBPixel* buffer, int imgSizeX, int imgSizeY, DisplayZone* zone)
{

	for (int j = 0; j < imgSizeY; j++)
	{
		int lineindex = j * imgSizeX;
		for (int i = 0; i < imgSizeX; i++)
		{

			rgbVect	error;
			DisplayCharacter color = GetAsciiArtFromRGBExt(buffer[i + lineindex], &error);
			PrintDisplayCharacterInDisplayZone(zone, color, i, j);

			// "split" error on 4 pixels

			rgbVect splitError[4] = { error,error,error,error };

			RGBIntDiv(&splitError[2],2);
			splitError[3] = splitError[2];
			RGBIntDiv(&splitError[3], 2);
			splitError[1] = splitError[2];
			RGBSub(&splitError[2], &splitError[3]);
			RGBSub(&splitError[1], &splitError[3]);
			RGBSub(&splitError[0], &splitError[3]);
			RGBSub(&splitError[0], &splitError[2]);
			RGBSub(&splitError[0], &splitError[1]);

			if (i < (imgSizeX - 1))
			{
				splitError[0].R += buffer[i + lineindex + 1].R;
				splitError[0].G += buffer[i + lineindex + 1].G;
				splitError[0].B += buffer[i + lineindex + 1].B;
				RGBClamp(&splitError[0]);
				buffer[i + lineindex + 1].R = splitError[0].R;
				buffer[i + lineindex + 1].G = splitError[0].G;
				buffer[i + lineindex + 1].B = splitError[0].B;

				if (j < (imgSizeY - 1))
				{
					splitError[1].R += buffer[i + lineindex + 1 + imgSizeX].R;
					splitError[1].G += buffer[i + lineindex + 1 + imgSizeX].G;
					splitError[1].B += buffer[i + lineindex + 1 + imgSizeX].B;
					RGBClamp(&splitError[1]);
					buffer[i + lineindex + 1 + imgSizeX].R = splitError[1].R;
					buffer[i + lineindex + 1 + imgSizeX].G = splitError[1].G;
					buffer[i + lineindex + 1 + imgSizeX].B = splitError[1].B;
				}
			}

			if (j < (imgSizeY - 1))
			{
				splitError[2].R += buffer[i + lineindex + imgSizeX].R;
				splitError[2].G += buffer[i + lineindex + imgSizeX].G;
				splitError[2].B += buffer[i + lineindex + imgSizeX].B;
				RGBClamp(&splitError[2]);
				buffer[i + lineindex + imgSizeX].R = splitError[2].R;
				buffer[i + lineindex + imgSizeX].G = splitError[2].G;
				buffer[i + lineindex + imgSizeX].B = splitError[2].B;

				if (i >= 1)
				{
					splitError[3].R += buffer[i + lineindex + imgSizeX - 1].R;
					splitError[3].G += buffer[i + lineindex + imgSizeX - 1].G;
					splitError[3].B += buffer[i + lineindex + imgSizeX - 1].B;
					RGBClamp(&splitError[3]);
					buffer[i + lineindex + imgSizeX - 1].R = splitError[3].R;
					buffer[i + lineindex + imgSizeX - 1].G = splitError[3].G;
					buffer[i + lineindex + imgSizeX - 1].B = splitError[3].B;
				}
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
			RGBAPixel pixel[4];

			pixel[0] = getBitmapRGBA(BitmapInfo, bmpdata, i, j);
			pixel[1] = getBitmapRGBA(BitmapInfo, bmpdata, i + 1, j);
			pixel[2] = getBitmapRGBA(BitmapInfo, bmpdata, i, j + 1);
			pixel[3] = getBitmapRGBA(BitmapInfo, bmpdata, i + 1, j + 1);

			DisplayCharacter color = GetDisplayCharacterFrom4Pixels(pixel);
			PrintDisplayCharacterInDisplayZone(resultZone, color, i / 2, j / 2);
		}
	}
	free(BitmapInfo);
	free(bmpdata);

	return resultZone;
}