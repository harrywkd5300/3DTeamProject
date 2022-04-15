#include "stdafx.h"
#include "..\Headers\ProgressUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"
#include "Object_Manager.h"
#include "Player.h"
CProgressUI::CProgressUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

HRESULT CProgressUI::Ready_Observer(void)
{
	m_pObserver = CUIObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CProgressUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;
	
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Progress");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	pComponent = m_pDonTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Don");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_DonTexture", pComponent));
	m_pDonTextureCom->AddRef();

	pComponent = m_pLeoTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Leo");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_LeoTexture", pComponent));
	m_pLeoTextureCom->AddRef();

	pComponent = m_pMikeTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Mike");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_MikeTexture", pComponent));
	m_pMikeTextureCom->AddRef();

	pComponent = m_pRaphTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Raph");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_RaphTexture", pComponent));
	m_pRaphTextureCom->AddRef();


	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CShader_UI");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

HRESULT CProgressUI::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
		return E_FAIL;
	
	m_fProgressSizeX = 564.f;
	m_fProgressSizeY = 27.f;

	m_fProgressX = BACKSIZEX / 2.f;
	m_fProgressY = m_fProgressSizeY * 4.f;
	
	m_fProgressBarSizeX = 554.f;
	m_fProgressBarSizeY = 18.f;
	m_fProgressBarX = BACKSIZEX / 2.f;
	m_fProgressBarY = m_fProgressY;

	m_fCharacterSizeX = 200.f;
	m_fCharacterSizeY = 200.f;
	m_fCharacterX = m_fProgressX - m_fProgressSizeX / 2.f ;
	m_fCharacterY = m_fProgressSizeY * 2.f ;


	m_fMinCharY = -100.f;

	m_fMaxCharY = m_fCharacterY;

	return NOERROR;
}

_int CProgressUI::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);
	
	m_eCurType = *m_pObserver->Get_Current_Type();
	
	m_fCurFrame += fTimeDelta * m_fFrameSpeed ;
	


	CWorldState* pWorldState = m_pObserver->Get_Current_WorldState();

	if (pWorldState != nullptr)
	{
		if (pWorldState->getVariable(CH_STATE_RUN) || pWorldState->getVariable(CH_STATE_SPRINT))
		{
			m_eCurState = MOVE;
		}
		else
			m_eCurState = IDLE;
	}	

	if (m_fCurFrame >= m_fMaxFrame)
		m_fCurFrame = 0.f;	

	Update_State();

	//Moving_UI(fTimeDelta);
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this, RENDER_TOP);

	return 0;
}

void CProgressUI::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_ProgressConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_ProgressBarConstantTable(pEffect);
	pEffect->BeginPass(TYPE_GRADALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_CharacterConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();


	pEffect->End();
}


HRESULT CProgressUI::SetUp_CharacterConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fCharacterSizeX;
	matView._22 = m_fCharacterSizeY;
	matView._33 = 1.f;
	matView._41 = m_fCharacterX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fCharacterY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	if (m_eCurType == PLAYER_TYPE::PLAYER_Donatello)
		m_pDonTextureCom->SetUp_OnShader(pEffect, "g_UITexture", (_int)(m_fCurFrame + m_fAddFrame));
	if (m_eCurType == PLAYER_TYPE::PLAYER_Leonardo)
		m_pLeoTextureCom->SetUp_OnShader(pEffect, "g_UITexture", (_int)(m_fCurFrame + m_fAddFrame));
	if (m_eCurType == PLAYER_TYPE::PLAYER_Michelangelo)
		m_pMikeTextureCom->SetUp_OnShader(pEffect, "g_UITexture", (_int)(m_fCurFrame + m_fAddFrame));
	if (m_eCurType == PLAYER_TYPE::PLAYER_Raphael)
		m_pRaphTextureCom->SetUp_OnShader(pEffect, "g_UITexture", (_int)(m_fCurFrame + m_fAddFrame));


	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CProgressUI::SetUp_ProgressConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fProgressSizeX;
	matView._22 = m_fProgressSizeY;
	matView._33 = 1.f;
	matView._41 = m_fProgressX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fProgressY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture",2);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CProgressUI::SetUp_ProgressBarConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fProgressBarSizeX;
	matView._22 = m_fProgressBarSizeY;
	matView._33 = 1.f;
	matView._41 = m_fProgressBarX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fProgressBarY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	
	m_pTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture", 1);
	m_pTextureCom->SetUp_OnShader(pEffect, "g_GradTexture", 3);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	//거리에 따라 조절
	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->SetVector("g_vColor", &_vec4(1.f, 255.f / 215.f, 0.f, 1.f));
		

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

