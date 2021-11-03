#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DVector.h"

#define elemAtPos(darray,pos) ((void*)(((char*)darray->mBuffer)+(pos)*darray->mStructSize))

void DVectorReset(DVector* darray)
{
	if (darray->mBuffer)
	{
		free(darray->mBuffer);
	}
	darray->mBuffer = NULL;
	darray->mCurrentSize = darray->mReservedSize = 0;
}


DVector* DVectorCreate()
{
	DVector* result = malloc(sizeof(DVector));
	if (result)
	{
		result->mBuffer = NULL;
		result->mCurrentSize = 0;
		result->mReservedSize = 0;
		result->mStructSize = 0;
	}
	return result;
}

DVector* DVectorDestroy(DVector* darray)
{
	if (darray)
	{
		DVectorReset(darray);
		free(darray);
	}
	return NULL;
}

void DVectorInit(DVector* darray,unsigned int structSize, unsigned int len, void* defaultVal)
{
	if (darray->mBuffer)
	{
		// free buffer, but don't free darray
		DVectorReset(darray);
	}

	if (structSize == 0)
	{
#ifdef _DEBUG
		printf("Cannot init DVector with structSize 0\n");
#endif
	}

	darray->mCurrentSize = len;
	darray->mReservedSize = len;
	// reserve 4 
	if(len<4)
		darray->mReservedSize = 4;

	darray->mStructSize = structSize;
	
	darray->mBuffer = malloc(structSize * darray->mReservedSize);

	// if defaultVal is not NULL init vector
	if (defaultVal)
	{
		char* initV = (char*)darray->mBuffer;

		for (int i = 0; i < darray->mCurrentSize; i++)
		{
			memcpy(initV, defaultVal, structSize);
			initV += structSize;
		}
	}

}

unsigned int		DVectorSize(DVector* darray)
{
	return darray->mCurrentSize;
}

void*	DVectorPushBack(DVector* darray, void* toAdd)
{
	darray->mCurrentSize++;
	// check if I need to reserve more memory
	if (darray->mCurrentSize > darray->mReservedSize)
	{
		unsigned int toReserve = darray->mReservedSize;
		if (toReserve < 256)
		{
			toReserve *= 2;
		}
		else
		{
			toReserve += 256;
		}
		DVectorReserve(darray, toReserve);
	}

	memcpy(elemAtPos(darray, darray->mCurrentSize - 1), toAdd, darray->mStructSize);

	return elemAtPos(darray, darray->mCurrentSize - 1);
}
void* DVectorPushBackEmpty(DVector* darray)
{
	darray->mCurrentSize++;
	// check if I need to reserve more memory
	if (darray->mCurrentSize > darray->mReservedSize)
	{
		unsigned int toReserve = darray->mReservedSize;
		if (toReserve < 256)
		{
			toReserve *= 2;
		}
		else
		{
			toReserve += 256;
		}
		DVectorReserve(darray, toReserve);
	}
	return elemAtPos(darray, darray->mCurrentSize - 1);
}
void	DVectorPopBack(DVector* darray)
{
	if(darray->mCurrentSize)
		darray->mCurrentSize--;

	// check if we should resize allocated memory
	if (darray->mCurrentSize < (darray->mReservedSize / 2))
	{
		unsigned int newReserved = (darray->mCurrentSize + darray->mReservedSize) / 2;
		DVectorReserve(darray, newReserved);
	}
}

void* DVectorBack(DVector* darray)
{
	if (darray->mCurrentSize)
	{
		return DVectorGet(darray, darray->mCurrentSize - 1);
	}
	return NULL;
}

int	DVectorInsert(DVector* darray, void* toInsert, unsigned int index)
{
	if (index >= darray->mCurrentSize)
	{
		return 0;
	}
	// use push back to manage vector growing
	DVectorPushBack(darray, toInsert);

	// compute size of buffer to move
	unsigned int moveSize = (darray->mCurrentSize - index -1) * darray->mStructSize;
	
	// then move values after selected index
	void* srcPos = elemAtPos(darray, index);
	void* dstPos = elemAtPos(darray, index+1);
	memmove(dstPos, srcPos, moveSize);
	
	// finaly copy toInsert at given index
	memcpy(srcPos, toInsert, darray->mStructSize);

	return 1;
}

int DVectorErase(DVector* darray, unsigned int index)
{
	if (index >= darray->mCurrentSize)
	{
		return 0;
	}

	// compute size of buffer to move
	unsigned int moveSize = ((darray->mCurrentSize-1) - index) * darray->mStructSize;

	// then move values after selected index
	void* srcPos = elemAtPos(darray, index + 1);
	void* dstPos = elemAtPos(darray, index);
	memmove(dstPos, srcPos, moveSize);

	// then popback
	DVectorPopBack(darray);

	return 1;
}

void	DVectorResize(DVector* darray, unsigned int newLen, void* defaultVal)
{
	// if needs to reserve more
	if (newLen > darray->mReservedSize)
	{
		DVectorReserve(darray, newLen);
	}

	if (newLen > darray->mCurrentSize)
	{
		// copy defaultvalue at empty places
		// if defaultVal is not NULL 
		if (defaultVal)
		{
			char* initV =(char*) elemAtPos(darray,darray->mCurrentSize);

			for (int i = darray->mCurrentSize; i < newLen; i++)
			{
				memcpy(initV, defaultVal, darray->mStructSize);
				initV += darray->mStructSize;
			}
		}
	}
	
	darray->mCurrentSize = newLen;

}


void	DVectorReserve(DVector* darray, unsigned int newReservedSize)
{
	// can not reserve less than array size
	if (newReservedSize < darray->mCurrentSize)
	{
		newReservedSize = darray->mCurrentSize;
	}
	
	// reserve at least 4
	if (newReservedSize < 4)
	{
		newReservedSize = 4;
	}

	if (newReservedSize != darray->mReservedSize)
	{
		darray->mReservedSize = newReservedSize;
		darray->mBuffer = realloc(darray->mBuffer, newReservedSize * darray->mStructSize);
	}
}

// clear don't change reserved size
void	DVectorClear(DVector* darray)
{
	darray->mCurrentSize = 0;
}


void* DVectorGet(DVector* darray, unsigned int index)
{
	if (index >= darray->mCurrentSize)
	{
		return NULL;
	}

	return elemAtPos(darray,index);
}

// replace element at given index
void	DVectorSet(DVector* darray, unsigned int index, void* toReplace)
{
	if (index < darray->mCurrentSize)
	{
		void* dst = elemAtPos(darray, index);
		memcpy(dst, toReplace, darray->mStructSize);
	}
}

void* DVectorData(DVector* darray)
{
	return darray->mBuffer;
}