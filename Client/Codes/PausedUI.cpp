#include "stdafx.h"
#include "..\Headers\PausedUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"

#include "Input_Device.h" // 삭제
#include "MyFont.h"
#include "OptionUI.h"
#include "AbilityUI.h"

CPausedUI::CPausedUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

HRESULT CPausedUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Option");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CShader_UI");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	pComponent = m_pMenuTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_OptionMenu");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_MenuTexture", pComponent));
	m_pMenuTextureCom->AddRef();

	pComponent = m_pKeyTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Texture_KeyIcon");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_KeyTexture", pComponent));
	m_pKeyTextureCom->AddRef();
	
	
	m_pFont = CMyFont::Create(m_pGraphicDev, L"CTexture_MarioFont", 17.f, 6.f);
	m_pOptionFont = CMyFont::Create(m_pGraphicDev, L"CTexture_MarioFont", 17.f, 6.f);

	return NOERROR;
}

HRESULT CPausedUI::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;


	m_pOptionUI = COptionUI::Create(m_pGraphicDev);
	m_pAbilityUI = CAbilityUI::Create(m_pGraphicDev);


	m_fAlpha = 0.7f;
	
	m_vIconSize = { 0.f,0.f };
	
	m_vOptionSize = { 0.f,0.f };

	m_vIconMaxSize.x = 88.f;
	m_vIconMaxSize.y = 88.f;
	
	m_pFont->Set_FontSetting(m_vIconMaxSize.x*0.4f, m_vIconMaxSize.y, -10.f, _vec4(0.7f, 0.7f, 0.7f, 1.f));
	m_pOptionFont->Set_FontSetting(m_vIconMaxSize.x*0.4f, m_vIconMaxSize.y, -10.f, _vec4(0.f, 1.f, 0.f, 1.f));

	for (_int i = 0; i < 4 ;i++)
	{
		m_vIconMaxPos[i].x = BACKSIZEX / 16 * 2;
		m_vIconMaxPos[i].y = (BACKSIZEY / 3.f + (i * (m_vIconMaxSize.x*2) + 10.f)) * 0.5f;

		m_vIconPos[i].x = BACKSIZEX * 0.5f;
		m_vIconPos[i].y = BACKSIZEY * 0.5f;
	}

	for (_int i = 0; i < 2; i++)
	{
		m_vKeySize[i] = {30.f +(25*i),30.f+(25 * i)};
		m_vKeyPos[i] = { (_float)(BACKSIZEX / 9) * (7+i), (_float)(BACKSIZEY / 13.f) * 12.f -(12.5f*i) };
	}



	return NOERROR;
}

_int CPausedUI::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);


	if(m_IsMove == true)
		Move_To_UI(fTimeDelta);
	
	Check_InputState(fTimeDelta);
	
	if (nullptr != m_pRendererCom && m_IsRender == true)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this, RENDER_PAUSE);

	m_pAbilityUI->Update_GameObject(fTimeDelta);
	m_pOptionUI->Update_GameObject(fTimeDelta);	

	return 0;
}

