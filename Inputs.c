#include <conio.h>
#include <Windows.h>
#include "Inputs.h"
#include "DVector.h"

#define IS_PRESS_BIT        256
#define WAS_PRESSED_BIT     512
#define WAS_RELEASED_BIT    1024

void	InitInputs(DVector* inputList)
{
	DVectorInit(inputList, sizeof(int), 0, 0);
}
void	RetrieveKeyboardInputs(DVector* inputList)
{
    SHORT    keys[256];
    memset(keys, 0, 256 * sizeof(SHORT));
    // set the key current status
    for (int i = 0; i < DVectorSize(inputList); i++)
    {
        int key = DVectorGetTyped(inputList, int, i);
        unsigned char keyCode = key & 255;
        if (key & IS_PRESS_BIT)
        {
            keys[keyCode] = IS_PRESS_BIT;
        }
    }

    DVectorClear(inputList);

    //check all the status if any key is pressed
    for (int i = 0; i < 256; i++) {
        SHORT key = GetAsyncKeyState(i);
        if (key & 0x8000) // key is pressed
        {
            if (keys[i] != IS_PRESS_BIT) // was not pressed before
            {
                keys[i] = IS_PRESS_BIT | WAS_PRESSED_BIT;
            }
        }
        else if (keys[i] == IS_PRESS_BIT) // was pressed
        {
            keys[i] = WAS_RELEASED_BIT;
        }
    }

    for (int i = 0; i < 256; i++)
    {
        if (keys[i])
        {
            DVectorPushBackTyped(inputList, int,keys[i]|i);
        }
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
int IsKeyPress(DVector* inputList, int key)
{
    key = toLowerCase(key);
 
    unsigned int  KeyCount = DVectorSize(inputList);
    for (unsigned int i = 0; i < KeyCount; i++)
    {
        int keyState = DVectorGetTyped(inputList, int, i);
        if (((toLowerCase(keyState&255)) == key) && (keyState& IS_PRESS_BIT))
        {
            return 1;
        }
    }
    return 0;
}

// return 1 if given key was just released
// else return 0
int WasKeyReleased(DVector* inputList, int key)
{
    key = toLowerCase(key);

    unsigned int  KeyCount = DVectorSize(inputList);
    for (unsigned int i = 0; i < KeyCount; i++)
    {
        int keyState = DVectorGetTyped(inputList, int, i);
        if (((toLowerCase(keyState & 255)) == key) && (keyState & WAS_RELEASED_BIT))
        {
            return 1;
        }
    }
    return 0;
}

int WasAnyKeyReleased(DVector* inputList)
{
    
    unsigned int  KeyCount = DVectorSize(inputList);
    for (unsigned int i = 0; i < KeyCount; i++)
    {
        int keyState = DVectorGetTyped(inputList, int, i);
        if (keyState & WAS_RELEASED_BIT)
        {
            return 1;
        }
    }
    return 0;
}

// return 1 if given key was just pressed
// else return 0
int KeyPressStart(DVector* inputList, int key)
{
    key = toLowerCase(key);

    unsigned int  KeyCount = DVectorSize(inputList);
    for (unsigned int i = 0; i < KeyCount; i++)
    {
        int keyState = DVectorGetTyped(inputList, int, i);
        if (((toLowerCase(keyState & 255)) == key) && (keyState & WAS_PRESSED_BIT))
        {
            return 1;
        }
    }
    return 0;
}