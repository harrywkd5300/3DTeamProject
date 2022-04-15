#include "stdafx.h"
#include "..\Headers\PlayerChangeUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"
#include "Object_Manager.h"
#include "Player.h"


CPlayerChangeUI::CPlayerChangeUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}
HRESULT CPlayerChangeUI::Ready_Observer(void)
{
	m_pObserver = CUIObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CPlayerChangeUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Swap");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture_Swap", pComponent));
	m_pTextureCom->AddRef();

	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CShader_UI");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	// For.Transform
	pComponent = m_pUITransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_UITransform", pComponent));
	m_pUITransformCom->AddRef();

	return NOERROR;
}

HRESULT CPlayerChangeUI::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
	return E_FAIL;

	m_vDirSize = _vec3(1.f, 1.f, 1.f);
	m_vDonSize = _vec3(1.f, 1.f, 1.f);
	m_vLeoSize = _vec3(1.f, 1.f, 1.f);
	m_vMikeSize = _vec3(1.f, 1.f, 1.f);
	m_vRaphSize = _vec3(1.f, 1.f, 1.f);

	m_fMaxSize = 1.f;
	m_fMinSize = 0.f;


	m_vDirPos = _vec3(0.f, 0.f, 0.f);
	
	m_vDonPos = _vec3(0.f, 1.f, 0.f);
	Set_LocalMatrix(&m_matDonLocal, &m_vDonSize, &m_vDonPos);
	m_vLeoPos = _vec3(-1.f, 0.f, 0.f);
	Set_LocalMatrix(&m_matLeoLocal, &m_vLeoSize, &m_vLeoPos);
	m_vMikePos = _vec3(0.f, -1.f, 0.f);
	Set_LocalMatrix(&m_matMikeLocal, &m_vMikeSize, &m_vMikePos);
	m_vRaphPos = _vec3(1.f, 0.f, 0.f);
	Set_LocalMatrix(&m_matRaphLocal, &m_vRaphSize, &m_vRaphPos);


	return NOERROR;
}

_int CPlayerChangeUI::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);
	
	m_eCurType = *m_pObserver->Get_Current_Type();
	
	if (m_eCurType != m_ePreType)
	{
		m_IsSelect = true;
		m_IsRender = true;
		m_fRenderTime = 3.f;
		m_fSelectTime = m_fMaxSelectTime;

		m_ePreType = m_eCurType;	
		
		Set_SelectInfo(m_eCurType);
	}

	MoveTo_UI(fTimeDelta);
	SelectMotion(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this, RENDER_TOP);

	return 0;
}

void CPlayerChangeUI::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_DirectionConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();	
	pEffect->EndPass();

	SetUp_SelectConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

		
	SetUp_DonConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_LeoConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_MikeConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_RaphConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	pEffect->End();
}

void CPlayerChangeUI::Set_LocalMatrix(_matrix * pMatWorld, _vec3 * vSize, _vec3 * vPos)
{
	m_pTransformCom->Set_Infomation(CTransform::INFO_SCALE, &m_vDirSize);
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &m_vDirPos);

	_matrix matTargetInvWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	D3DXMatrixInverse(&matTargetInvWorld, nullptr, &matTargetInvWorld);

	m_pUITransformCom->Set_Infomation(CTransform::INFO_SCALE, vSize);
	m_pUITransformCom->Set_Infomation(CTransform::INFO_POSITION, vPos);

	_matrix matWorld = m_pUITransformCom->Get_WorldMatrix_FORCED();

	*pMatWorld = matWorld * matTargetInvWorld;
}

