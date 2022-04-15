#include "..\Headers\Layer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Object_Manager.h"

USING(Engine)

CLayer::CLayer()
{
#ifndef _FINAL
	Tag(L"CLayer", std::bind(&CLayer::Debug_Render, this));
#endif

}

Engine::CComponent * CLayer::Get_Component(const _uint & iIndex, const _tchar * pComTag)
{
	_uint iSize = _uint(m_ObjectList.size());

	if (iIndex >= iSize)
		return nullptr;

	auto iter = m_ObjectList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_ObjectList.end())
		return nullptr;
	

	return (*iter)->Get_Component(pComTag);
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);

	return NOERROR;
}

CGameObject * CLayer::Find_GameObject(const _uint & iLayerIdx)
{
	_uint iSize = _uint(m_ObjectList.size());

	if (iLayerIdx >= iSize)
		return nullptr;

	auto iter = m_ObjectList.begin();

	for (size_t i = 0; i < iLayerIdx; ++i)
		++iter;

	if (iter == m_ObjectList.end())
		return nullptr;

	return (*iter);
}

CGameObject * CLayer::FindObject_Near_Position(CGameObject* pObj)
{
	_float fDistMin = 99999.f;
	_float fDist = 0.f;
	CGameObject* pNearObj = nullptr;

	CTransform* pDestTransform = nullptr;
	CTransform* pSourTransform = pObj->Get_Transform();
	
	if (pSourTransform == nullptr)
		return nullptr;

	for (auto pTmpObj : m_ObjectList)
	{
		if(pTmpObj->GetKey() == pObj->GetKey())
			continue;

		pDestTransform = pTmpObj->Get_Transform();
		if (pDestTransform != nullptr)
		{
			fDist = D3DXVec3Length(&(*pDestTransform->Get_Infomation(CTransform::INFO_POSITION) 
				- *pSourTransform->Get_Infomation(CTransform::INFO_POSITION)));
			if (fDist < fDistMin )
			{
				pNearObj = pTmpObj;
				fDistMin = fDist;
			}
		}
	}

	return pNearObj;
}

CGameObject * CLayer::FindObject_Near_Position( CGameObject* pObj, const _float& _fOffsetDegree, const _float& _fSightAngleDegree, const _float& _fSightRange )
{
	_float fDistMin = _fSightRange;
	_float fDist = 0.f;
	CGameObject* pNearObj = nullptr;

	CTransform* pDestTransform = nullptr;
	CTransform* pSourTransform = pObj->Get_Transform();

	if (pSourTransform == nullptr)
		return nullptr;

	_float fSightAngleRadian = D3DXToRadian( _fSightAngleDegree );
	_float fOffsetRadian = D3DXToRadian( _fOffsetDegree );
	if ( fSightAngleRadian < 0.f )
		fSightAngleRadian = D3DX_PI * 2.f - fSightAngleRadian;
	if ( fOffsetRadian < 0.f )
		fOffsetRadian = D3DX_PI * 2.f - fOffsetRadian;

	_vec3 vDstPos( 0.f, 0.f, 0.f );
	_vec3 vSrcPos = *pSourTransform->Get_Infomation( CTransform::INFO_POSITION );
	_vec3 vDirSrcToDst( 0.f, 0.f, 0.f );
	_vec3 vSrcLookDir = *pSourTransform->Get_WorldLookVec(); vSrcLookDir.y = 0.f;
	_vec3 vSrcRightDir = *pSourTransform->Get_WorldRightVec(); vSrcRightDir.y = 0.f;

	D3DXVec3Normalize( &vSrcLookDir, &vSrcLookDir );
	D3DXVec3Normalize( &vSrcRightDir, &vSrcRightDir );

	_uint iObjKey = pObj->GetKey();

	_float fAngle = 0.f;
	_vec3 vCross( 0.f, 0.f, 0.f );

	for ( auto pTmpObj : m_ObjectList )
	{
		if ( pTmpObj->GetKey() == iObjKey )
			continue;

		pDestTransform = pTmpObj->Get_Transform();
		if ( pDestTransform != nullptr )
		{
			vDstPos = *pDestTransform->Get_Infomation( CTransform::INFO_POSITION );
			vDirSrcToDst = vDstPos - vSrcPos;

			fDist = D3DXVec3Length( &vDirSrcToDst );

			if ( fDist < fDistMin ) // 내 시야 범위 안에 들어온 적이라면..
			{
				vDirSrcToDst.y = 0.f;
				D3DXVec3Normalize( &vDirSrcToDst, &vDirSrcToDst );

				// 각도를 구함..
				fAngle = acosf( D3DXVec3Dot( &vDirSrcToDst, &vSrcLookDir ) );

				_float fDot = D3DXVec3Dot( &vDirSrcToDst, &vSrcRightDir );

				if ( fDot < 0.f )
					fAngle = D3DX_PI * 2.f - fAngle;

				fAngle -= fOffsetRadian;

				if ( fAngle < 0.f )
					fAngle += D3DX_PI * 2.f;
				if ( fAngle > D3DX_PI * 2.f )
					fAngle -= D3DX_PI * 2.f;

				_float test = D3DXToDegree( fAngle );

				if ( D3DX_PI * 2.f - fSightAngleRadian <= fAngle || fAngle <= fSightAngleRadian )	// 적이 내 시야 각 안에 들어 왔다..
				{
					pNearObj = pTmpObj;
					fDistMin = fDist;
				}
			}
		}
	}

	return pNearObj;
}

