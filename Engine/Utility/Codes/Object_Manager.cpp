#include "..\Headers\Object_Manager.h"
#include "Layer.h"
//#include "Debug_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
#ifndef _FINAL
	Tag(L"CObject_Manager", std::bind(&CObject_Manager::Debug_Render, this));
#endif

	for (int i = 0; i < OBJ_TYPE_END; ++i)
	{
		if (nullptr != m_pMapLayer[i])
		{
			m_pMapLayer[i] = nullptr;
		}
	}
}


CObject_Manager::~CObject_Manager()
{
}

Engine::CComponent * CObject_Manager::Get_Component(_uint iSceneIdx, const _tchar * pLayerTag, const _uint & iIndex, const _tchar * pComTag)
{
	for (int eType = 0; eType < OBJ_TYPE_END; ++eType)
	{
		CLayer*		pLayer = Find_Layer(iSceneIdx, pLayerTag, eType);
		if (nullptr == pLayer)
			continue;

		return pLayer->Get_Component(iIndex, pComTag);
	}
	return nullptr;
}

_uint CObject_Manager::Get_NumObject(const _uint & iSceneIdx, const _tchar * pLayerTag)
{
	for (int eType = 0; eType < OBJ_TYPE_END; ++eType)
	{
		CLayer*		pLayer = Find_Layer(iSceneIdx, pLayerTag,eType);
		if (nullptr == pLayer)
			continue;

		return pLayer->Get_NumObject();
	}
	return 0;
}

HRESULT CObject_Manager::Reserve_ObjectContainer(const _uint & iMaxSize)
{
	for (int i = 0; i < OBJ_TYPE_END; ++i)
	{
		if (nullptr != m_pMapLayer[i])
		{
			MSG_BOX("Reserve_ResourceContainer Failed");
			return E_FAIL;
		}
	}
	for (int i = 0; i < OBJ_TYPE_END; ++i)
	{
		m_pMapLayer[i] = new MAPLAYER[iMaxSize];
	}

	m_iContainerSize = iMaxSize;

	return NOERROR;
}

HRESULT CObject_Manager::Release_Object(const _uint & iSceneIdx)
{
	if (iSceneIdx >= m_iContainerSize)
		return E_FAIL;

	_ulong		dwRefCnt = 0;
	for (int eType = 0; eType < OBJ_TYPE_END; ++eType)
	{
		for (auto& Pair : m_pMapLayer[eType][iSceneIdx])
		{
			if (dwRefCnt = Safe_Release(Pair.second))
			{
				MSG_BOX("Scene`s Layers Release Failed");
				return E_FAIL;
			}
		}
		m_pMapLayer[eType][iSceneIdx].clear();
	}

	
	return NOERROR;
}

CGameObject * CObject_Manager::Get_FindObject(const _uint & iArrayIdx, const _tchar * pLayerTag, const _uint & iLayerIdx)
{
	for (int eType = 0; eType < OBJ_TYPE_END; ++eType)
	{
		CLayer* pLayer = Find_Layer(iArrayIdx, pLayerTag, eType);

		if (nullptr != pLayer)
		{
			return pLayer->Find_GameObject(iLayerIdx);
		}
	}
	return nullptr;
}

CGameObject * CObject_Manager::Get_FindObject_Near_Position(const _uint & iArrayIdx, const _tchar * pLayerTag, CGameObject* pObj)
{
	for (int eType = 0; eType < OBJ_TYPE_END; ++eType)
	{
		CLayer* pLayer = Find_Layer(iArrayIdx, pLayerTag, eType);

		if (nullptr != pLayer)
		{
			return pLayer->FindObject_Near_Position(pObj);
		}
	}
	return nullptr;
}