void CPausedUI::Render_GameObject(void)
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

	for (_int i = 0; i < 4; i++)
	{
		SetUp_IconConstantTable(pEffect, i);

		pEffect->BeginPass(TYPE_ALPHA);
		m_pBufferCom->Render_Buffer();
		pEffect->EndPass();
	}
	if (m_IsInputCheck == true)
	{
		for (_int i = 0; i < 2; i++)
		{
			SetUp_KeyConstantTable(pEffect, i);

			pEffect->BeginPass(TYPE_ALPHA);
			m_pBufferCom->Render_Buffer();
			pEffect->EndPass();
		}

	}
	pEffect->End();
	
	if (m_IsInputCheck == true )
	{
		for (_int i = 0; i < 2; i++)
		{
			if (i == 0)
				m_pOptionFont->Draw2DFont(m_vKeyPos[i].x - m_pOptionFont->Get_FontInfo()->fWidth * 4, m_vKeyPos[0].y, "Back");
			else
				m_pOptionFont->Draw2DFont(m_vKeyPos[i].x - m_pOptionFont->Get_FontInfo()->fWidth * 5, m_vKeyPos[0].y, "Select");
		}
	}


	if (m_IsInputCheck == true && m_IsSelectState == false)
	{
		m_pOptionFont->Set_FontSetting(m_vIconMaxSize.x*0.4f, m_vIconMaxSize.y, -10.f, _vec4(0.f, 1.f, 0.f, 1.f));

		m_pOptionFont->Draw2DFont(BACKSIZEX / 9 * 7, BACKSIZEY / 13.f, "GAME PAUSED");
		
		m_pOptionFont->Set_FontSetting(m_vKeySize[0].x, m_vKeySize[0].y, -10.f, _vec4(0.f, 1.f, 0.f, 1.f));

		if (m_iSelectIndex == 0)
			m_pFont->Draw2DFont(m_vIconPos[m_iSelectIndex].x + m_vIconSize.x, m_vIconPos[m_iSelectIndex].y, "RESUME");
		else if (m_iSelectIndex == 1)
			m_pFont->Draw2DFont(m_vIconPos[m_iSelectIndex].x + m_vIconSize.x, m_vIconPos[m_iSelectIndex].y, "STATS");
		else if (m_iSelectIndex == 2)
			m_pFont->Draw2DFont(m_vIconPos[m_iSelectIndex].x + m_vIconSize.x, m_vIconPos[m_iSelectIndex].y, "OPTIONS");
		else
			m_pFont->Draw2DFont(m_vIconPos[m_iSelectIndex].x + m_vIconSize.x, m_vIconPos[m_iSelectIndex].y, "EXIT TO MAIN MENU");

	}		



}

HRESULT CPausedUI::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_vOptionSize.x;
	matView._22 = m_vOptionSize.y;
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

HRESULT CPausedUI::SetUp_IconConstantTable(LPD3DXEFFECT pEffect, const _int & iCurNum)
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
	matView._41 = m_vIconPos[iCurNum].x  - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_vIconPos[iCurNum].y  + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	if (iCurNum == m_iSelectIndex && m_IsInputCheck == true && m_IsSelectState == false)
		m_fIconAlpha = 1.f;
	else
		m_fIconAlpha = 0.3f;

	pEffect->SetFloat("g_fAlpha", m_fIconAlpha);

	m_pMenuTextureCom->SetUp_OnShader(pEffect, "g_UITexture", iCurNum);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CPausedUI::SetUp_KeyConstantTable(LPD3DXEFFECT pEffect, const _int & iCurNum)
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

	if(iCurNum == 0)
	m_pKeyTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 46);
	else
		m_pKeyTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 41);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

