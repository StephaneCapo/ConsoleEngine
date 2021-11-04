#include <string.h>
#include "MiniFactory.h"
#include "ParamsList.h"

void	InitFactory(Factory* factory)
{
	factory->mFItems.mBuffer = 0;
	DVectorInit(&factory->mFItems, sizeof(FactoryItem), 0, 0);
}
void	CloseFactory(Factory* factory)
{
	DVectorReset(&factory->mFItems);
}

void	RegisterObject(Factory* factory, char* objecttype, createObject func)
{
	FactoryItem* pushed=(FactoryItem *) DVectorPushBackEmpty(&factory->mFItems);
	pushed->mCreateFunc = func;
	strcpy_s(pushed->mObjectType, 32, objecttype);
}

// return a new allocated object of the given type
void* CreateObject(Factory* factory,char* objectType, ParamSection* params)
{
	int vsize = DVectorSize(&factory->mFItems);

	for (int i = 0; i < vsize; i++)
	{
		FactoryItem* item =(FactoryItem *) DVectorGet(&factory->mFItems, i);

		if (strcmp(item->mObjectType, objectType) == 0)
		{
			return item->mCreateFunc(params);
		}
	}
	return 0;
}
