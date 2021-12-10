#pragma once

#include "DVector.h"

typedef void* (*createObject)(struct ParamSection* init);

typedef struct CFactoryItem
{
	char			mObjectType[32];
	createObject	mCreateFunc;

}CFactoryItem;

typedef struct CFactory
{
	DVector	mFItems;
}CFactory;


void	InitFactory(CFactory* factory);
void	CloseFactory(CFactory* factory);

void	RegisterObject(CFactory* factory, char* objectName, createObject func);

// return a new allocated object of the given type
void* CreateObject(CFactory* factory,char* objectType,struct ParamSection* params);
