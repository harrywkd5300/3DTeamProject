#include "stdafx.h"
#include "..\Headers\OptionUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"

#include "Input_Device.h" 
#include "MyFont.h"


COptionUI::COptionUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

HRESULT COptionUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_OptionCheck");
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


	m_pFont = CMyFont::Create(m_pGraphicDev, L"CTexture_MarioFont", 17.f, 6.f);
	m_pOptionFont = CMyFont::Create(m_pGraphicDev, L"CTexture_MarioFont", 17.f, 6.f);

	return NOERROR;
}

HRESULT COptionUI::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;
		
	m_vIconSize = {55.f,55.f};
	m_vIconPos[0] = { BACKSIZEX / 16 * 14, BACKSIZEY / 9.f * 2 };
	m_vScrollSize = { 62.f,32.f };
	m_vScrollPos[0] = { BACKSIZEX / 16 * 14, BACKSIZEY / 9.f * 1.5f };
	m_vScrollPos[1] = { BACKSIZEX / 16 * 14, BACKSIZEY / 9.f * 7.f };

	m_pOptionFont->Set_FontSetting(40, 120, -5.f, _vec4(0.9f, 0.9f, 0.9f, 1.f));


	for (_int i = 0; i < 2; i++)
	{
		m_vKeySize[i] = { 30.f + (30 * i),30.f + (30 * i) };
		m_vKeyPos[i] = { (_float)(BACKSIZEX / 9) * (7 + i), (_float)(BACKSIZEY / 13.f) * 12.f - (12.5f*i) };
	}


	return NOERROR;
}

_int COptionUI::Update_GameObject(const _float & fTimeDelta)
{
	if (m_IsActivate == false)
	{
		m_IsUpdate = false;
		m_iSelectIndex = 0;
	}

	if (m_IsUpdate == true)
	{
		CUI::Update_GameObject(fTimeDelta);

		Update_OptionSet();

		Check_InputState(fTimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this, RENDER_PAUSE);
	}

	if (m_IsActivate == true)
		m_IsUpdate = true;

	return 0;
}

void COptionUI::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	
	pEffect->Begin(nullptr, 0);

	for (_int i = 0; i < 5; i++)
	{
		SetUp_ConstantTable(pEffect,i,m_TextureNumArray[i]);
		pEffect->BeginPass(TYPE_ALPHA);
		m_pBufferCom->Render_Buffer();
		pEffect->EndPass();
	}
	for (_int i = 0; i < 2; i++)
	{
		SetUp_ScrollConstantTable(pEffect, i);
		pEffect->BeginPass(TYPE_ALPHA);
		m_pBufferCom->Render_Buffer();
		pEffect->EndPass();

		SetUp_KeyConstantTable(pEffect, i);
		pEffect->BeginPass(TYPE_ALPHA);
		m_pBufferCom->Render_Buffer();
		pEffect->EndPass();
	}
	pEffect->End();

	
	m_pFont->Set_FontSetting(m_vIconSize.x*0.4f, m_vIconSize.y, -5.f, m_vColor[0]);
	m_pFont->Draw2DFont(m_vIconPos[0].x - m_vIconSize.x * 0.5f - m_pFont->Get_FontInfo()->fWidth * 3, m_vIconPos[0].y, "SSAO");

	m_pFont->Set_FontSetting(m_vIconSize.x*0.4f, m_vIconSize.y, -5.f, m_vColor[1]);
	m_pFont->Draw2DFont(m_vIconPos[1].x - m_vIconSize.x * 0.5f - m_pFont->Get_FontInfo()->fWidth * 4.5f, m_vIconPos[1].y, "SHADOW");

	m_pFont->Set_FontSetting(m_vIconSize.x*0.4f, m_vIconSize.y, -5.f, m_vColor[2]);
	m_pFont->Draw2DFont(m_vIconPos[2].x - m_vIconSize.x * 0.5f - m_pFont->Get_FontInfo()->fWidth * 7.5f, m_vIconPos[2].y, "MOTIONBULR");

	m_pFont->Set_FontSetting(m_vIconSize.x*0.4f, m_vIconSize.y, -5.f, m_vColor[3]);
	m_pFont->Draw2DFont(m_vIconPos[3].x - m_vIconSize.x * 0.5f - m_pFont->Get_FontInfo()->fWidth * 10.5f, m_vIconPos[3].y, "EDGEDECTECTING");

	m_pFont->Set_FontSetting(m_vIconSize.x*0.4f, m_vIconSize.y, -5.f, m_vColor[4]);
	m_pFont->Draw2DFont(m_vIconPos[4].x - m_vIconSize.x * 0.5f - m_pFont->Get_FontInfo()->fWidth * 2, m_vIconPos[4].y, "HDR");
	
	m_pOptionFont->Set_FontSetting(m_vKeySize[0].x, m_vKeySize[0].y, -10.f, _vec4(0.f, 1.f, 0.f, 1.f));

	for (_int i = 0; i < 2; i++)
	{		
		if (i == 0)
			m_pOptionFont->Draw2DFont(m_vKeyPos[i].x - m_pOptionFont->Get_FontInfo()->fWidth * 4, m_vKeyPos[0].y, "Back");
		else
			m_pOptionFont->Draw2DFont(m_vKeyPos[i].x - m_pOptionFont->Get_FontInfo()->fWidth * 5, m_vKeyPos[0].y, "Adjust");
	}
	


	m_pOptionFont->Set_FontSetting(40, 120, -5.f, _vec4(0.9f, 0.9f, 0.9f, 1.f));
	m_pOptionFont->Draw2DFont(BACKSIZEX / 9 * 7, BACKSIZEY / 13.f, "OPTIONS");
		
}

