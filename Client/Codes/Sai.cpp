#include "stdafx.h"
#include "..\Headers\Sai.h"

#include "Component_Manager.h"
#include "Object_Manager.h"

CSai::CSai(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEquipment(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CSai", std::bind(&CSai::Debug_Render, this));
#endif

}

HRESULT CSai::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;	

	m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(90.f, 0.f, 0.f));
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(0.f, 0.f, -10.f));


	return NOERROR;
}

_int CSai::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bHide)
		return 0;

	int iReturn = Engine::CEquipment::Update_GameObject(fTimeDelta);

	Collision_ToStone();
	
	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return iReturn;
}

void CSai::Render_GameObject(void)
{
//	if (nullptr == m_pMeshCom)
//		return;
//
//	if (nullptr == m_pGraphicDev)
//		return;
//
//	m_pTransformCom->SetUp_OnGraphicDev();
//
//#ifndef _FINAL
//	m_pColliderCom->Render_Buffer();
//#endif 
//
//	m_pMeshCom->Render_Mesh();	
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

#ifndef _FINAL
	m_pColliderCom->Render_Buffer();
#endif 
	
}

HRESULT CSai::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	
	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CMesh_Static_Sai");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CCollider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->ReSizing(CCollider::TYPE_OBB, m_pMeshCom->Get_Min(), m_pMeshCom->Get_Max(), m_pTransformCom);
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();


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

void CSai::Collision_ToStone(void)
{
	
	_uint iSize = CObject_Manager::GetInstance()->Get_NumObject(SCENE_STAGE, L"Layer_StaticObject");
	for (_uint i = 0; i < iSize; i++)
	{
		CComponent*		pCollider_Static = CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_StaticObject", i, L"Com_Collider");
		if (nullptr == pCollider_Static)
			return;

		if (true == m_pColliderCom->Collision_OBB((CCollider*)pCollider_Static))
			break;
	}	
}

HRESULT CSai::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CSai * CSai::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSai *	pInstance = new CSai(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CTree Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CSai::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CSai");

}
#endif
_ulong CSai::Free(void)
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

	dwRefCnt = Engine::CEquipment::Free();

	return dwRefCnt;
}
