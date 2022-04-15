#include "stdafx.h"
#include "..\Headers\UI_Stats.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"

#include "Object_Manager.h"
#include "Input_Device.h"

#include "MyFont.h"


CUI_Stats::CUI_Stats(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)	
{
#ifndef _FINAL
	Tag(L"CUI_Stats", std::bind(&CUI_Stats::Debug_Render, this));
#endif
}

HRESULT CUI_Stats::Ready_Observer(void)
{
	m_pObserver = CUIObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CUI_Stats::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;
	
	// For.Transform
	pComponent = m_pUITransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_UITransform", pComponent));
	m_pUITransformCom->AddRef();
	
	// For.Texture
	pComponent = m_pSlotTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Slot");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_SlotTexture", pComponent));
	m_pSlotTextureCom->AddRef();


	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Stats");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	pComponent = m_pHpTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Hp");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_TextureHp", pComponent));
	m_pHpTextureCom->AddRef();

	pComponent = m_pEpTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Ep");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_TextureEp", pComponent));
	m_pEpTextureCom->AddRef();

	pComponent = m_pMaskTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_StatsMask");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_TextureMask", pComponent));
	m_pMaskTextureCom->AddRef();
	
	pComponent = m_pAbilityTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Ability");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_TextureAbility", pComponent));
	m_pAbilityTextureCom->AddRef();
	
	pComponent = m_pItemTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_Item");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"CTexture_Item", pComponent));
	m_pItemTextureCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CShader_UI");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();
	
	m_pFont = CMyFont::Create(m_pGraphicDev, L"CTexture_MarioFont", 17.f, 6.f);
	m_pFont->Set_FontSetting(64.f / 2.f, 79.f / 2.f, 0.f, _vec4(1.f, 1.f, 1.f, 1.f));
	return NOERROR;
}

