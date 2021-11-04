#pragma once

#include "DVector.h"

typedef void* (*createObject)(struct ParamSection* init);

typedef struct FactoryItem
{
	char			mObjectType[32];
	createObject	mCreateFunc;

}FactoryItem;

typedef struct Factory
{
	DVector	mFItems;
}Factory;


void	InitFactory(Factory* factory);
void	CloseFactory(Factory* factory);

void	RegisterObject(Factory* factory, char* objectName, createObject func);

// return a new allocated object of the given type
void* CreateObject(Factory* factory,char* objectType,struct ParamSection* params);
