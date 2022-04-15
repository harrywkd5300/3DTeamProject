#include "..\Headers\Camera_Manager.h"

#include "Camera.h"
#include "Renderer.h"
#include "CameraLayer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CCamera_Manager)


CCamera_Manager::CCamera_Manager()
	:m_pCurrentCamera(nullptr)	
{
#ifndef _FINAL
	Tag(L"CCamera_Manager", std::bind(&CCamera_Manager::Debug_Render, this));
#endif
}

CCamera_Manager::~CCamera_Manager()
{
}
_uint CCamera_Manager::Get_NumCamera(const _uint & iSceneIdx, const _tchar * pLayerTag)
{
	CCameraLayer*		pLayer = Find_Layer(iSceneIdx, pLayerTag);
	if (nullptr == pLayer)
		return 0;

	return pLayer->Get_NumCamera();
}
HRESULT CCamera_Manager::Reserve_CameraContainer(const _uint & iMaxSize)
{
	if (nullptr != m_pMapLayer)
	{
		MSG_BOX("Reserve_ResourceContainer Failed");
		return E_FAIL;
	}

	m_pMapLayer = new MAPLAYER[iMaxSize];

	m_iContainerSize = iMaxSize;

	return NOERROR;
}

HRESULT CCamera_Manager::Release_Camera(const _uint & iSceneIdx)
{
	if (iSceneIdx >= m_iContainerSize)
		return E_FAIL;

	_ulong		dwRefCnt = 0;
	for (auto& Pair : m_pMapLayer[iSceneIdx])
	{
		if (dwRefCnt = Safe_Release(Pair.second))
		{
			MSG_BOX("Scene`s Layers Release Failed");
			return E_FAIL;
		}
	}

	m_pMapLayer[iSceneIdx].clear();

	return NOERROR;
}

CCamera * CCamera_Manager::Get_FindCamera(const _uint & iArrayIdx, const _tchar * pLayerTag, const _uint & iLayerIdx)
{
	CCameraLayer* pLayer = Find_Layer(iArrayIdx, pLayerTag);

	if (nullptr != pLayer)
	{
		return pLayer->Find_Camera(iLayerIdx);
	}

	return nullptr;
	
}

CCamera * CCamera_Manager::Get_CurrentCamera()
{
	if (nullptr == m_pCurrentCamera)
		return nullptr;

	return m_pCurrentCamera;
}

HRESULT CCamera_Manager::Set_CurrentCamera(CCamera * pCamera)
{
	if (nullptr == pCamera)
		return E_FAIL;

	if (m_pCurrentCamera == pCamera)
		return NOERROR;


	if (nullptr != m_pCurrentCamera)
		m_pCurrentCamera->Set_Operation(false);

	m_pCurrentCamera = pCamera;

	m_pCurrentCamera->Set_Operation(true);	

	m_pCurrentCamera->Update_GameObject(0.f);

	return NOERROR;
}

HRESULT CCamera_Manager::SetUp_Renderer(CRenderer * pRenderer)
{
	if (pRenderer == nullptr)
		return E_FAIL;
	m_pRenderer = pRenderer;
	m_pRenderer->AddRef();
	return NOERROR;
}

HRESULT CCamera_Manager::Add_Camera(const _uint & iArrayIdx, const _tchar* pLayerTag, CCamera * pCamera)
{
	if (nullptr == m_pMapLayer
		|| iArrayIdx >= m_iContainerSize)
		return E_FAIL;

	CCameraLayer* pLayer = Find_Layer(iArrayIdx, pLayerTag);

	if (nullptr != pLayer)
		pLayer->Add_Camera(pCamera);
	else
	{
		if (nullptr == m_pMapLayer)
			return E_FAIL;

		pLayer = CCameraLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_Camera(pCamera);

		m_pMapLayer[iArrayIdx].insert(MAPLAYER::value_type(pLayerTag, pLayer));
	}

	return NOERROR;
}

HRESULT CCamera_Manager::Reserve_NextCamera(CCamera * pCamera)
{
	m_ReserveList.push_back(pCamera);

	return NOERROR;
}

HRESULT CCamera_Manager::Ready_Camera_Manager(void)
{
	return NOERROR;
}

_int CCamera_Manager::Update_Camera_Manager(const _float & fTimeDelta)
{
	_int		iExitCode = 0;

	for (size_t i = 0; i < m_iContainerSize; ++i)
	{
		if (m_pMapLayer[i].empty())
			continue;

		for (auto& Pair : m_pMapLayer[i])
		{
			iExitCode = Pair.second->Update_Layer(fTimeDelta);
			if (iExitCode & 0x80000000)
				return iExitCode;
		}

	}
	
	if (m_pRenderer != nullptr
		&& m_pCurrentCamera != nullptr
		)
	{
		m_pRenderer->Update_ViewMatrix(m_pCurrentCamera->Get_ViewMatrix());
		m_pRenderer->Update_ProjMatrix(m_pCurrentCamera->Get_ProjMatrix());
	}
	
	return iExitCode;
}

