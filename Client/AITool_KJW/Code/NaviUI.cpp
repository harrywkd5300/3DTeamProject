#include "stdafx.h"
#include "..\Headers\NaviUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"
#include "Object_Manager.h"
#include "Player.h"


CNaviUI::CNaviUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{

}

HRESULT CNaviUI::Ready_Observer(void)
{
	m_pObserver = CUIObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CNaviUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;//CTexture_Hint//CTexture_Navi
			
	pComponent = m_pArrowTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_ArrowTransform", pComponent));
	m_pArrowTransformCom->AddRef();

	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Navi");
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

HRESULT CNaviUI::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
		return E_FAIL;

	m_fIconSizeX = 720.f / 4.f;
	m_fIconSizeY = 700.f / 4.f;
	m_fIconX = BACKSIZEX - BACKSIZEX / 10.f;
	m_fIconY = BACKSIZEY / 4.f ;


	m_fIconMaxSizeX = 720.f / 3.5f;
	m_fIconMaxSizeY = 700.f / 3.5f;
	m_fIconMinSizeX = 720.f / 4.f;
	m_fIconMinSizeY = 700.f / 4.f;
		
	m_pArrowTransformCom->Set_Infomation(CTransform::INFO_SCALE, &_vec3(0.94f,1.24f, 0.f));

	return NOERROR;
}

_int CNaviUI::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);
	

	CWorldState* pWorldState = m_pObserver->Get_Current_WorldState();

	if (pWorldState != nullptr)
	{
		if (pWorldState->getVariable(CH_STATE_IDLE))
		{
			m_fIdleTime += fTimeDelta;
		}
		else
			m_fIdleTime = 0.f;
	}
	
	
	if (m_fIdleTime > 5.f)
	{
		m_IsRender = true;
		m_fAlpha = 1.f;
	}		
	else
	{
		m_fAlpha -= fTimeDelta;

		if (m_fAlpha < 0)
		{
			m_fAlpha = 0;
			m_IsRender = false;
		}
			
	}

	Set_ArrowPosition();
	MoveTo_UI(fTimeDelta);

	


	if (nullptr != m_pRendererCom &&  m_IsRender == true)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this,RENDER_LOWEST);

	return 0;
}

void CNaviUI::Render_GameObject(void)
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

	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_ArrowConstantTable(pEffect);
	pEffect->BeginPass(TYPE_DEPTH);
	m_pBufferCom->Render_Buffer();
	
	pEffect->EndPass();

	pEffect->End();	
}

HRESULT CNaviUI::SetUp_IconConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fIconSizeX;
	matView._22 = m_fIconSizeY;
	matView._33 = 1.f;
	matView._41 = m_fIconX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fIconY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture",0);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CNaviUI::SetUp_ArrowConstantTable(LPD3DXEFFECT pEffect)
{
	if (pEffect == nullptr)
		return E_FAIL;

	pEffect->AddRef();		
		
	m_pArrowTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);  // 셋팅된 카메라 x 현재 플레이어 카메라의 View
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);	

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
		
	pEffect->SetFloat("g_fUAlpha", 1.f);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture",1);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}
void CNaviUI::Set_ArrowPosition()
{
	if (m_iNaviIndex >= m_iNaviPointListSize)
		return;

	_vec3 vPos = *m_pObserver->Get_CharPos();		
	_vec3 vPoint = m_vecNaviPoint[m_iNaviIndex];

	// vPoint = m_NaviPointlist.front();
	
	_matrix matWorld = *m_pObserver->Get_Current_PlayerWorld();
	_vec3 vLook = { 0,0,1.f };
	_vec3 vDir = vPoint - vPos;
		

	D3DXVec3Normalize(&vDir, &vDir);
	D3DXVec3Normalize(&vLook, &vLook);

	_float fAngle = D3DXVec3Dot(&vDir,&vLook);

	_float fRadion = (_float)acos(fAngle);

	if (vDir.x < 0)
		fRadion *= -1;	

	_vec3 vArrowPos = vPos + vDir*m_fDirLength;

	vArrowPos.y = 0.3f; // 임시

	m_pArrowTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(90.f, D3DXToDegree(fRadion) , 0.f));
	m_pArrowTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vArrowPos);

}
void CNaviUI::MoveTo_UI(const _float & fTimeDelta)
{
	if (m_IsIconChange == true)
	{
		_int i = 0;
		m_fIconSizeX += fTimeDelta * 200;
		m_fIconSizeY += fTimeDelta * 200;

		if (m_fIconSizeX >= m_fIconMaxSizeX)
		{			
			m_fIconSizeX = m_fIconMaxSizeX;
			i++;
		}
		if (m_fIconSizeY >= m_fIconMaxSizeY)
		{
			m_fIconSizeY = m_fIconMaxSizeY;
			i++;
		}
		
		if (i == 2)		
			m_IsIconChange = false;		
	}
	else
	{
		_int i = 0;
		m_fIconSizeX -= fTimeDelta * 50;
		m_fIconSizeY -= fTimeDelta * 50;

		if (m_fIconSizeX <= m_fIconMinSizeX)
		{
			m_fIconSizeX = m_fIconMinSizeX;
			i++;
		}
		if (m_fIconSizeY <= m_fIconMinSizeY)
		{
			m_fIconSizeY = m_fIconMinSizeY;
			i++;
		}

		if (i == 2)
			m_IsIconChange = true;
	}


	if (m_IsArrowChange == true)
	{
		m_fDirLength += fTimeDelta * 3.f;

		if (m_fDirLength >= m_fDirMax)
		{
			m_fDirLength = m_fDirMax;
			m_IsArrowChange = false;
		}
			
	}
	else
	{
		m_fDirLength -= fTimeDelta ;

		if (m_fDirLength <= m_fDirMin)
		{
			m_fDirLength = m_fDirMin;
			m_IsArrowChange = true;
		}
	}



}
CNaviUI * CNaviUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNaviUI *	pInstance = new CNaviUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CNaviUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CNaviUI::Free(void)
{	
	Safe_Release(m_pArrowTransformCom);
	Safe_Release(m_pObserver);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	m_vecNaviPoint.clear();

	return CUI::Free();
}
