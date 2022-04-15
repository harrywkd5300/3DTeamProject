#include "..\Headers\CameraLayer.h"

#include "Camera.h"

USING(Engine)

CCameraLayer::CCameraLayer()
{
#ifndef _FINAL
	Tag(L"CCameraLayer", std::bind(&CCameraLayer::Debug_Render, this));
#endif

}

Engine::CComponent * CCameraLayer::Get_Component(const _uint & iIndex, const _tchar * pComTag)
{
	_uint iSize = _uint(m_CameraList.size());

	if (iIndex >= iSize)
		return nullptr;

	auto iter = m_CameraList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_CameraList.end())
		return nullptr;


	return (*iter)->Get_Component(pComTag);
}

HRESULT CCameraLayer::Add_Camera(CCamera * pCamera)
{
	if (nullptr == pCamera)
		return E_FAIL;

	m_CameraList.push_back(pCamera);

	return NOERROR;
}

CCamera * CCameraLayer::Find_Camera(const _uint & iLayerIdx)
{
	_uint iSize = _uint(m_CameraList.size());

	if (iLayerIdx >= iSize)
		return nullptr;

	auto iter = m_CameraList.begin();

	for (size_t i = 0; i < iLayerIdx; ++i)
		++iter;

	if (iter == m_CameraList.end())
		return nullptr;

	return (*iter);
}

HRESULT CCameraLayer::Ready_Layer(void)
{
	return NOERROR;
}

_int CCameraLayer::Update_Layer(const _float & fTimeDelta)
{
	_int iExitCode = 0;

	auto& iter = m_CameraList.begin();

	for (; iter != m_CameraList.end(); )
	{
		iExitCode = (*iter)->Update_GameObject(fTimeDelta);

		if (1 == iExitCode)	/*Object Is Dead*/
		{
			Safe_Release((*iter));
			iter = m_CameraList.erase(iter);
		}
		else if (iExitCode & 0x80000000) /* exit code < 0*/
		{
			return iExitCode;
		}
		else
			++iter;
	}

	return iExitCode;
}

_int CCameraLayer::LastUpdate_Layer(const _float & fTimeDelta)
{
	_int iExitCode = 0;

	auto& iter = m_CameraList.begin();

	for (; iter != m_CameraList.end(); )
	{
		iExitCode = (*iter)->LastUpdate_GameObject(fTimeDelta);

		if (1 == iExitCode)	/*Object Is Dead*/
		{
			Safe_Release((*iter));
			iter = m_CameraList.erase(iter);
		}
		else if (iExitCode & 0x80000000) /* exit code < 0*/
		{
			return iExitCode;
		}
		else
			++iter;
	}

	return iExitCode;
}

void CCameraLayer::Render_Layer(void)
{
	for (auto& pCamera : m_CameraList)
		pCamera->Render_GameObject();
}

CCamera * CCameraLayer::Find_ObjectByKey(const _uint & dwKey)
{
	CAMERALIST::iterator iter_begin = m_CameraList.begin();
	CAMERALIST::iterator iter_end = m_CameraList.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		if ((*iter_begin)->GetKey() == dwKey)
		{
			return (*iter_begin);
		}
	}

	return nullptr;
}

HRESULT CCameraLayer::Delete_ObjectByKey(const _uint & pKey)
{
	CAMERALIST::iterator iter_begin = m_CameraList.begin();
	CAMERALIST::iterator iter_end = m_CameraList.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		if ((*iter_begin)->GetKey() == pKey)
		{
			//Safe_Release(*iter_begin);
			(*iter_begin)->Set_IsDead();
			//iter_begin = m_CameraList.erase(iter_begin);
			return NOERROR;
		}
	}
	return E_FAIL;
}

CCameraLayer * CCameraLayer::Create(void)
{
	CCameraLayer *	pInstance = new CCameraLayer();

	if (FAILED(pInstance->Ready_Layer()))
	{
		MessageBox(0, L"CCameraLayer Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CCameraLayer::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CCameraLayer");


	int Count = 0;
	for (auto pObject : m_CameraList)
	{
		pObject->Set_LeafNode();
		CDebug::GetInstance()->Add_DebugText(L"%c) %s (%d)", CDebug::DisplayKey[Count], pObject->Get_CurTagName(), pObject->GetKey());
		//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
		if (GetAsyncKeyState(CDebug::Key[Count]) & 0x0001)
		{
			CDebug::GetInstance()->Set_CurDebug(pObject->Set_LeafFunc(), pObject);
			//break;
		}
		++Count;
	}
}
#endif

_ulong CCameraLayer::Free(void)	
{

	_ulong dwRefCnt = 0;

	for_each(m_CameraList.begin(), m_CameraList.end(), CRelease_Single());
	m_CameraList.clear();
	
	return dwRefCnt;
}