_int CCamera_Manager::LastUpdate_Camera_Manager(const _float & fTimeDelta)
{
	_int		iExitCode = 0;

	for (size_t i = 0; i < m_iContainerSize; ++i)
	{
		if (m_pMapLayer[i].empty())
			continue;

		for (auto& Pair : m_pMapLayer[i])
		{
			iExitCode = Pair.second->LastUpdate_Layer(fTimeDelta);
			if (iExitCode & 0x80000000)
				return iExitCode;
		}

	}
	return iExitCode;
}

_vec3 CCamera_Manager::Get_WorldPosition()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _vec3();
	} 

	return m_pCurrentCamera->Get_WorldPosition();
}

_matrix CCamera_Manager::Get_WorldMatrix()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _matrix();
	}

	return m_pCurrentCamera->Get_WorldMatrix();
}

_matrix CCamera_Manager::Get_ProjMatrix()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _matrix();
	}
	return m_pCurrentCamera->Get_ProjMatrix();	
}

_matrix CCamera_Manager::Get_ViewMatrix()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _matrix();
	}
	return m_pCurrentCamera->Get_ViewMatrix();
}

_matrix CCamera_Manager::Get_OldProjMatrix()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _matrix();
	}
	return m_pCurrentCamera->Get_OldProjMatrix();
}

_matrix CCamera_Manager::Get_OldViewMatrix()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _matrix();
	}
	return m_pCurrentCamera->Get_OldViewMatrix();
}

_matrix CCamera_Manager::Get_InverseProjMatrix()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _matrix();
	}
	return m_pCurrentCamera->Get_InverseProjMatrix();
}

_matrix CCamera_Manager::Get_InverseViewMatrix()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _matrix();
	}
	return m_pCurrentCamera->Get_InverseViewMatrix();
}
_float CCamera_Manager::Get_Far()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return 0.f;
	}
	return m_pCurrentCamera->Get_Far();		
}
_vec3 CCamera_Manager::Get_Direction()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _vec3();
	}

	return m_pCurrentCamera->Get_Direction();
}
_vec3 CCamera_Manager::Get_Direction_NotY()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _vec3();
	}

	return m_pCurrentCamera->Get_Direction_NotY();
}
_vec3 CCamera_Manager::Get_At()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _vec3();
	}

	return m_pCurrentCamera->Get_At();
}
_vec3 CCamera_Manager::Get_Eye()
{
	if (nullptr == m_pCurrentCamera)
	{
		MSG_BOX("m_pCurrentCamera == nullptr !!");
		return _vec3();
	}

	return m_pCurrentCamera->Get_Eye();
}
CCameraLayer * CCamera_Manager::Find_Layer(const _uint & iArrayIdx, const _tchar * pLayerTag)
{
	if (nullptr == m_pMapLayer
		|| iArrayIdx >= m_iContainerSize)
		return nullptr;

	auto iter = find_if(m_pMapLayer[iArrayIdx].begin(), m_pMapLayer[iArrayIdx].end(), CFind_Tag(pLayerTag));

	if (iter == m_pMapLayer[iArrayIdx].end())
		return nullptr;

	return iter->second;
}
#ifndef _FINAL
void CCamera_Manager::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CCamera_Manager");

	//int iCount = 0;
	int Count = 0;
	for (int i = 0; i<m_iContainerSize; ++i)
	{
		for (auto pLayer : m_pMapLayer[i])
		{
			CDebug::GetInstance()->Add_DebugText(L"%c) %s", CDebug::DisplayKey[Count], pLayer.first);
			//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
			if (GetAsyncKeyState(CDebug::Key[Count]) & 0x0001)
			{
				CDebug::GetInstance()->Set_CurDebug(std::bind(&CCameraLayer::Debug_Render, pLayer.second), pLayer.second);
			}
			++Count;
		}
	}
}
#endif
_ulong CCamera_Manager::Free(void)
{
	_ulong dwRefCnt = 0;	

	m_pCurrentCamera = nullptr;

	for (size_t i = 0; i < m_iContainerSize; i++)
	{
		for_each(m_pMapLayer[i].begin(), m_pMapLayer[i].end(), CRelease_Pair());
		m_pMapLayer[i].clear();
	}
	Safe_Delete_Array(m_pMapLayer);
	Safe_Release(m_pRenderer);

	return dwRefCnt;
}