void CProgressUI::Update_State()
{
	if (m_eCurType != m_ePreType)
	{
		m_ePreType = m_eCurType;
		m_eCurState = IDLE;
		m_ePreState = STATE_NONE;
	}

	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case CProgressUI::IDLE:
			if (m_eCurType == PLAYER_TYPE::PLAYER_Donatello)
			{
				m_fMaxFrame = 4.f;
				m_fCurFrame = 0.f;
				m_fAddFrame = 0.f;
				m_fFrameSpeed = 10.f;
				m_fChangeFrame = 1.f;
			}
			else if (m_eCurType == PLAYER_TYPE::PLAYER_Leonardo)
			{
				m_fMaxFrame = 5.f;
				m_fCurFrame = 0.f;
				m_fAddFrame = 0.f;
				m_fFrameSpeed = 10.f;
				m_fChangeFrame = 1.f;
			}
			else if (m_eCurType == PLAYER_TYPE::PLAYER_Michelangelo)
			{
				m_fMaxFrame = 6.f;
				m_fCurFrame = 0.f;
				m_fAddFrame = 0.f;
				m_fFrameSpeed = 10.f;
				m_fChangeFrame = 1.f;
			}
			else //(m_eCurType == PLAYER_TYPE::PLAYER_Raphael)
			{
				m_fMaxFrame = 2.f;
				m_fCurFrame = 0.f;
				m_fAddFrame = 0.f;
				m_fFrameSpeed = 5.f;
				m_fChangeFrame = 1.f;
			}
			break;
		case CProgressUI::MOVE:
			if (m_eCurType == PLAYER_TYPE::PLAYER_Donatello)
			{
				m_fMaxFrame = 6.f;
				m_fCurFrame = 0.f;
				m_fAddFrame = 4.f;
				m_fFrameSpeed = 10.f;
				m_fChangeFrame = 1.f;
			}
			else if (m_eCurType == PLAYER_TYPE::PLAYER_Leonardo)
			{
				m_fMaxFrame = 4.f;
				m_fCurFrame = 0.f;
				m_fAddFrame = 5.f;
				m_fFrameSpeed = 10.f;
				m_fChangeFrame = 1.f;
			}
			else if (m_eCurType == PLAYER_TYPE::PLAYER_Michelangelo)
			{
				m_fMaxFrame = 10.f;
				m_fCurFrame = 0.f;
				m_fAddFrame = 6.f;
				m_fFrameSpeed = 20.f;
				m_fChangeFrame = 1.f;
			}
			else //(m_eCurType == PLAYER_TYPE::PLAYER_Raphael)
			{
				m_fMaxFrame = 6.f;
				m_fCurFrame = 0.f;
				m_fAddFrame = 2.f;
				m_fFrameSpeed = 10.f;
				m_fChangeFrame = 1.f;
			}
			break;

		}

		m_ePreState = m_eCurState;
	}
}

CProgressUI * CProgressUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CProgressUI *	pInstance = new CProgressUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CProgressUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CProgressUI::Free(void)
{
	Safe_Release(m_pDonTextureCom);
	Safe_Release(m_pLeoTextureCom);
	Safe_Release(m_pMikeTextureCom);
	Safe_Release(m_pRaphTextureCom);

	Safe_Release(m_pObserver);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	return CUI::Free();
}
