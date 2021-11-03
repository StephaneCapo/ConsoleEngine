#include <conio.h>
#include "Inputs.h"
#include "DVector.h"

void	InitInputs(DVector* inputList)
{
	DVectorInit(inputList, sizeof(int), 0, 0);
}
void	RetrieveKeyboardInputs(DVector* inputList)
{
    while (_kbhit())
    {
        int test = _getch();
        DVectorPushBack(inputList, &test);
    }
}

// Close input management
void	CloseInputs(DVector* inputList)
{
    DVectorClear(inputList);
}

int toLowerCase(int key)
{
    // if upper case, 
    if ((key >= 65) && (key <= 90))
    {
        key += 97-65;
    }
    return key;
}


// return 1 if given key is pressed
// else return 0
int IsKeyPressed(DVector* inputList, int key)
{
    key = toLowerCase(key);
 
    unsigned int  KeyCount = DVectorSize(inputList);
    for (unsigned int i = 0; i < KeyCount; i++)
    {
        if (toLowerCase(DVectorGetTyped(inputList, int, i)) == key)
        {
            return 1;
        }
    }
    return 0;
}