#include "..\Headers\MyEffect.h"
#include "Component_Manager.h"
#include "MyFX.h"

USING(Engine)

CMyEffect::CMyEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_bIsClone(false)
	, m_pRandomMgr(CRandom::GetInstance())
	, m_pShaderCom(nullptr)
	, m_tEffInfo(EFFECT_INFO())
	//, m_pTransformCom(nullptr)
{
	m_pRandomMgr->AddRef();

#ifndef _FINAL
	Tag(L"CMyEffect", std::bind(&CMyEffect::Debug_Render, this));
#endif
}

CMyEffect::CMyEffect(const CMyEffect & rhs)
	: CGameObject(rhs.m_pGraphicDev)
	, m_pRandomMgr(CRandom::GetInstance())
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_bIsClone(true)
	, m_tEffInfo(rhs.m_tEffInfo)
	//, m_pTransformCom(nullptr)
{
	if (nullptr != m_pRandomMgr)
		m_pRandomMgr->AddRef();

	if (nullptr != m_pShaderCom)
		m_pShaderCom->AddRef();

#ifndef _FINAL
	Tag(L"CMyEffect", std::bind(&CMyEffect::Debug_Render, this));
#endif
}

void CMyEffect::Render_Effect(CMyFX * pFX)
{
}

HRESULT CMyEffect::Ready_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	//pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CTransform");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	//m_pTransformCom->AddRef();

	return NOERROR;
}

void CMyEffect::SetUp_OnConstantTable(LPD3DXEFFECT pEffect, CMyFX * pFX)
{
}

#ifndef _FINAL
void CMyEffect::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CMyEffect");
}
#endif

_ulong CMyEffect::Free(void)
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pRandomMgr);
	Engine::Safe_Release(m_pShaderCom);
	//Engine::Safe_Release(m_pTransformCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}