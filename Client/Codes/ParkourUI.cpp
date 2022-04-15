#include "stdafx.h"
#include "..\Headers\ParkourUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"
#include "Object_Manager.h"
#include "Player.h"

#include "Input_Device.h" // 삭제

#include "MyFont.h"


CParkourUI::CParkourUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

HRESULT CParkourUI::Ready_Observer(void)
{
	m_pObserver = CUIObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CParkourUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Hint");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CShader_UI");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();
	
	// For.Transform
	pComponent = m_pLogoTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_LogoTransform", pComponent));
	m_pLogoTransformCom->AddRef();
	
	m_pFont = CMyFont::Create(m_pGraphicDev, L"CTexture_SeoulNamsanFont", 16.f, 6.f);

	return NOERROR;
}

HRESULT CParkourUI::Ready_GameObject(_vec3 vPos, _vec3 vAngle)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
		return E_FAIL;

	m_pTransformCom->Set_Infomation(CTransform::INFO_SCALE, &_vec3(0.62f / 3.f, 0.70f / 3.f, 1.f));
	m_pLogoTransformCom->Set_Infomation(CTransform::INFO_SCALE, &_vec3(0.68f / 2.f, 0.26f / 2.f, 1.f));
	
	m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE,&vAngle);
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vPos);
	m_pLogoTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &vAngle);
		 
	m_fAngleY = vAngle.y;
	m_fOriginAngleY = vAngle.y;

	m_vOriginalDir = *m_pTransformCom->Get_WorldLookVec_FORCED();

	D3DXVec3Normalize(&m_vOriginalDir, &m_vOriginalDir);

	return NOERROR;
}

_int CParkourUI::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);
	
	

	Set_Direction();

	// 현재내가 사라지는 상황일때 다시 그려져야하는 상황이라면 그리지 않고 조건이 만족되면 그린다.
	// ex) m_IsRender == false 일때 
			
	MoveTo_UI(fTimeDelta);

	if (nullptr != m_pRendererCom && m_IsRender == true)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this, RENDER_RELATIONLESS);

	return 0;
}

void CParkourUI::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_IconConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(TYPE_DEPTH);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_LogoConstantTable(pEffect);
	pEffect->BeginPass(TYPE_DEPTH);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	pEffect->End();

	m_pFont->Set_FontSetting(64.f / 4.f, 79.f / 5.5f, 0.f, _vec4(1.f, 1.f, 1.f, m_fFontAlpha));
	m_pFont->Draw3DFont(&m_vFontScale, &m_vFontAngle, &m_vFontPos, "%02dm%02d", m_iTenthDigit,m_iUnitsDigit);
	
}

HRESULT CParkourUI::SetUp_IconConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();
		
	m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 0);
	
	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CParkourUI::SetUp_LogoConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pLogoTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	

	pEffect->SetFloat("g_fAlpha", m_fLogoAlpha);
	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 2);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

void CParkourUI::Set_Direction()
{
	_vec3 vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

	_vec3 vRight = *m_pTransformCom->Get_WorldRightVec_FORCED();
	_vec3 vLook = *m_pTransformCom->Get_WorldLookVec_FORCED();
	_vec3 vUp = *m_pTransformCom->Get_WorldUpVec_FORCED();

	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);

	m_pLogoTransformCom->Set_Infomation(CTransform::INFO_POSITION, &(vPos + (vRight * m_vLogoMove.x * m_fDir + vUp * m_vLogoMove.y)));

	m_vFontScale = { 1.5f,1.5f,1.5f };
	m_vFontAngle = { 0.f,m_fAngleY,0.f };

	if(m_fDir > 0)
	m_vFontPos = (vPos + (vRight * m_vFontMove.x * m_fDir + vUp * m_vFontMove.y));
	else
		m_vFontPos = (vPos + (vRight * m_vFontMove.x * m_fDir * 3 + vUp * m_vFontMove.y));

	_vec3 vTargetPos = *m_pObserver->Get_CharPos();
	vTargetPos.y = 0;
	vPos.y = 0;
	_vec3 vLength = vTargetPos - vPos;
	_float fLength = D3DXVec3Length(&vLength);
	fLength *= 100.f;

	m_iTenthDigit = (_int)fLength / 100;
	m_iUnitsDigit = (_int)fLength % 100;

	if (m_iTenthDigit <= 5.f)
	{
		if (m_fAlpha <= 0)
		{
			m_IsAction = true;
		}
	}
	else
	{
		if (m_fAlpha >= 1)
		{
			m_IsAction = false;
		}
	}


	if (m_IsAction == true && m_fLogoAlpha >= 1.f)
	{
		D3DXVec3Normalize(&vLength, &vLength);

		_float fDir = D3DXVec3Dot(&m_vOriginalDir, &vLength);

		if (fDir > 0)
		{
			m_fOriginAngleY += 180;
			m_fAngleY += 180;
			m_vOriginalDir *= -1;

			m_fDir *= -1;
		}
	}
}

