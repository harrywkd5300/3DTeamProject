#include "stdafx.h"
#include "..\Headers\Effect_Sprite.h"
#include "Component_Manager.h"
#include "Target_Manager.h"


CEffect_Sprite::CEffect_Sprite(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	//, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_fCurFrame(0.f)
{
#ifndef _FINAL
	Tag(L"CEffect_Sprite", std::bind(&CEffect_Sprite::Debug_Render, this));
#endif

}

HRESULT CEffect_Sprite::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;		

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, 50.f, 0.f, 50.f);

	m_fFrameNum = 32.f;
	m_fSpriteSpeed = 30.f;

	return NOERROR;
}

_int CEffect_Sprite::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	_matrix			matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	m_fCurFrame += m_fSpriteSpeed * fTimeDelta;

	if (m_fCurFrame >= m_fFrameNum)
		m_fCurFrame = 0.f;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);


	return 0;
}

void CEffect_Sprite::Render_GameObject(void)
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
	pEffect->BeginPass(SHADER_EFFECT_CUTBLACK);
	
	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();
}

HRESULT CEffect_Sprite::Ready_Component(void)
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
	/*pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Effect");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();
*/
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Sprite_Test");
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
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CShader_Effect");
	if(nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

HRESULT CEffect_Sprite::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", (int)m_fCurFrame);
	CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"RenderTarget_Depth", "g_DepthTexture");

	// Effect Type - Sprite
	m_vSliceCnt.x = 8.f;
	m_vSliceCnt.y = 4.f;

	pEffect->SetFloat("g_fSliceX", m_vSliceCnt.x);
	pEffect->SetFloat("g_fSliceY", m_vSliceCnt.y);

	m_vUVOffset.x = ((int)m_fCurFrame % (int)m_vSliceCnt.x) / m_vSliceCnt.x;
	m_vUVOffset.y = ((int)m_fCurFrame / (int)m_vSliceCnt.x) / m_vSliceCnt.y;

	pEffect->SetFloatArray("g_vUVOffset", m_vUVOffset, 2);

	Safe_Release(pEffect);

	return NOERROR;
}

CEffect_Sprite * CEffect_Sprite::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_Sprite *	pInstance = new CEffect_Sprite(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CEffect_Sprite Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

#ifndef _FINAL
void CEffect_Sprite::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CEffect_Sprite");

}
#endif

_ulong CEffect_Sprite::Free(void)
{
	_ulong		dwRefCnt = 0;
		
	Engine::Safe_Release(m_pShaderCom);
	//Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
