#include "stdafx.h"
#include "..\Headers\Monster_Default_Weapon.h"

#include "Component_Manager.h"
#include "Object_Manager.h"

CMonster_Default_Weapon::CMonster_Default_Weapon(LPDIRECT3DDEVICE9 pGraphicDev, eMonster_Weapon eWeapon)
	: CEquipment(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CMonster_Default_Weapon", std::bind(&CMonster_Default_Weapon::Debug_Render, this));
#endif


	m_eWeapon = eWeapon;
}

HRESULT CMonster_Default_Weapon::Ready_GameObject()
{

	if (FAILED(Ready_Component()))
		return E_FAIL;	
	
	m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(90.f, 0.f, 90.f));
	

	return NOERROR;
}

_int CMonster_Default_Weapon::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bHide)
		return 0;

	int iReturn = Engine::CEquipment::Update_GameObject(fTimeDelta);
	
	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return iReturn;
}

void CMonster_Default_Weapon::Render_GameObject(void)
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

HRESULT CMonster_Default_Weapon::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	
	// For.Mesh
	if(Bat == m_eWeapon)
		pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CMesh_Static_Bat");
	else if(SpikeBat == m_eWeapon)
		pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CMesh_Static_SpikeBat");
	else if (Pipe == m_eWeapon)
		pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CMesh_Static_Pipe");
	else if (PipeWRench == m_eWeapon)
		pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CMesh_Static_PipeWRench");

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


HRESULT CMonster_Default_Weapon::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CMonster_Default_Weapon * CMonster_Default_Weapon::Create(LPDIRECT3DDEVICE9 pGraphicDev, eMonster_Weapon eWeapon)
{
	CMonster_Default_Weapon *	pInstance = new CMonster_Default_Weapon(pGraphicDev, eWeapon);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CMonster_Default_Weapon Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CMonster_Default_Weapon::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CMonster_Default_Weapon");

}
#endif
_ulong CMonster_Default_Weapon::Free(void)
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

	dwRefCnt = Engine::CEquipment::Free();

	return dwRefCnt;
}
