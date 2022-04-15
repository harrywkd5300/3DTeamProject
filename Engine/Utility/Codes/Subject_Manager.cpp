#include "..\Headers\Subject_Manager.h"

#include "Observer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CSubject_Manager)

CSubject_Manager::CSubject_Manager()
{
}

list<void*>* CSubject_Manager::GetDataList(int iMessage, SUBJECTTYPE eType)
{
	MAPSUBJECT::iterator iter;

	iter = m_mapData[eType].find(iMessage);

	if (iter == m_mapData[eType].end())
		return NULL;

	return &iter->second;
}

void CSubject_Manager::AddObserver(CObserver * pObserver, SUBJECTTYPE eType)
{
	m_ListObserver[eType].push_back(pObserver);
}

void CSubject_Manager::RemoveObserver(CObserver * pObserver, SUBJECTTYPE eType)
{
	OBSERVERLIST::iterator iter = m_ListObserver[eType].begin();
	OBSERVERLIST::iterator iter_end = m_ListObserver[eType].end();

	for (iter; iter != iter_end; )
	{
		if ((*iter) == pObserver)
		{			
			iter = m_ListObserver[eType].erase(iter);			
		}
		else
			++iter;
	}
}

void CSubject_Manager::Notify(int iMessage, SUBJECTTYPE eType, void * pData)
{
	OBSERVERLIST::iterator iter = m_ListObserver[eType].begin();
	OBSERVERLIST::iterator iter_end = m_ListObserver[eType].end();

	for (iter; iter != iter_end; ++iter)
	{
		(*iter)->Update(iMessage,eType,pData);
	}
}
void CSubject_Manager::AddData(int iMessage, SUBJECTTYPE eType, void * pData)
{
	if (pData != NULL)
	{
		MAPSUBJECT::iterator iter;

		iter = m_mapData[eType].find(iMessage);

		if (iter == m_mapData[eType].end())
			m_mapData[eType].insert(make_pair(iMessage, list<void*>()));

		m_mapData[eType][iMessage].push_back(pData);
		Notify(iMessage,eType,pData);
	}
}

void CSubject_Manager::RemoveData(int iMessage, SUBJECTTYPE eType, void * pData)
{
	MAPSUBJECT::iterator iter;

	iter = m_mapData[eType].find(iMessage);

	if (iter != m_mapData[eType].end())
	{
		list<void*>& ListData = m_mapData[eType][iMessage];

		for (list<void*>::iterator ListIter = ListData.begin();
			ListIter != ListData.end(); )
		{
			if ((*ListIter) == pData)
			{
				ListIter = ListData.erase(ListIter);
			}
			else
				++ListIter;
		}
	}
}

void CSubject_Manager::ReleaseData(SUBJECTTYPE eType)
{
	MAPSUBJECT::iterator iter = m_mapData[eType].begin();
	MAPSUBJECT::iterator iter_end = m_mapData[eType].end();

	for (iter; iter != iter_end; ++iter)
	{
		iter->second.clear();
	}
	m_mapData[eType].clear();
}

void CSubject_Manager::Delete_DynamicData()
{
	MAPSUBJECT::iterator iter = m_mapData[TYPE_DYNAMIC].begin();
	MAPSUBJECT::iterator iter_end = m_mapData[TYPE_DYNAMIC].end();

	for (iter; iter != iter_end; ++iter)
	{
		iter->second.clear();
	}
	m_mapData[TYPE_DYNAMIC].clear();
}

_ulong CSubject_Manager::Free()
{
	for (_uint i = 0; i < TYPE_END; i++)
	{
		m_ListObserver[i].clear();
		
		MAPSUBJECT::iterator iter = m_mapData[i].begin();
		MAPSUBJECT::iterator iter_end = m_mapData[i].end();

		for (iter; iter != iter_end; ++iter)
		{
			iter->second.clear();
		}
		m_mapData[i].clear();		
	}

	return 0;
}
