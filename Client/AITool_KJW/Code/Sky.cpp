#include "stdafx.h"
#include "..\Headers\Sky.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

CSky::CSky(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	//, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pMaterialCom(nullptr)
	, m_pShaderCom(nullptr)
{
#ifndef _FINAL
	Tag(L"CSky", std::bind(&CSky::Debug_Render, this));
#endif

}

HRESULT CSky::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &_vec3(100.f, 100.f, 100.f));

	//m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 90.f, 0.f));

	return NOERROR;
}

_int CSky::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_PRIORITY, this);

	return 0;
}

_int CSky::LastUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::LastUpdate_GameObject(fTimeDelta);

	_matrix				matView;
/*
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);*/

	matView = CCamera_Manager::GetInstance()->Get_InverseViewMatrix();
	
	_vec3 vUp = *(_vec3*)&matView.m[1];

	D3DXVec3Normalize(&vUp, &vUp);

	_vec3 vPos = *(_vec3*)&matView.m[3];

	vPos -= vUp * 15.f;

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &vPos);
	
	return _int();
}

void CSky::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom
		|| nullptr == m_pShaderCom)
		return;	
	
	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();


	SetUp_OnContantTable(pEffect);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);
	
	m_pBufferCom->Render_Buffer();	

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	
}

HRESULT CSky::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Material
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CMaterial");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Material", pComponent));
	m_pMaterialCom->AddRef();


	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CCube_Texture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CCube_Texture");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Texture
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Sky");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CShader_SkyBox");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

HRESULT CSky::SetUp_OnContantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture");

	Safe_Release(pEffect);

	return NOERROR;
}

CSky * CSky::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSky *	pInstance = new CSky(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CSky Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CSky::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CSky");

}
#endif
_ulong CSky::Free(void)
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pMaterialCom);
	//Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
