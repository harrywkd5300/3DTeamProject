#include "stdafx.h"
#include "Staff_UseTool.h"

#include "Component_Manager.h"
#include "Object_Manager.h"

CStaff_UseTool::CStaff_UseTool(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEquipment(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CStaff_UseTool", std::bind(&CStaff_UseTool::Debug_Render, this));
#endif

}

HRESULT CStaff_UseTool::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;	
	
	m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(275, 175, 0));
	

	return NOERROR;
}

_int CStaff_UseTool::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bHide)
		return 0;

	int iReturn = Engine::CEquipment::Update_GameObject(fTimeDelta);
	
	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return iReturn;
}

void CStaff_UseTool::Render_GameObject(void)
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

HRESULT CStaff_UseTool::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_TOOL,  L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	
	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_TOOL, L"CMesh_Static_Staff");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_TOOL, L"CCollider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->ReSizing(CCollider::TYPE_OBB, m_pMeshCom->Get_Min(), m_pMeshCom->Get_Max(), m_pTransformCom);
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();


	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_TOOL,  L"CRenderer");
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

HRESULT CStaff_UseTool::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CStaff_UseTool * CStaff_UseTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStaff_UseTool *	pInstance = new CStaff_UseTool(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CTree Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CStaff_UseTool::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CStaff_UseTool");

}
#endif
_ulong CStaff_UseTool::Free(void)
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

	dwRefCnt = Engine::CEquipment::Free();

	return dwRefCnt;
}
