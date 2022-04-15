#include "stdafx.h"
#include "..\Headers\Stone.h"
#include "Component_Manager.h"
#include "Object_Manager.h"
#include "Graphic_Device.h"

CStone::CStone(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CStone", std::bind(&CStone::Debug_Render, this));
#endif

}

HRESULT CStone::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &_vec3(0.01f, 0.01f, 0.01f));
	//m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(CRandom::GetInstance()->GetValueF(20.f), 0.f, CRandom::GetInstance()->GetValueF(20.f)));

	return NOERROR;
}

_int CStone::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	Move_OnTerrain();

	Collision_ToPlayer();

	if (nullptr != m_pFrustumCom)
		m_isDraw = m_pFrustumCom->isIn_Frustum(m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), 2.f);

	if(true == m_isDraw)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);
	else
	{
		int a = 10;
	}
	return _int();
}

void CStone::Render_GameObject(void)
{
	if (nullptr == m_pMeshCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	
	if(nullptr == pEffect)
		return;

	SetUp_ConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pMeshCom->Render_Mesh();	


	pEffect->EndPass();
	pEffect->End();

#ifndef _FINAL
	m_pColliderCom->Render_Buffer();
#endif 
}

HRESULT CStone::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	
	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CMesh_Static_Stone");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CCollider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->ReSizing(CCollider::TYPE_AABB, m_pMeshCom->Get_Min(), m_pMeshCom->Get_Max(), m_pTransformCom);
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();	


	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Frustum
	pComponent = m_pFrustumCom = (Engine::CFrustum*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CFrustum");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Frustum", pComponent));
	m_pFrustumCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Mesh");
	if(nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();	

	return NOERROR;
}

void CStone::Move_OnTerrain(void)
{
	if (nullptr == m_pTransformCom)
		return;

	if (nullptr == m_pTargetBufferCom)
	{
		m_pTargetBufferCom = (CVIBuffer*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_BackGround", 0, L"Com_Buffer");
		m_pTargetBufferCom->AddRef();
	}
	m_pTransformCom->Move_OnBuffer(m_pTargetBufferCom);
}

void CStone::Collision_ToPlayer(void)
{
	CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Player", 0, L"Com_Collider");
	if (nullptr == pCollider_Player)
		return;
	
	//m_pColliderCom->Collision_AABB((CCollider*)pCollider_Player);
}

HRESULT CStone::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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



CStone * CStone::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStone *	pInstance = new CStone(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CStone Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CStone::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CStone");

}
#endif
_ulong CStone::Free(void)
{
	_ulong		dwRefCnt = 0;
	Engine::Safe_Release(m_pTargetBufferCom);//Jang
	Engine::Safe_Release(m_pColliderCom);
	//Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pFrustumCom);
	Engine::Safe_Release(m_pShaderCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
