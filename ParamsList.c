#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ParamsList.h"
#include "ini.h"

void GetParamElemInt(ParamSection* Section, int* stat, char* ParamName)
{
	ParamElem* elem = GetParamInSection(Section, ParamName);
	if (elem)
	{
		*stat = ((ParamInt*)elem)->mValue;
	}
}

void GetParamElemFloat(ParamSection* Section, float* stat, char* ParamName)
{
	ParamElem* elem = GetParamInSection(Section, ParamName);
	if (elem)
	{
		*stat = ((ParamFloat*)elem)->mValue;
	}
}

void GetParamElemString(ParamSection* Section, char* destString, int len, char* ParamName)
{
	ParamElem* elem = GetParamInSection(Section, ParamName);
	if (elem)
	{
		strcpy_s(destString, len, ((ParamString*)elem)->mValue);
	}
}


ParamSection* GetSection(ParamsList* list, const char* name)
{
	int vsize = DVectorSize(&list->mSectionList);
	for (unsigned int i = 0; i < vsize; i++)
	{
		ParamSection* section = DVectorGet(&list->mSectionList, i);
		if (strcmp(section->mSectionName, name) == 0)
		{
			return section;
		}
	}

	return NULL;
}
ParamElem* GetParamInSection(ParamSection* section, const char* paramname)
{
	int arraySize = DVectorSize(&section->mParamList);
	ParamElem** elementsArray = DVectorData(&section->mParamList);
	for (int i = 0; i < arraySize; i++)
	{
		if (strcmp(elementsArray[i]->mParamName, paramname) == 0)
		{
			return elementsArray[i];
		}
	}
	return NULL;
}
ParamSection* AddSection(ParamsList* list, const char* name)
{
	// check that this section already exist
	if (GetSection(list, name) != NULL)
	{
		// return already existing one
		return GetSection(list, name);
	}
	// push a new section
	ParamSection toAdd;
	DVectorPushBack(&list->mSectionList, &toAdd);
	// get the newly added section
	ParamSection* added = DVectorBack(&list->mSectionList);
	added->mParamList.mBuffer = NULL;
	// init mParamList DVector
	DVectorInit(&added->mParamList, sizeof(ParamElem*), 0, 0);
	// copy name
	strcpy_s(added->mSectionName, 32, name);

	return added;
}
void				AddParamElem(ParamSection* section, ParamElem* elem)
{
	ParamElem* testExisting = GetParamInSection(section, elem->mParamName);
	if (testExisting)
	{
		printf("Can not add already existing element\n");
		return;
	}
	DVectorPushBack(&section->mParamList, &elem);
}


// clean all allocations
void				ClearParamList(ParamsList* list)
{
	int vsize = DVectorSize(&list->mSectionList);
	for (unsigned int i = 0; i < vsize; i++)
	{
		ParamSection* section = DVectorGet(&list->mSectionList, i);
		int arraySize = DVectorSize(&section->mParamList);
		
		ParamElem** elementsArray = DVectorData(&section->mParamList);
		for (int j = 0; j < arraySize; j++)
		{
			// free each ParamElem
			free(elementsArray[j]);
		}
		DVectorReset(&section->mParamList);
	}

	DVectorReset(&list->mSectionList);

}

int insertParamFromIni(void* user, const char* sectionname, const char* name, const char* value)
{
	ParamsList* newlist =(ParamsList *) user;
	ParamSection* section=GetSection(newlist, sectionname);

	if (section == NULL)
	{
		section=AddSection(newlist, sectionname);
	}

	ParamElem* param=GetParamInSection(section, name);

	if (param == NULL)
	{
		if (value[0] == '"') // suppose string
		{
			ParamString* ps = malloc(sizeof(ParamString));
			ps->mLink.mType = STRING_PARAM;
			ps->mLink.mParamName[31] = 0;
			// copy name
			strncpy_s(ps->mLink.mParamName,32, name, 31);
			strncpy_s(ps->mValue,1000, value+1,strlen(value)-2);
			param = (ParamElem*)ps;
		}
		else if (strchr(value,'.')) // suppose float
		{
			ParamFloat *pf= malloc(sizeof(ParamFloat));
			pf->mLink.mType = FLOAT_PARAM;
			pf->mLink.mParamName[31] = 0;
			// copy name
			strncpy_s(pf->mLink.mParamName,32, name, 31);
			sscanf_s(value, "%f", &pf->mValue,32);
			param = (ParamElem*)pf;
		}
		else
		{
			ParamInt* pi = malloc(sizeof(ParamInt));
			pi->mLink.mType = INT_PARAM;
			pi->mLink.mParamName[31] = 0;
			// copy name
			strncpy_s(pi->mLink.mParamName,32, name, 31);
			sscanf_s(value, "%d", &pi->mValue,32);
			param = (ParamElem*)pi;
		}

		AddParamElem(section, param);
	}
	else
	{
		// parameter already exist ! do nothing
	}

	return 0;

}


ParamsList* InitParamListFromIniFile(const char* fname)
{
	ParamsList* newlist =(ParamsList *) malloc(sizeof(ParamsList));
	newlist->mSectionList.mBuffer = NULL;

	DVectorInit(&newlist->mSectionList, sizeof(ParamSection), 0, 0);
	ini_parse(fname, insertParamFromIni, newlist);

	return newlist;
}
