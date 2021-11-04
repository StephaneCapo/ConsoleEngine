#pragma once

#include "DVector.h"

// Init input management
void	InitInputs(DVector* inputList);

// push all currently pressed keys to inputList
void	RetrieveKeyboardInputs(DVector* inputList);

// Close input management
void	CloseInputs(DVector* inputList);

// return 1 if given key is press
// else return 0
int IsKeyPress(DVector* inputList, int key);

// return 1 if given key was just released
// else return 0
int WasKeyReleased(DVector* inputList, int key);

// return 1 if a key was released
// else return 0
int WasAnyKeyReleased(DVector* inputList);

// return 1 if given key was just pressed
// else return 0
int KeyPressStart(DVector* inputList, int key);
