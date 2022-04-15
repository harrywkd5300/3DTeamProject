#include "stdafx.h"
#include "..\Headers\Effect_Distort.h"
#include "Component_Manager.h"
#include "Camera_Manager.h"
#include "Target_Manager.h"


CEffect_Distort::CEffect_Distort(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	//, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pCam_Manager(CCamera_Manager::GetInstance())
{
	m_pCam_Manager->AddRef();
#ifndef _FINAL
	Tag(L"CEffect_Distort", std::bind(&CEffect_Distort::Debug_Render, this));
#endif

}

HRESULT CEffect_Distort::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;		

	m_vPos = _vec3(127.7f, 1.3f, 118.f);
	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &m_vPos);

	return NOERROR;
}

_int CEffect_Distort::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pCam_Manager)
		return 0;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	_matrix		matInvView = m_pCam_Manager->Get_InverseViewMatrix();

	m_pTransformCom->Set_Angle(matInvView);
	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &m_vPos);

	m_fTimeDelta += fTimeDelta;

	/*_vec3	vScale = _vec3(1.f, 1.f, 1.f);
	vScale *= m_fTimeDelta * 0.08f;

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &vScale);*/

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_DISOTRT, this);

	return 0;
}

void CEffect_Distort::Render_GameObject(void)
{
	if (nullptr == m_pGraphicDev
		|| nullptr == m_pShaderCom
		|| nullptr == m_pBufferCom
		|| nullptr == m_pCam_Manager)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if(nullptr == pEffect)
		return;

	SetUp_OnConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(SHADER_EFFECT_DISTORTION);
	
	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();
}

HRESULT CEffect_Distort::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;
		
	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();
	
	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CRect_Texture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CRect_Texture");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Texture
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Distort_Test1");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	//pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Distort_Test2");
	//if (nullptr == pComponent)
	//return E_FAIL;
	//m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	//m_pTextureCom->AddRef();

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

	return NOERROR;
}

HRESULT CEffect_Distort::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	matView = m_pCam_Manager->Get_ViewMatrix();
	matProj = m_pCam_Manager->Get_ProjMatrix();

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_DistortTexture");

	pEffect->SetFloat("g_fTimeDelta", m_fTimeDelta);

	Safe_Release(pEffect);

	return NOERROR;
}

CEffect_Distort * CEffect_Distort::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_Distort *	pInstance = new CEffect_Distort(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CEffect_Distort Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CEffect_Distort::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CEffect_Distort");

}
#endif
_ulong CEffect_Distort::Free(void)
{
	_ulong		dwRefCnt = 0;
		
	//Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pCam_Manager);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