HRESULT CUI_Stats::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;	

	if (FAILED(Ready_Observer()))
		return E_FAIL;
	
	//m_vHUDSize = _vec3(0.6f, 0.3f, 0.f); // 기준 사이즈
	//m_vHpSize = _vec3(0.38f, 0.025f, 0.f);
	//m_vCpSize = _vec3(0.125f, 0.008f, 0.f);
	//m_vEpSSize = _vec3(0.31f, 0.02f, 0.f);
	//m_vEpMSize = _vec3(0.31f, 0.02f, 0.f);
	//m_vEpFSize = _vec3(0.31f, 0.07f, 0.f);
	//m_vEpHUDSize = _vec3(0.325f, 0.06f, 0.f);

	m_vHUDSize = _vec3(0.6f, 0.3f, 0.3f); // 기준 사이즈
	m_vAbilitySize = _vec3(0.08f, 0.08f, 0.06f);
	m_vItemSize = _vec3(0.065f, 0.065f, 0.06f);
	m_vItemFontSize = _vec3(0.110f, 0.064f, 0.06f);
	m_vHpSize = _vec3(0.38f, 0.025f, 0.06f);
	m_vCpSize = _vec3(0.125f, 0.008f, 0.02f);
	m_vEpSSize = _vec3(0.31f, 0.02f, 0.05f);
	m_vEpMSize = _vec3(0.31f, 0.02f, 0.05f);
	m_vEpFSize = _vec3(0.31f, 0.07f, 0.05f);
	m_vEpHUDSize = _vec3(0.325f, 0.06f, 0.06f);

	m_vCountFontSize = _vec3(0.05f, 0.055f, 0.06f);
	//m_vHUDSize = _vec3(0.8f, 0.5f, 0.f); // 기준 사이즈
	//m_vHpSize = _vec3(0.28f, 0.225f, 0.f);
	//m_vCpSize = _vec3(0.125f, 0.008f, 0.f);
	//m_vEpSSize = _vec3(0.31f, 0.02f, 0.f);
	//m_vEpMSize = _vec3(0.31f, 0.02f, 0.f);
	//m_vEpFSize = _vec3(0.31f, 0.07f, 0.f);
	//m_vEpHUDSize = _vec3(0.325f, 0.06f, 0.f);
		
	m_vHUDPos = _vec3(0.f, 0.f, 0.f); // 기준 포스

	m_vAbilityPos = _vec3(-0.17f, -0.05f, 0.f); // 기준 포스;
	Set_LocalMatrix(&m_matLocalAbility, &m_vAbilitySize, &m_vAbilityPos);

	m_vItemPos = _vec3(-0.07f, -0.118f, 0.f); // 기준 포스;
	Set_LocalMatrix(&m_matLocalItem, &m_vItemSize, &m_vItemPos);

	m_vItemFontPos = _vec3(-0.154f, -0.118f, 0.f); // 기준 포스;
	Set_LocalMatrix(&m_matLocalItemFont, &m_vItemFontSize, &m_vItemFontPos);

	m_vItemPos2 = _vec3(0.106f, -0.118f, 0.f);
	Set_LocalMatrix(&m_matLocalItem2, &m_vItemSize, &m_vItemPos2);

	m_vItemFontPos2 = _vec3(0.186f, -0.118f, 0.f); // 기준 포스;
	Set_LocalMatrix(&m_matLocalItemFont2, &m_vItemFontSize, &m_vItemFontPos2);

	m_vHpPos  = _vec3(0.087f, 0.075f, 0.f); // 기준 포스;
	Set_LocalMatrix(&m_matLocalHp,&m_vHpSize,&m_vHpPos);

	m_vCpPos  = _vec3(-0.052f, 0.055f, 0.f);
	Set_LocalMatrix(&m_matLocalCp, &m_vCpSize, &m_vCpPos);

	m_vCp1Pos = _vec3(0.063f, 0.055f, 0.f);
	Set_LocalMatrix(&m_matLocalCp1, &m_vCpSize, &m_vCp1Pos);

	m_vCp2Pos = _vec3(0.178f, 0.055f, 0.f);
	Set_LocalMatrix(&m_matLocalCp2, &m_vCpSize, &m_vCp2Pos);

	m_vEpSPos = _vec3(0.04f, 0.01f, 0.f); // 더해줄값
	Set_LocalMatrix(&m_matLocalEpS, &m_vEpSSize, &m_vEpSPos);

	m_vEpMPos = _vec3(0.04f, 0.01f, 0.f);
	Set_LocalMatrix(&m_matLocalEpM, &m_vEpMSize, &m_vEpMPos);

	m_vEpFPos = _vec3(0.04f, 0.017f, 0.f);
	Set_LocalMatrix(&m_matLocalEpF, &m_vEpFSize, &m_vEpFPos);

	m_vEpHUDPos = _vec3(0.04f, 0.017f, 0.f);
	Set_LocalMatrix(&m_matLocalEpHUD, &m_vEpHUDSize, &m_vEpHUDPos);

	m_vCountFontPos = _vec3(-0.17f, -0.115f, 0.f);  //-0.154f, -0.118f, 0.f
	Set_LocalMatrix(&m_matLocalCountFont, &m_vCountFontSize, &m_vCountFontPos);

	m_vCountFontPos2 = _vec3(0.21f, -0.115f, 0.f); //0.186f, -0.118f, 0.f
	Set_LocalMatrix(&m_matLocalCountFont2, &m_vCountFontSize, &m_vCountFontPos2);
		
	m_fMaxHP = 100.f;
	m_fHP = m_fMaxHP;
	
	m_fHPAcc = 1.f;

	m_fDamageHp_Acc = 1.f;
	m_fDamageHp_Cut = 1.f;

	return NOERROR;
}

_int CUI_Stats::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);

	m_fUTimeDelta += fTimeDelta*3;	
	
	CWorldState* pWorldState = m_pObserver->Get_Current_WorldState();

	if (pWorldState != nullptr)
	{
		if (pWorldState->getVariable(CH_STATE_HIT) == true ||
			pWorldState->getVariable(CH_STATE_ATTACK) == true)
		{
			m_IsRender = true;
			m_IsRenderMotion = true;
			m_fRenderTime = 5.f;
		}
	}
	MoveTo_UI(fTimeDelta);
		

	///////////
	if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_B))
	{
		m_fHP -= 10.f;
	}
	if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_N))
	{
		m_fHP += 10.f;
	}	
	//////////

	HitMotion(fTimeDelta);

	if (nullptr != m_pRendererCom && m_IsRender == true)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this, RENDER_TOP);
	
	return 0;
}