HRESULT CLayer::Ready_Layer(void)
{
	return NOERROR;
}

_int CLayer::Update_Layer(const _float & fTimeDelta)
{
	_int iExitCode = 0;

	auto& iter = m_ObjectList.begin();

	for (; iter != m_ObjectList.end(); )
	{
		//(*iter)->Set_TimeDelta(fTimeDelta);
		iExitCode = (*iter)->Update_GameObject(fTimeDelta);

		if (1 == iExitCode)	/*Object Is Dead*/
		{
			//Safe_Release((*iter));
			CObject_Manager::GetInstance()->Add_DeleteObject(*iter);
			iter = m_ObjectList.erase(iter);
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

_int CLayer::LastUpdate_Layer(const _float & fTimeDelta)
{
	_int iExitCode = 0;

	auto& iter = m_ObjectList.begin();

	for (; iter != m_ObjectList.end(); )
	{
		iExitCode = (*iter)->LastUpdate_GameObject(fTimeDelta);

		if (1 == iExitCode)	/*Object Is Dead*/
		{
			//Safe_Release((*iter));
			CObject_Manager::GetInstance()->Add_DeleteObject(*iter);
			iter = m_ObjectList.erase(iter);
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

void CLayer::Render_Layer(void)
{
	for (auto& pGameObject : m_ObjectList)	
		pGameObject->Render_GameObject();	
}

CGameObject * CLayer::Find_ObjectByKey(const _uint & dwKey)
{
	OBJECTLIST::iterator iter_begin = m_ObjectList.begin();
	OBJECTLIST::iterator iter_end = m_ObjectList.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		if ((*iter_begin)->GetKey() == dwKey)
		{
			return (*iter_begin);
		}
	}

	return nullptr;
}

HRESULT CLayer::Delete_ObjectByKey(const _uint & pKey)
{
	OBJECTLIST::iterator iter_begin = m_ObjectList.begin();
	OBJECTLIST::iterator iter_end = m_ObjectList.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		if ((*iter_begin)->GetKey() == pKey)
		{
			//Safe_Release(*iter_begin);
			//(*iter_begin)->Set_IsDead();
			//iter_begin = m_ObjectList.erase(iter_begin);
			CObject_Manager::GetInstance()->Add_DeleteObject(*iter_begin);
			iter_begin = m_ObjectList.erase(iter_begin);
			return NOERROR;
		}
	}
	return E_FAIL;
}

CLayer * CLayer::Create(void)
{
	CLayer *	pInstance = new CLayer();

	if (FAILED(pInstance->Ready_Layer()))
	{
		MessageBox(0, L"CLayer Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

#ifndef _FINAL
void CLayer::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CLayer");
	//CDebug::GetInstance()->Add_DebugText();
	//int iCount = 0;

	int Count =0;
	for (auto pObject : m_ObjectList)
	{
		pObject->Set_LeafNode();
		CDebug::GetInstance()->Add_DebugText(L"%c) %s (%d)", CDebug::DisplayKey[Count], pObject->Get_CurTagName(),pObject->GetKey());
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

_ulong CLayer::Free(void)	//By.Jang
{
	
	_ulong dwRefCnt = 0;


	OBJECTLIST::iterator iter_begin = m_ObjectList.begin();
	OBJECTLIST::iterator iter_end = m_ObjectList.end();

	for (; iter_begin != iter_end;)
	{
		CObject_Manager::GetInstance()->Add_DeleteObject(*iter_begin);
		iter_begin = m_ObjectList.erase(iter_begin);
	}
	m_ObjectList.clear();


	//for (auto pObj : m_ObjectList)
	//{
	//	dwRefCnt += Safe_Release(pObj);
	//	CObject_Manager::GetInstance()->Add_DeleteObject(pObj);
	//}
	////for_each(m_ObjectList.begin(), m_ObjectList.end(), CRelease_Single());
	//m_ObjectList.clear();

	//for (auto& iter : m_ObjectList)
	//{
	//	Safe_Release((*iter));
	//}
	//m_ObjectList.clear();

	//for (auto pObj : m_ObjectList)
	//{
	//	dwRefCnt += Safe_Release(pObj);
	//}
	//m_ObjectList.clear();


	return dwRefCnt;
}