void CPlayerChangeUI::Set_SelectInfo(PLAYER_TYPE eType)
{
	switch (eType)
	{
	case PLAYER_Donatello:
		m_fMaxLookSize = 11.5f;
		m_fMinLookSize = 11.f;
		m_fMaxRightSize = 9.f;
		m_fMinRightSize = 9.f;
		m_fMaxUpSize = -3.f; //+
		m_fMinUpSize = -3.5f;
		break;
	case PLAYER_Leonardo:
		m_fMaxLookSize = 11.5f;
		m_fMinLookSize = 11.f;
		m_fMaxRightSize = 8.5f; //-
		m_fMinRightSize = 9.f;
		m_fMaxUpSize = -3.5f;
		m_fMinUpSize = -3.5f;
		break;
	case PLAYER_Michelangelo:
		m_fMaxLookSize = 11.5f;
		m_fMinLookSize = 11.f;
		m_fMaxRightSize = 9.f;
		m_fMinRightSize = 9.f;
		m_fMaxUpSize = -4.f; //-
		m_fMinUpSize = -3.5f;
		break;
	case PLAYER_Raphael:
		m_fMaxLookSize = 11.5f;
		m_fMinLookSize = 11.f;
		m_fMaxRightSize = 9.5f; //+
		m_fMinRightSize = 9.f;
		m_fMaxUpSize = -3.5f;
		m_fMinUpSize = -3.5f;
		break;
	}
}

void CPlayerChangeUI::MoveTo_UI(const _float & fTimeDelta)
{
	if (m_IsRender == true)
	{
		m_fCurSize += fTimeDelta * 10.f;

		if (m_fCurSize >= m_fMaxSize)
		{
			m_fCurSize = m_fMaxSize;			
		}
		m_fRenderTime -= fTimeDelta;

	}
	else
	{		
		m_fCurSize -= fTimeDelta * 10.f;
		if (m_fCurSize <= m_fMinSize)
		{
			m_fCurSize = m_fMinSize;
		}
	}

	if (m_fRenderTime <= 0)
	{
		m_IsRender = false;
		m_fRenderTime = 3.f;
	}

}
void CPlayerChangeUI::SelectMotion(const _float & fTimeDelta)
{
	if (m_IsSelect == true)
	{
		if (m_eCurType == PLAYER_TYPE::PLAYER_Donatello)
		{		
			m_fUpSize += fTimeDelta * 10.f;			

			if (m_fUpSize >= m_fMaxUpSize)
			{					
				m_fUpSize = m_fMaxUpSize;
			}
		}
		else if (m_eCurType == PLAYER_TYPE::PLAYER_Leonardo)
		{
			m_fRightSize -= fTimeDelta * 10.f;		
		
			if (m_fRightSize <= m_fMaxRightSize)
			{			
				m_fRightSize = m_fMaxRightSize;						
			}
		}
		else if (m_eCurType == PLAYER_TYPE::PLAYER_Michelangelo)
		{			
			m_fUpSize -= fTimeDelta * 10.f;		

			if (m_fUpSize <= m_fMaxUpSize)
			{					
				m_fUpSize = m_fMaxUpSize;				
			}
		}
		else //(m_eCurType == PLAYER_TYPE::PLAYER_Raphael)
		{
			m_fRightSize += fTimeDelta * 10.f;	
			if (m_fRightSize >= m_fMaxRightSize)
			{			
				m_fRightSize = m_fMaxRightSize;					
			}
		}
		
		m_fSelectTime -= fTimeDelta;
	}
	else
	{
		if (m_eCurType == PLAYER_TYPE::PLAYER_Donatello)
		{			
			m_fUpSize -= fTimeDelta * 10.f;
			
			if (m_fUpSize <= m_fMinUpSize)
			{				
				m_fRightSize = m_fMinRightSize;
				m_fUpSize = m_fMinUpSize;			
			}
		}
		else if (m_eCurType == PLAYER_TYPE::PLAYER_Leonardo)
		{
			m_fRightSize += fTimeDelta * 10.f;			
			
			if (m_fRightSize >= m_fMinRightSize)
			{
				m_fRightSize = m_fMinRightSize;
				m_fUpSize = m_fMinUpSize;				
			}
		}
		else if (m_eCurType == PLAYER_TYPE::PLAYER_Michelangelo)
		{			
			m_fUpSize += fTimeDelta * 10.f;			

			if (m_fUpSize >= m_fMinUpSize)
			{
				m_fRightSize = m_fMinRightSize;
				m_fUpSize = m_fMinUpSize;			
			}
		}
		else //(m_eCurType == PLAYER_TYPE::PLAYER_Raphael)
		{
			m_fRightSize -= fTimeDelta * 10.f;			
		
			if (m_fRightSize <= m_fMinRightSize)
			{
				m_fRightSize = m_fMinRightSize;
				m_fUpSize = m_fMinUpSize;				
			}
		}		

	

	}

	if (m_fSelectTime <= 0)
	{
		m_IsSelect = false;
		m_fSelectTime = m_fMaxSelectTime;
	}
}
HRESULT CPlayerChangeUI::SetUp_DirectionConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();	 

	CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurrentCamera();
	_vec3 vAt = pCamera->Get_At();

	_vec3 vRight = *(_vec3*)&pCamera->Get_WorldMatrix().m[0];
	_vec3 vUp = *(_vec3*)&pCamera->Get_WorldMatrix().m[1];
	_vec3 vLook = *(_vec3*)&pCamera->Get_WorldMatrix().m[2];
	_vec3 vPos = *(_vec3*)&pCamera->Get_WorldMatrix().m[3];

	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vUp, &vUp);
		
	m_vDirPos = vPos + vLook * m_fLookSize + vRight * m_fRightSize + vUp * m_fUpSize;
	m_vDirSize = { 1.f,1.f,1.f };

	m_pTransformCom->Set_Infomation(CTransform::INFO_SCALE, &(m_vDirSize*m_fCurSize));
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &m_vDirPos);

	//_vec3 vAngle = *m_pTransformCom->Get_Infomation(CTransform::INFO_ANGLE);
		
	m_pTransformCom->Set_Angle(pCamera->Get_InverseViewMatrix());
	//m_pTransformCom->Rotation_X(vAngle.x, 1.f);
	//m_pTransformCom->Rotation_Y(vAngle.y, 1.f);
	//m_pTransformCom->Add_Infomation(CTransform::INFO_ANGLE, &_vec3(0.f,30.f,0.f)); // 오른쪽 
																		

	m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);  // 셋팅된 카메라 x 현재 플레이어 카메라의 View
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);


	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 9);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();

	Safe_Release(pEffect);



	return NOERROR;
}