CGameObject * CObject_Manager::Get_FindObject_Near_Position( const _uint& iArrayIdx, const _tchar* pLayerTag, CGameObject* pObj,
														 const _float& _fOffsetDegree, const _float& _fSightAngleDegree, const _float& _fSightRange )
{
	for (int eType = 0; eType < OBJ_TYPE_END; ++eType)
	{
		CLayer* pLayer = Find_Layer(iArrayIdx, pLayerTag, eType);

		if (nullptr != pLayer)
		{
			return pLayer->FindObject_Near_Position(pObj, _fOffsetDegree, _fSightAngleDegree, _fSightRange);
		}
	}

	return nullptr;
}

HRESULT CObject_Manager::Ready_GameObject(const _uint & iArrayIdx, const _tchar * pLayerTag, CGameObject * pGameObject, OBJ_TYPE eType)
{
	if (nullptr == m_pMapLayer[eType]
		|| iArrayIdx >= m_iContainerSize)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iArrayIdx, pLayerTag,eType);

	if (nullptr != pLayer)
		pLayer->Add_GameObject(pGameObject);
	else
	{
		if (nullptr == m_pMapLayer[eType])
			return E_FAIL;

		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);

		m_pMapLayer[eType][iArrayIdx].insert(MAPLAYER::value_type(pLayerTag, pLayer));
	}
	if (eType == OBJ_TYPE_STATIC)
		pGameObject->Update_GameObject(1.f);

	return NOERROR;
}

HRESULT CObject_Manager::Ready_Object_Manager(void)
{
	return NOERROR;
}

_int CObject_Manager::Update_Object_Manager(const _float & fTimeDelta)
{
	_int		iExitCode = 0;
	
	Release_DeleteObjectList();

	for (size_t i = 0; i < m_iContainerSize; ++i)
	{
		if (m_pMapLayer[OBJ_TYPE_DYNAMIC][i].empty())
			continue;

		for (auto& Pair : m_pMapLayer[OBJ_TYPE_DYNAMIC][i])
		{
			iExitCode = Pair.second->Update_Layer(fTimeDelta);
			if (iExitCode & 0x80000000)
				return iExitCode;
		}
		
	}
	return iExitCode;
}

_int CObject_Manager::LastUpdate_Object_Manager(const _float & fTimeDelta)
{
	_int		iExitCode = 0;

	for (size_t i = 0; i < m_iContainerSize; ++i)
	{
		if (m_pMapLayer[OBJ_TYPE_DYNAMIC][i].empty())
			continue;

		for (auto& Pair : m_pMapLayer[OBJ_TYPE_DYNAMIC][i])
		{
			iExitCode = Pair.second->LastUpdate_Layer(fTimeDelta);
			if (iExitCode & 0x80000000)
				return iExitCode;
		}

	}
	return iExitCode;
}

HRESULT CObject_Manager::Update_StaticObject()
{
	_int		iExitCode = 0;

	for (_uint i = 0; i < m_iContainerSize; ++i)
	{
		if (m_pMapLayer[OBJ_TYPE_STATIC][i].empty())
			continue;

		for (auto& Pair : m_pMapLayer[OBJ_TYPE_STATIC][i])
		{
			iExitCode = Pair.second->Update_Layer(0.f);
			if (iExitCode & 0x80000000)
				return iExitCode;
		}

	}
	return NOERROR;
}


HRESULT CObject_Manager::Delete_ObjectByKey(const _uint & dwKey)
{
	for (int eType = 0; eType < OBJ_TYPE_END; ++eType)
	{
		for (_uint i = 0; i < m_iContainerSize; ++i)
		{
			MAPLAYER::iterator iter_begin = m_pMapLayer[eType][i].begin();
			MAPLAYER::iterator iter_end = m_pMapLayer[eType][i].end();
			for (; iter_begin != iter_end; ++iter_begin)
			{
				if (!FAILED((*iter_begin).second->Delete_ObjectByKey(dwKey)))
				{
					return NOERROR;
				}
			}
		}
	}
	return E_FAIL;
}

