#include "stdafx.h"
#include "..\Headers\Soldier_Sword.h"

#include "Component_Manager.h"
#include "Object_Manager.h"

CSoldier_Sword::CSoldier_Sword(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEquipment(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CSoldier_Sword", std::bind(&CSoldier_Sword::Debug_Render, this));
#endif
}

HRESULT CSoldier_Sword::Ready_GameObject()
{

	if (FAILED(Ready_Component()))
		return E_FAIL;	
	
	m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(90.f, 0.f, 90.f));
	
	_vec3 vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	vPos.x -= 18.f;
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vPos);


	return NOERROR;
}

_int CSoldier_Sword::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bHide)
		return 0;

	int iReturn = Engine::CEquipment::Update_GameObject(fTimeDelta);
	
	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return iReturn;
}

void CSoldier_Sword::Render_GameObject(void)
{
	if (nullptr == m_pMeshCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_ConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);
	//pEffect->BeginPass(0);

	//m_pMeshCom->Render_Mesh();
	m_pMeshCom->Render_Mesh(pEffect, "g_BaseTexture", "g_NormalTexture", "g_SpecularTexture");

	//pEffect->EndPass();
	pEffect->End();

	
}

HRESULT CSoldier_Sword::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	
	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CMesh_Static_Monster_Sword");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();



	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();


	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();


	return NOERROR;
}


HRESULT CSoldier_Sword::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	Safe_Release(pEffect);

	return NOERROR;
}

CSoldier_Sword * CSoldier_Sword::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSoldier_Sword *	pInstance = new CSoldier_Sword(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CSoldier_Sword Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CSoldier_Sword::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CSoldier_Sword");

}
#endif
_ulong CSoldier_Sword::Free(void)
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

	dwRefCnt = Engine::CEquipment::Free();

	return dwRefCnt;
}
