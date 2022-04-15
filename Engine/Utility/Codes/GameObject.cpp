#include "..\Headers\GameObject.h"
#include "Timer_Manager.h"
#include "Transform.h"
USING(Engine)

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pTransformCom(nullptr)
	, m_bIsDead(false)
	, m_pTimer(CTimer_Manager::GetInstance()->Get_MainGameTimer())
{
	m_pGraphicDev->AddRef();
#ifndef _FINAL
	Tag(L"CGameObject", std::bind(&CGameObject::Debug_Render, this));
#endif
}



Engine::CComponent * CGameObject::Get_Component(const _tchar * pComTag)
{
	Engine::CComponent*	pComponent = Find_Component(pComTag);

	if(nullptr == pComponent)
		return nullptr;

	return pComponent;
}

HRESULT CGameObject::Ready_GameObject(void)
{
	return NOERROR;
}

_int CGameObject::Update_GameObject(const _float & fTimeDelta)
{
	for (auto& Pair : m_mapComponent[Engine::CComponent::TYPE_DYNAMIC])
	{
		Pair.second->Update_Component(fTimeDelta);
	}
	return 0;
}

_int CGameObject::LastUpdate_GameObject(const _float & fTimeDelta)
{
	for (auto& Pair : m_mapComponent[Engine::CComponent::TYPE_DYNAMIC])
	{
		Pair.second->LastUpdate_Component(fTimeDelta);
	}
	return 0;
}

void CGameObject::Render_GameObject(void)
{
}

#ifndef _FINAL
void CGameObject::Debug_Render(void)
{
	//CBase::Debug_Render();
	CDebug::GetInstance()->Add_DebugText("CGameObject");
	CDebug::GetInstance()->Add_DebugText("Key :%d", GetKey() );
	if(m_bIsDead)
		CDebug::GetInstance()->Add_DebugText("m_bIsDead : true");
	else
		CDebug::GetInstance()->Add_DebugText("m_bIsDead : false");

	int Count = 0;
	for (size_t i = 0; i < Engine::CComponent::TYPE_END; ++i)
	{
		for (auto& Pair : m_mapComponent[i])
		{
			CComponent* pCom = Pair.second;
			CDebug::GetInstance()->Add_DebugText(L"%c) %s (%d)", CDebug::DisplayKey[Count], pCom->Get_CurTagName(), pCom->GetKey());
			//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
			if (GetAsyncKeyState(CDebug::Key[Count]) & 0x0001)
			{
				CDebug::GetInstance()->Set_CurDebug(pCom->Set_LeafFunc(), pCom);
				//break;
			}
			++Count;
		}
	}
	return;
}
#endif

CComponent * CGameObject::Find_Component(const _tchar * pComTag)
{
	for (size_t i = 0; i < Engine::CComponent::TYPE_END; ++i)
	{
		auto& iter = find_if(m_mapComponent[i].begin(), m_mapComponent[i].end(), CFind_Tag(pComTag));
		if (iter != m_mapComponent[i].end())
			return iter->second;
	}
	return nullptr;
}

_ulong CGameObject::Free(void)
{
	_ulong dwRefCnt = 0;

	for (size_t i = 0; i < Engine::CComponent::TYPE_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CRelease_Pair());
		m_mapComponent[i].clear();
	}	

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pGraphicDev);
	
	return dwRefCnt;
}