void CParkourUI::MoveTo_UI(const _float & fTimeDelta)
{
	if (m_IsAction == false) // 플레이어가 액션을 취했을때 or 보이지 않는 위치에 있을때 
	{
		m_fFontAlpha -= fTimeDelta * 5;
		m_vFontMove.x += fTimeDelta;
		if (m_vFontMaxMove.x <= m_vFontMove.x)
			m_vFontMove.x = m_vFontMaxMove.x;

		if (m_fFontAlpha <= 0)
		{
			m_fFontAlpha = 0;

			m_fLogoAlpha -= fTimeDelta * 5;
			m_vLogoMove.x -= fTimeDelta ;
			if (m_vLogoMinMove.x >= m_vLogoMove.x)
				m_vLogoMove.x = m_vLogoMinMove.x;
		}

		if (m_fLogoAlpha <= 0)
		{
			m_fLogoAlpha = 0;
			m_fAngleY -= fTimeDelta * 400.f;
			//if (m_fAngleY <= m_fOriginAngleY - 180.f)
			//	m_fAngleY = m_fOriginAngleY - 180.f;

			m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(0.f, m_fAngleY, 0.f));
		}

		if (m_fAngleY <= m_fOriginAngleY - 180.f)
		{
			m_fAlpha -= fTimeDelta * 10;
		}

		if (m_fAlpha <= 0)
		{
			m_fAlpha = 0.f;
			m_fAngleY = m_fOriginAngleY - 180;
			m_fFontAlpha = 0.f;
			m_fLogoAlpha = 0.f;
			m_vLogoMove.x = m_vLogoMaxMove.x;
			m_vFontMove.x = m_vFontMinMove.x;
			m_vLogoMove.y = m_vLogoMaxMove.y;
			m_vFontMove.y = m_vFontMaxMove.y;
			//m_fLogoAngleX = 90.f;		
			m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(0.f, m_fAngleY, 0.f));
			m_pLogoTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(0.f, m_fAngleY, 0.f));

			m_IsRender = false;
		}		
	}
	else
	{
		m_IsRender = true;
		
		m_fAlpha += fTimeDelta * 10;	

		if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;

		m_fAngleY += fTimeDelta * 400.f;		

		if (m_fAngleY >= m_fOriginAngleY)
		{
			m_fAngleY = m_fOriginAngleY;	

			m_fFontAlpha += fTimeDelta * 10;

			if (m_fFontAlpha >= 1.f)
				m_fFontAlpha = 1.f;

			m_vFontMove.y -= fTimeDelta ;			

			if (m_vFontMove.y <= m_vFontMinMove.y)
				m_vFontMove.y = m_vFontMinMove.y;
		}

		if (m_vFontMove.y == m_vFontMinMove.y)
		{
			m_fLogoAlpha += fTimeDelta * 10;
			
			if (m_fLogoAlpha >= 1.f)
				m_fLogoAlpha = 1.f;

			m_vLogoMove.y -= fTimeDelta ;

			if (m_vLogoMove.y <= m_vLogoMinMove.y)
				m_vLogoMove.y = m_vLogoMinMove.y;
		}
		
		m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(0.f, m_fAngleY, 0.f));
		m_pLogoTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(0.f, m_fAngleY, 0.f));
						
	}	
	
}
CParkourUI * CParkourUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vAngle)
{
	CParkourUI *	pInstance = new CParkourUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPos,vAngle)))
	{
		MSG_BOX("CParkourUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CParkourUI::Free(void)
{
	//
	Safe_Release(m_pFont);
	Safe_Release(m_pObserver);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pLogoTransformCom);

	return CUI::Free();
}