CGameObject * CObject_Manager::Find_ObjectByKey(const _uint & dwKey)
{
	CGameObject* FindObj = nullptr;
	for (int eType = 0; eType < OBJ_TYPE_END; ++eType)
	{
		for (_uint i = 0; i < m_iContainerSize; ++i)
		{
			MAPLAYER::iterator iter_begin = m_pMapLayer[eType][i].begin();
			MAPLAYER::iterator iter_end = m_pMapLayer[eType][i].end();
			for (; iter_begin != iter_end; ++iter_begin)
			{
				FindObj = (*iter_begin).second->Find_ObjectByKey(dwKey);
				if (nullptr != FindObj)
				{
					return FindObj;
				}
			}
		}
	}
	return nullptr;
}

void CObject_Manager::Add_DeleteObject(CGameObject * pDeleteObject)
{
	if (pDeleteObject != nullptr)
	{
		pDeleteObject->Set_IsDead(true);
		m_DeleteObjectList.push_back(pDeleteObject);
	}
}

_ulong CObject_Manager::Release_DeleteObjectList()
{
	_ulong dwRefCnt = 0;

	for (auto pObj : m_DeleteObjectList)
	{
		dwRefCnt += Safe_Release(pObj);
	}
	m_DeleteObjectList.clear();

	return dwRefCnt;
}

CLayer * CObject_Manager::Find_Layer(const _uint & iArrayIdx, const _tchar * pLayerTag, int eType)
{
	if (iArrayIdx >= m_iContainerSize)
		return nullptr;
	//for (int i = 0; i < OBJ_TYPE_END; ++i)
	//{
	//	if (nullptr == m_pMapLayer[i])
	//	{
	//		return nullptr;
	//	}
	//}
	//for (int eType = 0; eType < OBJ_TYPE_END; ++eType)
	//{
	auto iter = find_if(m_pMapLayer[eType][iArrayIdx].begin(), m_pMapLayer[eType][iArrayIdx].end(), CFind_Tag(pLayerTag));

	if (iter == m_pMapLayer[eType][iArrayIdx].end())
		return nullptr;//continue;

	return iter->second;
	//}

	//return nullptr;
	
}
#ifndef _FINAL
CLayer * CObject_Manager::Find_Layer_ForMapTool(const _uint & iArrayIdx, const _tchar * pLayerTag, int eType)
{
	auto iter = find_if(m_pMapLayer[eType][iArrayIdx].begin(), m_pMapLayer[eType][iArrayIdx].end(), CFind_Tag(pLayerTag));

	if (iter == m_pMapLayer[eType][iArrayIdx].end())
		return nullptr;//continue;

	return iter->second;
}

void CObject_Manager::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CObject_Manager");

	//int iCount = 0;
	int Count = 0;
	for (int i=0; i<m_iContainerSize; ++i)
	{
		for (auto pLayer : m_pMapLayer[OBJ_TYPE_DYNAMIC][i])
		{
			CDebug::GetInstance()->Add_DebugText(L"%c) %s", CDebug::DisplayKey[Count], pLayer.first);
			//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
			if (GetAsyncKeyState(CDebug::Key[Count]) & 0x0001)
			{
				CDebug::GetInstance()->Set_CurDebug(std::bind(&CLayer::Debug_Render, pLayer.second), pLayer.second);
			}
			++Count;
		}
	}
}
#endif
_ulong CObject_Manager::Free(void) //By.Jang
{
	_ulong dwRefCnt = 0;

	//for (size_t i = 0; i < m_iContainerSize; i++)
	//{
	//	for_each(m_pMapLayer[i].begin(), m_pMapLayer[i].end(), CRelease_Pair());
	//	m_pMapLayer[i].clear();
	//}
	//Safe_Delete_Array(m_pMapLayer);
	//



	for (int eType = 0; eType < OBJ_TYPE_END; ++eType)
	{
		for (size_t i = 0; i < m_iContainerSize; i++)
		{

			for (auto pLayer : m_pMapLayer[eType][i])
			{
				dwRefCnt += Safe_Release(pLayer.second);
			}

			m_pMapLayer[eType][i].clear();
		}
		Safe_Delete_Array(m_pMapLayer[eType]);
	}

	dwRefCnt += Release_DeleteObjectList();

	return dwRefCnt;
}
