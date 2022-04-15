#include "stdafx.h"
#include "..\Headers\Effect_Buffer.h"
#include "Component_Manager.h"
#include "Target_Manager.h"
#include "Camera_Manager.h"

CEffect_Buffer::CEffect_Buffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	//, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
{
	for (auto& pTexture : m_pTextureCom)
		pTexture = nullptr;
#ifndef _FINAL
	Tag(L"CEffect_Buffer", std::bind(&CEffect_Buffer::Debug_Render, this));
#endif

}

HRESULT CEffect_Buffer::Ready_GameObject(void)
{
	m_iTexCnt = 3;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_vPos = _vec3(127.7f, 1.f + CRandom::GetInstance()->GetValueF(-0.1f, 0.1f), 116.8f + CRandom::GetInstance()->GetValueF(-0.005f, 0.005f));
	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &m_vPos);
	//m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &_vec3(1000.f,1000.f,1000.f));

	_float	fTemp = CRandom::GetInstance()->GetValueF(-0.5f, 0.5f);
	m_fUVSpeed = 0.7f + fTemp;

	return NOERROR;
}

_int CEffect_Buffer::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	_matrix		matInvView = CCamera_Manager::GetInstance()->Get_InverseViewMatrix();

	m_pTransformCom->Set_Angle(matInvView);
	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &m_vPos);

	m_fTimeDelta += m_fUVSpeed * fTimeDelta;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CEffect_Buffer::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;		

	if (nullptr == m_pGraphicDev)
		return;

	if(nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if(nullptr == pEffect)
		return;

	SetUp_OnConstantTable(pEffect);

	//pEffect->SetTechnique("HDR_Device");
	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(SHADER_EFFECT_MASK);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();
}

HRESULT CEffect_Buffer::Ready_Component(void)
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

	// For. Mix Texture
	pComponent = m_pTextureCom[0] = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_AlphaMask_Test1");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture1", pComponent));
	m_pTextureCom[0]->AddRef();

	pComponent = m_pTextureCom[1] = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_AlphaMask_Test2");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture2", pComponent));
	m_pTextureCom[1]->AddRef();

	pComponent = m_pTextureCom[2] = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Tiling_Test2");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture3", pComponent));
	m_pTextureCom[2]->AddRef();

	return NOERROR;
}

HRESULT CEffect_Buffer::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//pEffect->SetInt("g_iMixCnt", m_iTexCnt);

	D3DXHANDLE	hUVSpeed;

	for (int i = 0 ; i < 5; ++i)
	{
		hUVSpeed = pEffect->GetParameterElement("g_fUVSpeed", i);
		pEffect->SetFloat(hUVSpeed, 0.f);
	}

	hUVSpeed = pEffect->GetParameterElement("g_fUVSpeed", 0);
	pEffect->SetFloat(hUVSpeed, m_fTimeDelta);

	m_pTextureCom[0]->SetUp_OnShader(pEffect, "g_BlendTexture_0");
	m_pTextureCom[1]->SetUp_OnShader(pEffect, "g_BlendTexture_1");
	m_pTextureCom[2]->SetUp_OnShader(pEffect, "g_BlendTexture_2");

	CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"RenderTarget_Depth", "g_DepthTexture");

	Safe_Release(pEffect);

	return NOERROR;
}

CEffect_Buffer * CEffect_Buffer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_Buffer *	pInstance = new CEffect_Buffer(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CEffect_Buffer Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CEffect_Buffer::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CEffect_Buffer");

}
#endif
_ulong CEffect_Buffer::Free(void)
{
	_ulong		dwRefCnt = 0;
		
	//Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pShaderCom);
	
	for (size_t i = 0; i < m_iTexCnt; ++i)
	{
		Engine::Safe_Release(m_pTextureCom[i]);
	}

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
