#include "..\Headers\Trigger.h"
#include "Component_Manager.h"

USING(Engine)

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev),
	//m_pTransformCom(nullptr),
	m_pRendererCom(nullptr),
	m_pFunc(nullptr)
{
#ifndef _FINAL
	Tag(L"CTrigger", std::bind(&CTrigger::Debug_Render, this));
#endif

}

HRESULT CTrigger::Ready_GameObject(Func pFunc, CTrigger::Type eType)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	m_pFunc = pFunc;
	m_eType = eType;

	return NOERROR;
}

_int CTrigger::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	if (m_bHide)
		return 0;

	
	_int iReturn = CGameObject::Update_GameObject(fTimeDelta);

	switch (m_eType)
	{
	case 충돌후벗어나면파괴:
	{
		if (m_bCollOut)
		{
			if (nullptr != m_pFunc)
			{
				m_pFunc();
				//m_bIsDead = true;
				Set_IsDead(true);
			}
		}
	}
		break;
	case 충돌후벗어나면작동:
	{
		if (m_bCollOut)
		{
			if (nullptr != m_pFunc)
			{
				m_pFunc();
			}
		}
	}
		break;
	case 충돌시작동후파괴:
	{
		if (m_bColl)
		{
			if (nullptr != m_pFunc)
			{
				m_pFunc();
				//m_bIsDead = true;
				Set_IsDead(true);
			}
		}
	}
		break;
	case 충돌시작동:
	{
		if (m_bColl)
		{
			if (nullptr != m_pFunc)
			{
				m_pFunc();
			}
		}
	}
		break;
	}


	return iReturn;
}

_int CTrigger::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bHide)
		return 0;

	_int iReturn = CGameObject::LastUpdate_GameObject(fTimeDelta);

	return iReturn;
}

void CTrigger::Render_GameObject()
{

}

#ifndef _FINAL
CTrigger * CTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev, Func pFunc, CTrigger::Type eType)
{
	CTrigger *	pInstance = new CTrigger(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pFunc, eType)))
	{
		MSG_BOX("CTrigger Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
void CTrigger::Debug_Render(void)
{
	CDebug::GetInstance()->Add_DebugText("CTrigger");
	switch (m_eType)
	{
	case 충돌후벗어나면파괴:
	{
		CDebug::GetInstance()->Add_DebugText("Type : Destroy after collision");
	}
	break;
	case 충돌후벗어나면작동:
	{
		CDebug::GetInstance()->Add_DebugText("Type : Active after collision");
	}
	break;
	case 충돌시작동후파괴:
	{
		CDebug::GetInstance()->Add_DebugText("Type : Destroy At collision");
	}
	break;
	case 충돌시작동:
	{
		CDebug::GetInstance()->Add_DebugText("Type : Active At collision");
	}
	break;
	case 속성없음:
	{
		CDebug::GetInstance()->Add_DebugText("Type : None");
	}
	break;
	}
	if (m_bColl)
		CDebug::GetInstance()->Add_DebugText("m_bColl : true");
	else
		CDebug::GetInstance()->Add_DebugText("m_bColl : false");

	if (m_bHide)
		CDebug::GetInstance()->Add_DebugText("m_bHide : true");
	else
		CDebug::GetInstance()->Add_DebugText("m_bHide : false");


}
#endif

_ulong CTrigger::Free()
{
	//Safe_Release(m_pUser);
	//Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	m_pFunc = nullptr;

	return  Engine::CGameObject::Free();
}
