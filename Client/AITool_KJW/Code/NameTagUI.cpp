#include "stdafx.h"
#include "..\Headers\NameTagUI.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"
#include "Object_Manager.h"
#include "Player.h"

CNameTagUI::CNameTagUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{

}

HRESULT CNameTagUI::Ready_Observer(void)
{
	m_pObserver = CUIObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CNameTagUI::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;


	pComponent = m_pArrowTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_ArrowTrans", pComponent));
	m_pArrowTransformCom->AddRef();
	

	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_NameTag");
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

HRESULT CNameTagUI::Ready_GameObject(PLAYER_TYPE eType)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
		return E_FAIL;

	m_ePlayerType = eType;

	m_vNamePos = { 0,0 };
	m_vNameSize = { 203 / 3.f,127 / 3.f };
	m_vArrowPos = { 0, 0 };
	m_vArrowSize = { 92.f/ 3.f, 85.f/ 3.f };


	return NOERROR;
}

_int CNameTagUI::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);

	MoveTo_UI(fTimeDelta);

	Set_WorldToView();

	if (*m_pObserver->Get_Current_Type() == m_ePlayerType 
		|| *m_pObserver->Get_PlayerIsDead(m_ePlayerType) == true)
		m_IsRender = false;
	else
		m_IsRender = true;

	if (nullptr != m_pRendererCom && m_IsRender == true)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this);

	return 0;
}

void CNameTagUI::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_NameConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_ArrowConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	pEffect->End();
}

HRESULT CNameTagUI::SetUp_NameConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_vNameSize.x;
	matView._22 = m_vNameSize.y;
	matView._33 = 1.f;
	matView._41 = m_vNamePos.x - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_vNamePos.y + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", m_ePlayerType);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CNameTagUI::SetUp_ArrowConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pArrowTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_vArrowSize.x;
	matView._22 = m_vArrowSize.y;
	matView._33 = 1.f;
	matView._41 = m_vArrowPos.x - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_vArrowPos.y + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	pEffect->SetFloat("g_fUAlpha", 1.f);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 4+m_ePlayerType);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}



void CNameTagUI::Set_WorldToView()
{	
	_matrix matPlayerWorld = *m_pObserver->Get_Player_WorldMatrix(m_ePlayerType);

	_vec3 vCharPos = *(_vec3*)&matPlayerWorld.m[3];
	_vec4 vOriginPos = _vec4(vCharPos, 1.f);

	_vec3 vUp = *(_vec3*)&(matPlayerWorld.m[1]);
	
	D3DXVec3Normalize(&vUp, &vUp);

	//_vec3 vEye = CCamera_Manager::GetInstance()->Get_Eye();
		
	vCharPos += vUp*2.f;

	_vec4 vPos = _vec4(vCharPos,1.f);
	
	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	
	D3DXVec4Transform(&vPos, &vPos, &matView);
	D3DXVec4Transform(&vPos, &vPos, &matProj);
	D3DXVec4Transform(&vOriginPos, &vOriginPos, &matView);
	D3DXVec4Transform(&vOriginPos, &vOriginPos, &matProj);
	
	vPos.x /= vPos.w;
	vPos.y /= vPos.w;
	vPos.z /= vPos.w;

	vOriginPos.x /= vOriginPos.w;
	vOriginPos.y /= vOriginPos.w;
	vOriginPos.z /= vOriginPos.w;
		
	_vec2 v2OriginPos;

	v2OriginPos.x = BACKSIZEX * 0.5f * vOriginPos.x + BACKSIZEX * 0.5f;
	v2OriginPos.y = BACKSIZEY * 0.5f * -vOriginPos.y + BACKSIZEY * 0.5f;

	m_vArrowPos.x = BACKSIZEX * 0.5f * vPos.x + BACKSIZEX * 0.5f;
	m_vArrowPos.y = BACKSIZEY * 0.5f * -vPos.y + BACKSIZEY * 0.5f;	
	

	if (vPos.z > 1.f)
	{
		m_vArrowPos.y += BACKSIZEY;
		v2OriginPos.y += BACKSIZEY;
	}
	else if (vPos.z < -1.f)
	{
		m_vArrowPos.y -= BACKSIZEY;
		v2OriginPos.y -= BACKSIZEY;
	}

	if (m_vArrowPos.x < BACKSIZEX / 6)
		m_vArrowPos.x = BACKSIZEX / 6;
	else if (m_vArrowPos.x > BACKSIZEX - BACKSIZEX / 6)
		m_vArrowPos.x = BACKSIZEX - BACKSIZEX / 6;

	if (m_vArrowPos.y < BACKSIZEY / 6)
		m_vArrowPos.y = BACKSIZEY / 6;
	else if (m_vArrowPos.y > BACKSIZEY - BACKSIZEY / 6)
		m_vArrowPos.y = BACKSIZEY - BACKSIZEY / 6;
	
	_vec2 vDir = (v2OriginPos - m_vArrowPos);
		
	D3DXVec2Normalize(&vDir, &vDir);	

	_vec2 vLook = { 0.f,-1.f};

	D3DXVec2Normalize(&vLook, &vLook);

	_float fAngle = D3DXVec2Dot(&vDir, &vLook);

	_float fRadion = (_float)acos(fAngle);

	if (vDir.x > 0)
		fRadion *= -1;

	m_pArrowTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToDegree(fRadion)));

	m_vNamePos = m_vArrowPos + vDir * -m_vNameSize.x;
}

void CNameTagUI::MoveTo_UI(const _float & fTimeDelta)
{
	
}
CNameTagUI * CNameTagUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, PLAYER_TYPE eType)
{
	CNameTagUI *	pInstance = new CNameTagUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(eType)))
	{
		MSG_BOX("CNameTagUI Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CNameTagUI::Free(void)
{
	Safe_Release(m_pArrowTransformCom);
	Safe_Release(m_pObserver);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	return CUI::Free();
}

