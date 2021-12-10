#include <string.h>
#include "MiniFactory.h"
#include "ParamsList.h"

void	InitFactory(CFactory* factory)
{
	factory->mFItems.mBuffer = 0;
	DVectorInit(&factory->mFItems, sizeof(CFactoryItem), 0, 0);
}
void	CloseFactory(CFactory* factory)
{
	DVectorReset(&factory->mFItems);
}

void	RegisterObject(CFactory* factory, char* objecttype, createObject func)
{
	CFactoryItem* pushed=(CFactoryItem *) DVectorPushBackEmpty(&factory->mFItems);
	pushed->mCreateFunc = func;
	strcpy_s(pushed->mObjectType, 32, objecttype);
}

// return a new allocated object of the given type
void* CreateObject(CFactory* factory,char* objectType, ParamSection* params)
{
	int vsize = DVectorSize(&factory->mFItems);

	for (int i = 0; i < vsize; i++)
	{
		CFactoryItem* item =(CFactoryItem *) DVectorGet(&factory->mFItems, i);

		if (strcmp(item->mObjectType, objectType) == 0)
		{
			return item->mCreateFunc(params);
		}
	}
	return 0;
}