HRESULT COptionUI::SetUp_ConstantTable(LPD3DXEFFECT pEffect, _int iCurNum, _int iTexture)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_vIconSize.x;
	matView._22 = m_vIconSize.y;
	matView._33 = 1.f;
	matView._41 = m_vIconPos[iCurNum].x - BACKSIZEX * 0.5f;
	matView._42 = -m_vIconPos[iCurNum].y + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_vColor[iCurNum].w);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", iTexture);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT COptionUI::SetUp_ScrollConstantTable(LPD3DXEFFECT pEffect, _int iCurNum)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_vScrollSize.x;
	matView._22 = m_vScrollSize.y;
	matView._33 = 1.f;
	matView._41 = m_vScrollPos[iCurNum].x - BACKSIZEX * 0.5f;
	matView._42 = -m_vScrollPos[iCurNum].y + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", 1.f);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", iCurNum+2);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT COptionUI::SetUp_KeyConstantTable(LPD3DXEFFECT pEffect, const _int & iCurNum)
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

	// 300
	matView._42 = -m_vKeyPos[iCurNum].y + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", 1.f);

	if (iCurNum == 0)
		m_pKeyTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 46);
	else
		m_pKeyTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 47);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

void COptionUI::Update_OptionSet()
{
	m_TextureNumArray[0] = (_int)m_pRendererCom->Get_ShaderOptionOnOff(CRenderer::ShaderOption_SSAO);
	m_TextureNumArray[1] = (_int)m_pRendererCom->Get_ShaderOptionOnOff(CRenderer::ShaderOption_Shadow);
	m_TextureNumArray[2] = (_int)m_pRendererCom->Get_ShaderOptionOnOff(CRenderer::ShaderOption_MotionBlur);
	m_TextureNumArray[3] = (_int)m_pRendererCom->Get_ShaderOptionOnOff(CRenderer::ShaderOption_EdgeDetecting);
	m_TextureNumArray[4] = (_int)m_pRendererCom->Get_ShaderOptionOnOff(CRenderer::ShaderOption_HDR);


	for (_int i = 0; i < 5; i++)
	{
		if (i == m_iSelectIndex)
			m_vColor[i] = _vec4(0.f, 1.f, 0.f, 1.f);
		else
			m_vColor[i] = _vec4(0.f, 1.f, 0.f, 0.3f);

		if (i > 0)
		{
			m_vIconPos[i].x = m_vIconPos[0].x;
			m_vIconPos[i].y = m_vIconPos[0].y + (i * (m_vIconSize.x + 10));
		}

	}

}
void COptionUI::Check_InputState(const _float & fTimeDelta)
{
	if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_UP))
	{
		m_iSelectIndex--;
	}
	if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_DOWN))
	{
		m_iSelectIndex++;
	}
	
	if (m_iSelectIndex < 0)
		m_iSelectIndex = 4;
	else if (m_iSelectIndex > 4)
		m_iSelectIndex = 0;

	if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_RETURN))
	{
		Check_Option(m_iSelectIndex);
	}
}
void COptionUI::Check_Option(_int iSelect)
{
	switch (iSelect)
	{
	case 0:
		m_pRendererCom->Set_ShaderOptionOnOff(CRenderer::ShaderOption_SSAO, (_bool)!m_TextureNumArray[iSelect]);
		break;
	case 1:
		m_pRendererCom->Set_ShaderOptionOnOff(CRenderer::ShaderOption_Shadow, (_bool)!m_TextureNumArray[iSelect]);
		break;
	case 2:
		m_pRendererCom->Set_ShaderOptionOnOff(CRenderer::ShaderOption_MotionBlur, (_bool)!m_TextureNumArray[iSelect]);
		break;
	case 3:
		m_pRendererCom->Set_ShaderOptionOnOff(CRenderer::ShaderOption_EdgeDetecting, (_bool)!m_TextureNumArray[iSelect]);
		break;
	case 4:
		m_pRendererCom->Set_ShaderOptionOnOff(CRenderer::ShaderOption_HDR, (_bool)!m_TextureNumArray[iSelect]);
		break;	
	}
}
COptionUI * COptionUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	COptionUI *	pInstance = new COptionUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("COptionUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong COptionUI::Free(void)
{
	Safe_Release(m_pFont);
	Safe_Release(m_pOptionFont);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pKeyTextureCom);
	Safe_Release(m_pShaderCom);

	return CUI::Free();
}
