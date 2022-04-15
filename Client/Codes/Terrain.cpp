#include "stdafx.h"
#include "..\Headers\Terrain.h"

#include "Component_Manager.h"
#include "Target_Manager.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	//, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pMaterialCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pBrushCom(nullptr)
	, m_pFrustumCom(nullptr)
{
#ifndef _FINAL
	Tag(L"CTerrain", std::bind(&CTerrain::Debug_Render, this));
#endif

}

HRESULT CTerrain::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	
	D3DXCreateTexture(m_pGraphicDev, 129, 129, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pFilterTexture);

	D3DLOCKED_RECT		Lock_Rect;

	m_pFilterTexture->LockRect(0, &Lock_Rect, nullptr, 0);

	for (_uint i = 0; i < 129; i++)
	{
		for (_uint j = 0; j < 129; j++)
		{
			_uint iIndex = i * 129 + j;

			if(j > 64)
				((_ulong*)Lock_Rect.pBits)[iIndex] = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.f);
			else
				((_ulong*)Lock_Rect.pBits)[iIndex] = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		}

	}

	m_pFilterTexture->UnlockRect(0);	

	return NOERROR;
}

_int CTerrain::Update_GameObject(const _float & fTimeDelta)
{

	Engine::CGameObject::Update_GameObject(fTimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

void CTerrain::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom
		|| nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	CTarget_Manager::GetInstance()->SetUp_OnShader( pEffect, L"RenderTarget_ShadowMap_Near", "g_ShadowNearTexture" );
	CTarget_Manager::GetInstance()->SetUp_OnShader( pEffect, L"RenderTarget_ShadowMap_Mid", "g_ShadowMidTexture" );
	CTarget_Manager::GetInstance()->SetUp_OnShader( pEffect, L"RenderTarget_ShadowMap_Far", "g_ShadowFarTexture" );
	m_pRendererCom->SetUp_ShadowDataOnShader( pEffect, "g_matLightVP", "g_matLightVPB", "g_fShadowRange" );
	SetUp_OnConstantTable(pEffect);

	pEffect->Begin( nullptr, 0 );
	pEffect->BeginPass( 0 );

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CTerrain::Ready_Component(void)
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
	pComponent = m_pBufferCom = (Engine::CTerrain_Buffer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTerrain_Buffer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	//// For.Texture
	//pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Terrain");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	//m_pTextureCom->AddRef();

	// For.Buffers
	pComponent = m_pTextureCom = (Engine::CMultiTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CMultiTexture_Terrain");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	// For.Texture_Filter
	pComponent = m_pFilterCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Filter");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Filter", pComponent));
	m_pFilterCom->AddRef();

	// For.Texture_Brush
	pComponent = m_pBrushCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Brush");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Brush", pComponent));
	m_pBrushCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	//// For.Shader
	//pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CShader_Terrain");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	//m_pShaderCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CShader_Terrain_KOH");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	pComponent = m_pFrustumCom = (Engine::CFrustum*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CFrustum");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Frumstum", pComponent));
	m_pFrustumCom->AddRef();

	

	return NOERROR;
}

HRESULT CTerrain::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture");
	//m_pTextureCom->SetUp_OnShader(pEffect, "g_DestTexture", 1);
	m_pTextureCom->SetUp_OnShader(pEffect, CMultiTexture::TEX_TYPE_DIFFUSE, "g_BaseTexture");
	m_pTextureCom->SetUp_OnShader(pEffect, CMultiTexture::TEX_TYPE_NORMAL, "g_BaseNormalTexture");
	m_pTextureCom->SetUp_OnShader(pEffect, CMultiTexture::TEX_TYPE_SPECULAR, "g_BaseSpecTexture");

	m_pBrushCom->SetUp_OnShader(pEffect, "g_BrushTexture");	

	pEffect->SetTexture("g_FilterTexture", m_pFilterTexture);	

	pEffect->SetFloat("g_fUV", 257.f);

	Safe_Release(pEffect);

	return NOERROR;
}

CTerrain * CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain *	pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CTerrain Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

#ifndef _FINAL
void CTerrain::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CTerrain");

}
#endif

_ulong CTerrain::Free(void)
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pMaterialCom);
	//Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pFilterCom);
	Engine::Safe_Release(m_pBrushCom);
	Engine::Safe_Release(m_pFilterTexture);
	Engine::Safe_Release(m_pFrustumCom);
	

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
