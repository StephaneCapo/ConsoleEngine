#pragma once

// some utili
#define DVectorPushBackTyped(darray,type,value) \
{ \
	type toPush=value; \
	DVectorPushBack(darray,&toPush);\
}

#define DVectorGetTyped(darray,type,i) \
	*(type*)DVectorGet(darray,i)

#define DVectorBackTyped(darray,type) \
	*(type*)DVectorBack(darray)

#define DVectorInitTyped(darray,type,defaultvalue,elemCount) \
{ \
	type toPush=defaultvalue; \
	DVectorInit(darray,sizeof(type),elemCount,&toPush);\
}

#define DVectorInsertTyped(darray,type,value,i) \
{ \
	type toPush=value; \
	DVectorInsert(darray,&toPush,i);\
}

#define DVectorResizeTyped(darray,type,defaultvalue,elemCount) \
{ \
	type toPush=defaultvalue; \
	DVectorResize(darray,elemCount,&toPush);\
}

typedef struct DVector
{
	unsigned int	mCurrentSize;
	unsigned int	mReservedSize;
	unsigned int	mStructSize;
	void*			mBuffer;
}DVector;

// create an empty DVector
DVector* DVectorCreate();

// destroy a DVector 
DVector* DVectorDestroy(DVector* darray);

// init a DVector
void	DVectorInit(DVector* darray,unsigned int structSize,unsigned int len,void* defaultVal);

// return size of DVector
unsigned int		DVectorSize(DVector* darray);
// increment size of the array and add an element at the end of the array 
// return pointer on added element
void*	DVectorPushBack(DVector* darray, void* toAdd);
// increment size of the array  
// return pointer on empty added element
void* DVectorPushBackEmpty(DVector* darray);
// decrement array size ( erase last element )
void	DVectorPopBack(DVector* darray);
// get a pointer on last element
void*	DVectorBack(DVector* darray);
// increment size of the array and add an element at the given index
// return 0 if out of range error
int	DVectorInsert(DVector* darray, void* toInsert, unsigned int index);
// decrement size of the array and erase element at index
// return 0 if out of range error
int	DVectorErase(DVector* darray, unsigned int index);
// resize array
void	DVectorResize(DVector* darray, unsigned int newLen, void* defaultVal);
// reserve array
void	DVectorReserve(DVector* darray, unsigned int newReservedSize);
// clear array
void	DVectorClear(DVector* darray);
// get element at index
void*	DVectorGet(DVector* darray, unsigned int index);
// get raw buffer
void*	DVectorData(DVector* darray);
// free buffer and set everything to 0
void DVectorReset(DVector* darray);
// replace element at given index
void	DVectorSet(DVector* darray, unsigned int index, void* toReplace);


