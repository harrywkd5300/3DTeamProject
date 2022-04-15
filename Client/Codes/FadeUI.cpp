#include "stdafx.h"
#include "..\Headers\FadeUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"

#include "Input_Device.h" // 삭제


CFadeUI::CFadeUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

HRESULT CFadeUI::Ready_Observer(void)
{
	m_pObserver = CUIObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CFadeUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Fade");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CShader_UI");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

HRESULT CFadeUI::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
		return E_FAIL;


	return NOERROR;
}

_int CFadeUI::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);

	//if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_K))
	//{
	//	m_eFadeType = FADE_IN;
	//}
	//if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_L))
	//{
	//	m_eFadeType = FADE_OUT;
	//}

	MoveTo_UI(fTimeDelta);

	if (nullptr != m_pRendererCom && m_eFadeType != FADE_NONE)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this, RENDER_FADE);

	return 0;
}

void CFadeUI::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_ComboConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	pEffect->End();	
}

HRESULT CFadeUI::SetUp_ComboConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = BACKSIZEX;
	matView._22 = BACKSIZEY;
	matView._33 = 1.f;
	matView._41 = BACKSIZEX * 0.5f - BACKSIZEX * 0.5f;

	// 300
	matView._42 = (BACKSIZEY) * -0.5f + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture");

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

void CFadeUI::MoveTo_UI(const _float & fTimeDelta)
{
	if (m_eFadeType == FADE_IN)
	{
		m_fAlpha -= fTimeDelta * 1 / m_fRenderTime; // 3초에 걸쳐서 1을 만들어야한다.

		if (m_fAlpha <= 0.f)
			m_eFadeType = FADE_NONE;

	}
	else if (m_eFadeType == FADE_OUT)
	{
		m_fAlpha += fTimeDelta * 1 / m_fRenderTime; // 3초에 걸쳐서 1을 만들어야한다.		

		if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;
	}
		
}
CFadeUI * CFadeUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFadeUI *	pInstance = new CFadeUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CFadeUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CFadeUI::Free(void)
{	
	Safe_Release(m_pObserver);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	return CUI::Free();
}
