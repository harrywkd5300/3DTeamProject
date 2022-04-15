#include "stdafx.h"
#include "..\Headers\Effect_Mesh.h"
#include "Component_Manager.h"
#include "Target_Manager.h"
#include "Graphic_Device.h"
#include "Camera_Manager.h"

CEffect_Mesh::CEffect_Mesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	//, m_pTransformCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pMeshCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pCam_Manager(CCamera_Manager::GetInstance())
{
	m_pCam_Manager->AddRef();
#ifndef _FINAL
	Tag(L"CEffect_Mesh", std::bind(&CEffect_Mesh::Debug_Render, this));
#endif

}

HRESULT CEffect_Mesh::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;		

	//m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &_vec3(3.f, 1.f, 3.f + CRandom::GetInstance()->GetValueF(-1.f, 1.f)));
	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &_vec3(3.f, 1.f, 3.f));
	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &_vec3(0.01f, 0.01f, 0.01f));

	m_fUVSpeed = 0.5f;
	m_fLifeTime = 10.f;

	m_fCurTime = 0.f;

	return NOERROR;
}

_int CEffect_Mesh::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_fCurTime += fTimeDelta;
	if (m_fCurTime >= m_fLifeTime)
	{
		m_fUVScale = 0.f;
		m_fCurTime = 0.f;
		//Set_IsDead(); 
	}

	m_fUVScale += m_fUVSpeed * fTimeDelta;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CEffect_Mesh::Render_GameObject(void)
{
	if (nullptr == m_pMeshCom
		|| nullptr == m_pGraphicDev
		|| nullptr == m_pShaderCom
		|| nullptr == m_pCam_Manager)
		return;		


	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if(nullptr == pEffect)
		return;

	SetUp_OnConstantTable(pEffect);

	//pEffect->SetTechnique("HDR_Device");
	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(SHADER_EFFECT_MESH);
	
	m_pMeshCom->Render_Mesh();

	pEffect->EndPass();
	pEffect->End();

	CDebug::GetInstance()->Add_DebugText(30.f, 60.f, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), "CurTime : %f", m_fCurTime);
}

HRESULT CEffect_Mesh::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;
		
	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();
	
	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CShader_Effect");
	if(nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	// For. Texture
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Mesh_Test1");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	// For. Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CMesh_Static_Effect_Test1");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();

	return NOERROR;
}

HRESULT CEffect_Mesh::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_vec3	vPos = *m_pTransformCom->Get_Infomation(Engine::CTransform::INFO_POSITION);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	matView = m_pCam_Manager->Get_ViewMatrix();
	matProj = m_pCam_Manager->Get_ProjMatrix();

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fUVScale", m_fUVScale);
	pEffect->SetVector("g_vMeshPos", &_vec4(vPos, 1.f));

	m_pTextureCom->SetUp_OnShader(pEffect, "g_MeshUVTexture");

	// 1 - 조절 첫 시간(상대값 <- 전체지속시간)
	// 2 - 조절 끝 시간(상대값 <- 전체지속시간)
	// 3 - 시작 색상 강도(0 ~ 1) -> 알파값(255)
	// 4 - 끝 색상 강도(0 ~ 1)
	// 알파값 조절을 여러 개의 노드로 처리하는 것은 필요하면 추후에!
	float	arrFadeTime[4] = { 0.f, 0.4f, 0.6f, 1.f };
	float	arrFadePower[2] = { 1.f, 0.f };
	
	float	fFadeValue = 0.f, fTimeRatio = 0.f;

	fTimeRatio = m_fCurTime / m_fLifeTime;

	if (fTimeRatio > arrFadeTime[0] && fTimeRatio < arrFadeTime[1])
		fFadeValue = (fTimeRatio / arrFadeTime[1]) * arrFadePower[0];
	else if (fTimeRatio > arrFadeTime[1] && fTimeRatio < arrFadeTime[2])
		fFadeValue = arrFadePower[0];
	else if (fTimeRatio > arrFadeTime[2] && fTimeRatio < arrFadeTime[3])
		fFadeValue = ((1.f - fTimeRatio) / (arrFadeTime[3] - arrFadeTime[2])) + arrFadePower[1];

	pEffect->SetFloat("g_fFadeValue", fFadeValue);

	CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"RenderTarget_Depth", "g_DepthTexture");

	Engine::Safe_Release(pEffect);

	return NOERROR;
}

CEffect_Mesh * CEffect_Mesh::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_Mesh *	pInstance = new CEffect_Mesh(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CEffect_Mesh Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

#ifndef _FINAL
void CEffect_Mesh::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CEffect_Mesh");

}
#endif

_ulong CEffect_Mesh::Free(void)
{
	_ulong		dwRefCnt = 0;
		
	//Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pCam_Manager);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
