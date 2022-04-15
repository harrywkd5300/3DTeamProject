#include "stdafx.h"
#include "..\Headers\ComboUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"
#include "Object_Manager.h"
#include "Player.h"

#include "Input_Device.h" // 삭제
#include "MyFont.h"


CComboUI::CComboUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

HRESULT CComboUI::Ready_Observer(void)
{
	m_pObserver = CUIObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CComboUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Combo");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CShader_UI");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();


	m_pFont = CMyFont::Create(m_pGraphicDev, L"CTexture_MarioFont", 17.f, 6.f);

	
	return NOERROR;
}

HRESULT CComboUI::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
	return E_FAIL;
	

	m_fComboSizeX = 340.f;
	m_fComboSizeY = 239.f;

	m_fComboX = m_fComboSizeX / 2.f;
	m_fComboY = m_fComboSizeY /2.f;

	return NOERROR;
}

_int CComboUI::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);

	if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_U))
	{
		// 카메라의 월드행렬
		m_IsRender = true;
		m_fRenderTime = 3.f;
	}

	// 콤보가 상승하면 ComboTime 다시 증가.
	// m_fComboTime -= fTimeDelta;
	//if (// GetComobo())
	//{
	//	m_fComoboTime = 1.f;
	//}
	
	MoveTo_UI(fTimeDelta);
	

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this, RENDER_TOP);

	return 0;
}

void CComboUI::Render_GameObject(void)
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

	if (m_fAlpha != 0)
	{
		// Combo가 5이상이면 Color를바꾼다.

		m_pFont->Set_FontSetting(114.f, 129.f ,0.f, _vec4(1.f, 1.f, 1.f, m_fAlpha));
		m_pFont->Draw2DFont(m_fComboX, m_fComboY, "%d", 3);
	}

	m_pFont->Set_FontSetting(64.f / 2.f, 79.f / 2.f,  0.f, _vec4(1.f, 1.f, 1.f, 1.f));
	m_pFont->Draw3DFont(&_vec3(1.f, 1.f, 1.f), &_vec3(0, 60, 0),&_vec3(100, 0.5, 100), "This is %dD Font",3);

}

HRESULT CComboUI::SetUp_ComboConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fComboSizeX;
	matView._22 = m_fComboSizeY;
	matView._33 = 1.f;
	matView._41 = m_fComboX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fComboY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);	

	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture");

	pEffect->CommitChanges();
	
	Safe_Release(pEffect);

	return NOERROR;
}

void CComboUI::MoveTo_UI(const _float & fTimeDelta)
{
	if (m_IsRender == true)
	{
		m_fAlpha += fTimeDelta * 30.f;
		if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;

		m_fRenderTime -= fTimeDelta;
	}
	else
	{
		m_fAlpha -= fTimeDelta * 10.f;

		if (m_fAlpha <= 0.f)
			m_fAlpha = 0.f;
	}	

	if (m_fRenderTime <= 0)
	{
		m_IsRender = false;
		m_fRenderTime = 3.f;
	}
		

}
CComboUI * CComboUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CComboUI *	pInstance = new CComboUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CComboUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CComboUI::Free(void)
{
	Safe_Release(m_pFont);
	Safe_Release(m_pObserver);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	return CUI::Free();
}
