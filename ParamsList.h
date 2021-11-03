#pragma once
#include "DVector.h"

typedef enum ParamType
{
	STRING_PARAM =	0,
	INT_PARAM =		1,
	FLOAT_PARAM =	2
}ParamType;


typedef struct ParamElem
{
	char		mParamName[32];
	ParamType	mType;
}ParamElem;

typedef struct ParamInt
{
	ParamElem	mLink;
	int			mValue;
}ParamInt;

typedef struct ParamFloat
{
	ParamElem	mLink;
	float		mValue;
}ParamFloat;

typedef struct ParamString
{
	ParamElem	mLink;
	char		mValue[1000];
}ParamString;

typedef struct ParamSection
{
	char		mSectionName[32];
	// vector of pointer on ParamElem
	DVector		mParamList;
}ParamSection;


typedef struct ParamsList
{
	// vector of section
	DVector		mSectionList;
}ParamsList;

inline ParamType			GetParamType(ParamElem* elem)
{
	return elem->mType;
}

void				GetParamElemInt(ParamSection* Section, int* stat, char* ParamName);
void				GetParamElemFloat(ParamSection* Section, float* stat, char* ParamName);
void				GetParamElemString(ParamSection* Section, char* destString, int len, char* ParamName);
ParamSection*		GetSection(ParamsList* list,const char* name);
ParamElem*			GetParamInSection(ParamSection* section, const char* paramname);
ParamSection*		AddSection(ParamsList* list, const char* name);
void				AddParamElem(ParamSection* section, ParamElem* elem);
// clean all allocations
void				ClearParamList(ParamsList* list);

ParamsList*			 InitParamListFromIniFile(const char* fname);