void CPausedUI::Move_To_UI(const _float & fTimeDelta)
{
	if (m_IsAction == true)
	{
		_int iCount = 0;
		_float fResult = (_float)((m_fCumulativeTime == 1.0) ? m_fCumulativeTime : 1 - pow(2, -10 * m_fCumulativeTime));

		m_vOptionSize.x = fResult * m_vOptionMaxSize.x;
		m_vOptionSize.y = fResult * m_vOptionMaxSize.y;

		for (_int i = 0; i < 4; i++)
		{
			m_vIconPos[i].x = fResult * m_vIconMaxPos[i].x + (BACKSIZEX*0.5f - (BACKSIZEX*0.5f * fResult));
			m_vIconPos[i].y = fResult * m_vIconMaxPos[i].y + (BACKSIZEY*0.5f - (BACKSIZEY*0.5f * fResult));
		}

		m_vIconSize.x = fResult * m_vIconMaxSize.x;
		m_vIconSize.y = fResult * m_vIconMaxSize.y;

		if (m_vOptionSize.x >= m_vOptionMaxSize.x)
		{
			m_vOptionSize.x = m_vOptionMaxSize.x;
			iCount += 1;
		}
		if (m_vOptionSize.y >= m_vOptionMaxSize.y)
		{
			m_vOptionSize.y = m_vOptionMaxSize.y;
			iCount += 1;
		}
		if (m_vIconSize.x >= m_vIconMaxSize.x)
		{
			m_vIconSize.x = m_vIconMaxSize.x;
			iCount += 1;
		}
		if (m_vIconSize.y >= m_vIconMaxSize.y)
		{
			m_vIconSize.y = m_vIconMaxSize.y;
			iCount += 1;
		}

		if(m_fCumulativeTime > 0.9f )
			m_fCumulativeTime += fTimeDelta * 10;
		else
			m_fCumulativeTime += fTimeDelta;

		if (iCount == 4)
		{
			m_fCumulativeTime = 1.f;
			m_IsMove = false;
			m_IsInputCheck = true;
			return;
		}
		else
			m_IsInputCheck = false;
	}
	else
	{
		m_IsInputCheck = false;

		_int iCount = 0;
		_float fResult = (_float)((m_fCumulativeTime == 1.0) ? m_fCumulativeTime : 1 - pow(2, -10 * m_fCumulativeTime));

		m_vOptionSize.x = fResult * m_vOptionMaxSize.x;
		m_vOptionSize.y = fResult * m_vOptionMaxSize.y;

		m_vIconSize.x = fResult * m_vIconMaxSize.x;
		m_vIconSize.y = fResult * m_vIconMaxSize.y;

		for (_int i = 0; i < 4; i++)
		{
			m_vIconPos[i].x = fResult * m_vIconMaxPos[i].x + (BACKSIZEX*0.5f - (BACKSIZEX*0.5f * fResult));
			m_vIconPos[i].y = fResult * m_vIconMaxPos[i].y + (BACKSIZEY*0.5f - (BACKSIZEY*0.5f * fResult));
		}

		if (m_vOptionSize.x <= 0)
		{
			m_vOptionSize.x = 0;
			iCount += 1;
		}
		if (m_vOptionSize.y <= 0)
		{
			m_vOptionSize.y = 0;
			iCount += 1;
		}
		if (m_vIconSize.x <= 0)
		{
			m_vIconSize.x = 0;
			iCount += 1;
		}
		if (m_vIconSize.y <= 0)
		{
			m_vIconSize.y = 0;
			iCount += 1;
		}

		if (iCount == 4)
		{
			m_fCumulativeTime = 0.f;
			m_IsMove = false;
			m_IsRender = false;
			m_iSelectIndex = 0;
			return;
		}

		if (m_fCumulativeTime > 0.9f)
			m_fCumulativeTime -= fTimeDelta * 10;
		else
			m_fCumulativeTime -= fTimeDelta * 2.f;
		//m_fCumulativeTime -= fTimeDelta*2.f;
	}
}
void CPausedUI::Check_InputState(const _float & fTimeDelta)
{

	if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_ESCAPE))
	{

		if (m_IsSelectState == false)
		{
			m_IsMove = true;
			m_IsRender = true;
			m_IsAction = !m_IsAction;			
		}
		else
		{
			m_pOptionUI->Set_ActivateOption(false);
			m_pAbilityUI->Set_ActivateOption(false);
			m_IsSelectState = false;
			m_IsInputCheck = true;			
		}
	}

	if (m_IsInputCheck == true)
	{
		if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_DOWN))
		{
			m_iSelectIndex++;
		}
		if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_UP))
		{
			m_iSelectIndex--;
		}

		if (m_iSelectIndex < 0)
			m_iSelectIndex = 3;
		else if (m_iSelectIndex > 3)
			m_iSelectIndex = 0;
		
		if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_RETURN))
		{
			switch (m_iSelectIndex)
			{
			case 0:
				m_IsMove = true;
				m_IsRender = true;
				m_IsAction = false;			
				break;
			case 1:			
				m_pAbilityUI->Set_ActivateOption(true);
				m_IsSelectState = true;
				m_IsInputCheck = false;
				break;
			case 2:
				m_pOptionUI->Set_ActivateOption(true);
				m_IsSelectState = true;
				m_IsInputCheck = false;
				break;
			case 3:
				m_IsSelectState = true;
				m_IsInputCheck = false;
				break;
			}
		}
	}

}
CPausedUI * CPausedUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPausedUI *	pInstance = new CPausedUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CPausedUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CPausedUI::Free(void)
{
	Safe_Release(m_pKeyTextureCom);
	Safe_Release(m_pMenuTextureCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFont);
	Safe_Release(m_pOptionFont);
	Safe_Release(m_pOptionUI);
	Safe_Release(m_pAbilityUI);

	return CUI::Free();
}
