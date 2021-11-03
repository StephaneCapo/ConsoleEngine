#include <time.h>
#include <Windows.h>
#include "TimeManagement.h"

double gInitTime;

// return current spent time in application
double	GetTime()
{
	return (GetTickCount64() / 1000.0) - gInitTime;
}

void	InitTime()
{
	gInitTime = (GetTickCount64() / 1000.0);
}