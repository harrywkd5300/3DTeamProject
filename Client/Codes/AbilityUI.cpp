#include "stdafx.h"
#include "..\Headers\AbilityUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"

#include "Input_Device.h"
#include "MyFont.h"


CAbilityUI::CAbilityUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

HRESULT CAbilityUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_OptionAbility");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CShader_UI");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	pComponent = m_pKeyTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Texture_KeyIcon");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_KeyTexture", pComponent));
	m_pKeyTextureCom->AddRef();


	m_pKeyFont = CMyFont::Create(m_pGraphicDev, L"CTexture_MarioFont", 17.f, 6.f);

	return NOERROR;
}

HRESULT CAbilityUI::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_int i = 0; i < 2; i++)
	{
		m_vKeySize[i] = { 30.f + (25 * i), 30.f + (25 * i) };
		m_vKeyPos[i] = { (_float)(BACKSIZEX / 9) * (7 + i), (_float)(BACKSIZEY / 13.f) * 12.f - (12.5f*i) };
	}


	return NOERROR;
}

_int CAbilityUI::Update_GameObject(const _float & fTimeDelta)
{
	if (m_IsActivate == false)
	{
		m_IsUpdate = false;	
	}

	if (m_IsUpdate == true)
	{
		CUI::Update_GameObject(fTimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this, RENDER_PAUSE);
	}
	if (m_IsActivate == true)
	{
		m_IsUpdate = true;		
	}

	return 0;
}

void CAbilityUI::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_ConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	for (_int i = 0; i < 2; i++)
	{
		SetUp_KeyConstantTable(pEffect, i);

		pEffect->BeginPass(TYPE_ALPHA);
		m_pBufferCom->Render_Buffer();
		pEffect->EndPass();
	}
	
	pEffect->End();

	m_pKeyFont->Set_FontSetting(m_vKeySize[0].x, m_vKeySize[0].y, -10.f, _vec4(0.f, 1.f, 0.f, 1.f));

	for (_int i = 0; i < 2; i++)
	{		
		if (i == 0)
			m_pKeyFont->Draw2DFont(m_vKeyPos[i].x - m_pKeyFont->Get_FontInfo()->fWidth * 4, m_vKeyPos[0].y, "Back");
		else
			m_pKeyFont->Draw2DFont(m_vKeyPos[i].x - m_pKeyFont->Get_FontInfo()->fWidth * 6, m_vKeyPos[0].y, "Purchase");
	}
}

HRESULT CAbilityUI::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

HRESULT CAbilityUI::SetUp_KeyConstantTable(LPD3DXEFFECT pEffect, _int iCurNum)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_vKeySize[iCurNum].x;
	matView._22 = m_vKeySize[iCurNum].y;
	matView._33 = 1.f;
	matView._41 = m_vKeyPos[iCurNum].x - BACKSIZEX * 0.5f;
	matView._42 = -m_vKeyPos[iCurNum].y + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	if (iCurNum == 0)
		m_pKeyTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 46);
	else
		m_pKeyTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 41);

	pEffect->CommitChanges();

	Safe_Release(pEffect);
	return NOERROR;
}

CAbilityUI * CAbilityUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAbilityUI *	pInstance = new CAbilityUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CAbilityUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CAbilityUI::Free(void)
{
	Safe_Release(m_pKeyTextureCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pKeyFont);

	return CUI::Free();
}