void CUI_Stats::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_HUDConstantTable(pEffect);	
		
	pEffect->Begin(nullptr,0);

	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_EP_S_ConstantTable(pEffect);

	pEffect->BeginPass(TYPE_ANIMATION);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();


	SetUp_EP_M_ConstantTable(pEffect);

	pEffect->BeginPass(TYPE_ANIMATION);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();


	SetUp_EP_F_ConstantTable(pEffect);

	pEffect->BeginPass(TYPE_ANIMATION);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();


	SetUp_DamageHPConstantTable(pEffect);

	pEffect->BeginPass(TYPE_GRADALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_HPConstantTable(pEffect);

	pEffect->BeginPass(TYPE_GRADALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();
	
	SetUp_CPConstantTable(pEffect);

	pEffect->BeginPass(TYPE_GRADALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_CP1ConstantTable(pEffect);

	pEffect->BeginPass(TYPE_GRADALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();
	
	SetUp_CP2ConstantTable(pEffect);

	pEffect->BeginPass(TYPE_GRADALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_EPHUDConstantTable(pEffect);

	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_AbilityConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();
	
	SetUp_ItemFontConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_ItemConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();
	
	SetUp_Item2FontConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();

	SetUp_Item2ConstantTable(pEffect);
	pEffect->BeginPass(TYPE_ALPHA);
	m_pBufferCom->Render_Buffer();
	pEffect->EndPass();
	
	pEffect->End();

	Render_Font();

	

}
void CUI_Stats::Set_LocalMatrix(_matrix* pMatWorld, _vec3* vSize, _vec3* vPos)
{
	m_pTransformCom->Set_Infomation(CTransform::INFO_SCALE, &m_vHUDSize);
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &m_vHUDPos);	

	_matrix matTargetInvWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	D3DXMatrixInverse(&matTargetInvWorld, nullptr, &matTargetInvWorld);

	m_pUITransformCom->Set_Infomation(CTransform::INFO_SCALE,vSize);
	m_pUITransformCom->Set_Infomation(CTransform::INFO_POSITION, vPos);

	_matrix matWorld = m_pUITransformCom->Get_WorldMatrix_FORCED();

	*pMatWorld = matWorld * matTargetInvWorld;

}
void CUI_Stats::Set_Data()
{
	m_fHPAcc = m_pObserver->Get_Current_CharInfo()->m_fHpAcc;

}
HRESULT CUI_Stats::SetUp_HUDConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();
	
	 _matrix CameraWorld = *m_pObserver->Get_Current_CameraWorldMatrix();
	
	_vec3 vRight = *(_vec3*)&CameraWorld.m[0];
	_vec3 vUp = *(_vec3*)&CameraWorld.m[1];
	_vec3 vLook = *(_vec3*)&CameraWorld.m[2];
	
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);
	
	m_vHUDPos = *m_pObserver->Get_CharPos() + vRight*1.5f + vLook*2.f + vUp * 0.7f;

	m_pTransformCom->Set_Infomation(CTransform::INFO_SCALE, &(m_vHUDSize*m_fCurSize));
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &m_vHUDPos);

	_vec3 vAngle = *m_pTransformCom->Get_Infomation(CTransform::INFO_ANGLE);

	m_pTransformCom->Set_Angle(*m_pObserver->Get_Current_CameraInvViewMatrix());

	m_pTransformCom->Rotation_X(vAngle.x, 1.f);

	m_pTransformCom->Add_Infomation(CTransform::INFO_ANGLE, &_vec3(0.f,40.f,0.f));
	
	m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);  // 셋팅된 카메라 x 현재 플레이어 카메라의 View
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
HRESULT CUI_Stats::SetUp_AbilityConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalAbility * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	PLAYER_TYPE eType = *m_pObserver->Get_Current_Type();

	m_pAbilityTextureCom->SetUp_OnShader(pEffect, "g_UITexture", eType);	

	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_ItemConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalItem * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	PLAYER_TYPE eType = *m_pObserver->Get_Current_Type();

	m_pItemTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 0);

	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_ItemFontConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalItemFont * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	PLAYER_TYPE eType = *m_pObserver->Get_Current_Type();

	m_pSlotTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 0);

	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_Item2ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalItem2 * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	PLAYER_TYPE eType = *m_pObserver->Get_Current_Type();

	m_pItemTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 3);

	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_Item2FontConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalItemFont2 * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	PLAYER_TYPE eType = *m_pObserver->Get_Current_Type();

	m_pSlotTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 1);

	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_HPConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalHp * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pHpTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture", 0);
	m_pHpTextureCom->SetUp_OnShader(pEffect, "g_GradTexture", 1);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	//pEffect->SetFloat("g_fUAlpha", 1.f);

	// m_pObserver->Get_Current_CharInfo()

	pEffect->SetFloat("g_fUAlpha", m_fHPAcc);


	PLAYER_TYPE eType = *m_pObserver->Get_Current_Type();

	if (eType == PLAYER_TYPE::PLAYER_Donatello)
		pEffect->SetVector("g_vColor", &_vec4(138.f/255.f, 43.f/255.f, 226.f/255.f,1.f));
	else if (eType == PLAYER_TYPE::PLAYER_Leonardo)
		pEffect->SetVector("g_vColor", &_vec4(16.f/255.f,72.f/255.f,1.f, 1.f));
	else if (eType == PLAYER_TYPE::PLAYER_Michelangelo)
		pEffect->SetVector("g_vColor", &_vec4(1.f, 140.f/255.f,15.f/255.f, 1.f));
	else// (iType == CPlayer::PLAYER_Raphael)
		pEffect->SetVector("g_vColor", &_vec4(1.f, 0.f, 0.f, 1.f));	

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_DamageHPConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalHp * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	
	pEffect->SetFloat("g_fAlpha", 0.6f);

	m_pHpTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture", 0);
	m_pHpTextureCom->SetUp_OnShader(pEffect, "g_GradTexture", 1);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	//pEffect->SetFloat("g_fUAlpha", 1.f);
	pEffect->SetFloat("g_fUAlpha", m_fDamageHp_Acc);


	PLAYER_TYPE eType = *m_pObserver->Get_Current_Type();

	if (eType == PLAYER_TYPE::PLAYER_Donatello)
		pEffect->SetVector("g_vColor", &_vec4(138.f / 255.f, 43.f / 255.f, 226.f / 255.f, 1.f));
	else if (eType == PLAYER_TYPE::PLAYER_Leonardo)
		pEffect->SetVector("g_vColor", &_vec4(16.f / 255.f, 72.f / 255.f, 1.f, 1.f));
	else if (eType == PLAYER_TYPE::PLAYER_Michelangelo)
		pEffect->SetVector("g_vColor", &_vec4(1.f, 140.f / 255.f, 15.f / 255.f, 1.f));
	else// (iType == CPlayer::PLAYER_Raphael)
		pEffect->SetVector("g_vColor", &_vec4(1.f, 0.f, 0.f, 1.f));

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_CPConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalCp * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pHpTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture", 0);
	m_pHpTextureCom->SetUp_OnShader(pEffect, "g_GradTexture", 1);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
		
	_float fMin = 0.f;
	_float fRatio = m_pObserver->Get_Current_CharInfo()->m_fMaxCp / 3.f;
	_float fCur = m_pObserver->Get_Current_CharInfo()->m_fCurCp - fMin;

	_float fUAlpha = 0.f;

	if (fCur >= 0)
	{
		fUAlpha = fCur / fRatio;
	}
	if (fUAlpha >= 1.f)
		fUAlpha = 1.f;

	pEffect->SetFloat("g_fUAlpha", m_pObserver->Get_Current_CharInfo()->m_fCpAcc);
	pEffect->SetVector("g_vColor", &_vec4(1.f, 1.f, 1.f, 1.f));

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_CP1ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalCp1 * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pHpTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture", 0);
	m_pHpTextureCom->SetUp_OnShader(pEffect, "g_GradTexture", 1);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
		
	_float fMin = m_pObserver->Get_Current_CharInfo()->m_fMaxCp / 3.f;
	_float fRatio = m_pObserver->Get_Current_CharInfo()->m_fMaxCp / 3.f;
	_float fCur = m_pObserver->Get_Current_CharInfo()->m_fCurCp - fMin;

	_float fUAlpha = 0.f;

	if (fCur >= 0)
	{
		fUAlpha = fCur / fRatio;
	}
	if (fUAlpha >= 1.f)
		fUAlpha = 1.f;
	

	pEffect->SetFloat("g_fUAlpha", m_pObserver->Get_Current_CharInfo()->m_fCpAcc);
	pEffect->SetVector("g_vColor", &_vec4(1.f, 1.f, 1.f, 1.f));

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_CP2ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalCp2 * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pHpTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture", 0);
	m_pHpTextureCom->SetUp_OnShader(pEffect, "g_GradTexture", 1);

	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	//pEffect->SetFloat("g_fUAlpha", 1.f);


	//cpAcc 를 전체 1로 봤을때 1/3 정도 줄도록 처리 
	//	
	_float fMin = m_pObserver->Get_Current_CharInfo()->m_fMaxCp / 3.f * 2.f;
	_float fRatio = m_pObserver->Get_Current_CharInfo()->m_fMaxCp /3.f;
	_float fCur = m_pObserver->Get_Current_CharInfo()->m_fCurCp - fMin;

	_float fUAlpha = 0.f;

	if (fCur >= 0)
	{
		fUAlpha = fCur/fRatio;
	}
	if (fUAlpha >= 1.f)
		fUAlpha = 1.f;


	//m_pObserver->Get_Current_CharInfo()->m_fCpAcc

	pEffect->SetFloat("g_fUAlpha", fUAlpha);

	pEffect->SetVector("g_vColor", &_vec4(1.f, 1.f, 1.f, 1.f));

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_EP_S_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalEpS * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	m_pEpTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 0);

	
	pEffect->SetFloat("g_fUTimeDelta", -m_fUTimeDelta*0.1f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	//pEffect->SetFloat("g_fUAlpha", m_fEP/m_fMaxEP );
	pEffect->SetFloat("g_fUAlpha", m_pObserver->Get_Current_CharInfo()->m_fEpAcc);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_EP_M_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();
	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalEpM * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	m_pEpTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 1);

	pEffect->SetFloat("g_fUTimeDelta", -m_fUTimeDelta*0.1f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);
	

	_float fUAlpha = m_pObserver->Get_Current_CharInfo()->m_fEpAcc;
	//_float fUAlpha = m_fEP / m_fMaxEP;

	if (fUAlpha > 0 && fUAlpha < 0.17f)
		fUAlpha = 0.f;
	if (fUAlpha >= 0.17 && fUAlpha < 0.34f)
		fUAlpha = 0.17f;
	if (fUAlpha >= 0.34 && fUAlpha < 0.51f)
		fUAlpha = 0.34f;
	if (fUAlpha >= 0.51f && fUAlpha < 0.66f)
		fUAlpha = 0.51f;
	if (fUAlpha >= 0.66f && fUAlpha < 0.83f)
		fUAlpha = 0.66f;
	if (fUAlpha >= 0.83f && fUAlpha < 1.f)
		fUAlpha = 0.83f;
	if (fUAlpha >= 1.f)
		fUAlpha = 1.f;

	pEffect->SetFloat("g_fUAlpha", fUAlpha);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_EP_F_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalEpF * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pEpTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 2);

	pEffect->SetFloat("g_fUTimeDelta", -m_fUTimeDelta*0.1f);	

	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	_float fUAlpha = m_pObserver->Get_Current_CharInfo()->m_fEpAcc;
	//_float fUAlpha = m_fEP / m_fMaxEP;

	if (fUAlpha > 0 && fUAlpha < 0.34f)
		fUAlpha = 0.f;
	if (fUAlpha >= 0.34f && fUAlpha < 0.66f)
		fUAlpha = 0.34f;
	if (fUAlpha >= 0.66f && fUAlpha < 1.f)
		fUAlpha = 0.66f;
	if (fUAlpha >= 1.f)
		fUAlpha = 1.f;

	pEffect->SetFloat("g_fUAlpha", fUAlpha);	

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CUI_Stats::SetUp_EPHUDConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();
	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalEpHUD * matTargetWorld;

	m_pUITransformCom->Set_WorldMatrix_ForEffect(matWorld);

	m_pUITransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	pEffect->SetFloat("g_fUTimeDelta", 0.f);
	pEffect->SetFloat("g_fVTimeDelta", 0.f);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UITexture", 1);

	pEffect->SetFloat("g_fUAlpha", 1.f);

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}
void CUI_Stats::Render_Font()
{
	_matrix matTargetWorld = m_pTransformCom->Get_WorldMatrix_FORCED();

	_matrix matWorld;

	matWorld = m_matLocalCountFont * matTargetWorld;

	m_pFont->Draw3DFont_NotDepth(&matWorld, "%d", 3);

	matWorld = m_matLocalCountFont2 * matTargetWorld;

	m_pFont->Draw3DFont_NotDepth(&matWorld, "%d", 3);

}
void CUI_Stats::MoveTo_UI(const _float& fTimeDelta)
{
	if (m_IsRenderMotion == true)
	{
		//전체의 거리를 1이라고 봤을때 현재위치에서의 이동값 // 멀수록 빨라야하며 가까울수록 느려야함.

		//_float fCurDist = abs(1 - m_fCurSize); // 현재 거리 

		//_float fSpeed = 1 / fCurDist ;

		m_fCurSize += fTimeDelta * 20.f;

		if (m_fCurSize >= m_fMaxSize)
		{
			m_fCurSize = m_fMaxSize;
		}
		m_fRenderTime -= fTimeDelta;

	}
	else
	{
		m_fCurSize -= fTimeDelta * 20.f;
		if (m_fCurSize <= m_fMinSize)
		{
			m_fCurSize = m_fMinSize;
			m_IsRender = false; 
			m_fRenderTime = 3.f;
		}
	}


	

	if (m_fRenderTime <= 0)
	{
		m_IsRenderMotion = false;			
	}

}
void CUI_Stats::HitMotion(const _float & fTimeDelta)
{
	m_fHPAcc = m_fHP / m_fMaxHP;

	if (m_fHPAcc < m_fDamageHp_Cut)
	{
		m_fDamageTime += fTimeDelta;
		//전체의 거리를 1이라고 봤을때 현재위치에서의 이동값 // 멀수록 빨라야하며 가까울수록 느려야함.

		if (m_fDamageTime > 1.f)
		{
			_float fDistance = m_fDamageHp_Cut - m_fHPAcc; // 전체이동거리

			_float fCurDist = m_fDamageHp_Acc - m_fHPAcc; // 현재 거리 

			if (fCurDist < 0.01f)
				fCurDist = 0.01f;


			_float fSpeed = fCurDist / fDistance * 10;

			m_fDamageHp_Acc -= fTimeDelta * 0.1f * fSpeed;
		}

	}

	if (m_fDamageHp_Acc <= m_fHPAcc)
	{
		m_fDamageHp_Acc = m_fHPAcc;
		m_fDamageHp_Cut = m_fHPAcc;
		m_fDamageTime = 0.f;
	}

}
#ifndef _FINAL
void CUI_Stats::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CUI_Stats");


	CDebug::GetInstance()->Add_DebugText(m_vHUDSize,"m_vHUDSize");
	CDebug::GetInstance()->Add_DebugText(m_vHpSize,"m_vHpSize");
	CDebug::GetInstance()->Add_DebugText(m_vCpSize,"m_vCpSize");
	CDebug::GetInstance()->Add_DebugText(m_vEpSSize,"m_vEpSSize");
	CDebug::GetInstance()->Add_DebugText(m_vEpMSize,"m_vEpMSize");
	CDebug::GetInstance()->Add_DebugText(m_vEpFSize,"m_vEpFSize");
	CDebug::GetInstance()->Add_DebugText(m_vEpHUDSize,"m_vEpHUDSize");

	CDebug::GetInstance()->Add_DebugText(m_vHUDPos,"m_vHUDPos");
	CDebug::GetInstance()->Add_DebugText(m_vHpPos,"m_vHpPos");
	CDebug::GetInstance()->Add_DebugText(m_vCpPos,"m_vCpPos");
	CDebug::GetInstance()->Add_DebugText(m_vCp1Pos, "m_vCp1Pos");
	CDebug::GetInstance()->Add_DebugText(m_vCp2Pos, "m_vCp2Pos");
	CDebug::GetInstance()->Add_DebugText(m_vEpSPos,"m_vEpSPos");
	CDebug::GetInstance()->Add_DebugText(m_vEpMPos,"m_vEpMPos");
	CDebug::GetInstance()->Add_DebugText(m_vEpFPos,"m_vEpFPos");
	CDebug::GetInstance()->Add_DebugText(m_vEpHUDPos,"m_vEpHUDPos");
	
}																						
#endif																					
CUI_Stats * CUI_Stats::Create(LPDIRECT3DDEVICE9 pGraphicDev)							
{																						
	CUI_Stats *	pInstance = new CUI_Stats(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CUI_Stats Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
_ulong CUI_Stats::Free(void)
{	
	Safe_Release(m_pFont);
	Safe_Release(m_pSlotTextureCom);
	Safe_Release(m_pItemTextureCom);
	Safe_Release(m_pAbilityTextureCom);
	Safe_Release(m_pObserver);
	Safe_Release(m_pUITransformCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pHpTextureCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pEpTextureCom);
	Safe_Release(m_pShaderCom);	

	return CUI::Free();
}