HRESULT CPlayerChangeUI::SetUp_DonConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matDonLocal * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix	matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 0);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CPlayerChangeUI::SetUp_LeoConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLeoLocal * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix	matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 2);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CPlayerChangeUI::SetUp_MikeConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matMikeLocal * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix	matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 4);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CPlayerChangeUI::SetUp_RaphConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matRaphLocal * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix	matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 6);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CPlayerChangeUI::SetUp_SelectConstantTable(LPD3DXEFFECT pEffect)
{

	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;


	if (m_eCurType == PLAYER_TYPE::PLAYER_Donatello)
		matWorld = m_matDonLocal * matTargetWorld;
	if (m_eCurType == PLAYER_TYPE::PLAYER_Leonardo)
		matWorld = m_matLeoLocal * matTargetWorld;
	if (m_eCurType == PLAYER_TYPE::PLAYER_Michelangelo)
		matWorld = m_matMikeLocal * matTargetWorld;
	if (m_eCurType == PLAYER_TYPE::PLAYER_Raphael)
		matWorld = m_matRaphLocal * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->Set_Infomation(CTransform::INFO_SCALE, &(_vec3(1.2f, 1.2f, 1.2f)*m_fCurSize));

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix	matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 10);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);
		
	return NOERROR;
}

CPlayerChangeUI * CPlayerChangeUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayerChangeUI *	pInstance = new CPlayerChangeUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CPlayerChangeUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CPlayerChangeUI::Free(void)
{
	Safe_Release(m_pObserver);
	Safe_Release(m_pTextureCom);	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pUITransformCom);

	return CUI::Free();
}
