#pragma once

#include "DVector.h"

// Init input management
void	InitInputs(DVector* inputList);

// push all currently pressed keys to inputList
void	RetrieveKeyboardInputs(DVector* inputList);

// Close input management
void	CloseInputs(DVector* inputList);

// return 1 if given key is pressed
// else return 0
int IsKeyPressed(DVector* inputList, int key);
