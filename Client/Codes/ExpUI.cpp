#include "stdafx.h"
#include "..\Headers\ExpUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"
#include "Object_Manager.h"
#include "Player.h"

#include "Input_Device.h" // 삭제

#include "MyFont.h"


CExpUI::CExpUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

HRESULT CExpUI::Ready_Observer(void)
{
	m_pObserver = CUIObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CExpUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;
	
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Exp");
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

HRESULT CExpUI::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
	return E_FAIL;

		
	m_fGlowSizeX = 512.f;
	m_fGlowSizeY = 128.f;
	m_fGlowX = BACKSIZEX - m_fGlowSizeX / 2.f;
	m_fGlowY = m_fGlowSizeY / 1.5f;

	m_fBloodSizeX = 512.f;
	m_fBloodSizeY = 128.f;
	m_fBloodX = BACKSIZEX - m_fBloodSizeX / 2.f;
	m_fBloodY = m_fBloodSizeY / 1.5f;

	m_fMinX = BACKSIZEX + m_fGlowSizeX / 2.f;
	m_fMaxX = BACKSIZEX - m_fGlowSizeX / 2.f;


	return NOERROR;
}

_int CExpUI::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);


	if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_Y))
	{
		// 카메라의 월드행렬
		m_IsRender = true;
		m_fRenderTime = 3.f;
	}

	MoveTo_UI(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this, RENDER_TOP);

	return 0;
}

void CExpUI::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_ExpGlowConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(TYPE_COLOR);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_ExpBloodConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	pEffect->End();

	if (m_fBloodX != m_fMinX)
	{
		m_pFont->Set_FontSetting(64.f / 2.f, 79.f / 2.f, 0.f, _vec4(1.f, 1.f, 1.f, m_fAlpha));
		m_pFont->Draw2DFont(m_fBloodX, m_fBloodY*0.5f, "Exp: ");
	}


}

HRESULT CExpUI::SetUp_ExpGlowConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fGlowSizeX;
	matView._22 = m_fGlowSizeY;
	matView._33 = 1.f;
	matView._41 = m_fGlowX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fGlowY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetVector("g_vColor",&_vec4(115.f/255.f,1.f,0.f,1.f));

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture");

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CExpUI::SetUp_ExpBloodConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fBloodSizeX;
	matView._22 = m_fBloodSizeY;
	matView._33 = 1.f;
	matView._41 = m_fBloodX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fBloodY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture",1);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

void CExpUI::MoveTo_UI(const _float & fTimeDelta)
{
	if (m_IsRender == true)
	{
		//전체의 거리를 1이라고 봤을때 현재위치에서의 이동값 // 멀수록 빨라야하며 가까울수록 느려야함.

		_float fDistance = abs(m_fMaxX - m_fMinX); // 이동거리

		_float fCurDist = abs(m_fMaxX - m_fBloodX); // 현재 거리 

		_float fSpeed = fCurDist / fDistance * 10;
		
		m_fBloodX += -fTimeDelta * 1000.f * fSpeed;
		m_fGlowX += -fTimeDelta * 1000.f * fSpeed;

		if (m_fBloodX <= m_fMaxX || m_fGlowX <= m_fMaxX)
		{
			m_fBloodX = m_fMaxX;
			m_fGlowX = m_fMaxX;
		}

		m_fRenderTime -= fTimeDelta;

	}
	else
	{
		m_fBloodX += fTimeDelta * 1000.f;
		m_fGlowX += fTimeDelta * 1000.f;

		if (m_fBloodX >= m_fMinX || m_fGlowX >= m_fMinX)
		{
			m_fBloodX = m_fMinX;
			m_fGlowX = m_fMinX;
		}
	}	

	if (m_fRenderTime <= 0)
	{
		m_IsRender = false;
		m_fRenderTime = 3.f;
	}

}
CExpUI * CExpUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CExpUI *	pInstance = new CExpUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CExpUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CExpUI::Free(void)
{
	//
	Safe_Release(m_pFont);
	Safe_Release(m_pObserver);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	return CUI::Free();
}
