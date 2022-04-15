#pragma once

#include "UI.h"
#include "Player.h"


namespace Engine
{
	class CTexture;
}

class CUIObserver;
class CMyFont;

class CUI_Stats : public CUI
{
public:
	enum UISTATE{STATE_STABILITY, STATE_HIT};

private:
	explicit CUI_Stats(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI_Stats() = default;

public:
	HRESULT Ready_Observer(void);
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);


private:
	void Set_LocalMatrix(_matrix* pMatWorld,_vec3* vSize,_vec3* vPos);
	void Set_Data();
private:
	HRESULT SetUp_HUDConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_AbilityConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_ItemConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_ItemFontConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_Item2ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_Item2FontConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_HPConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_DamageHPConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_CPConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_CP1ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_CP2ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_EP_S_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_EP_M_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_EP_F_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_EPHUDConstantTable(LPD3DXEFFECT pEffect);

	void Render_Font();
private:
	void MoveTo_UI(const _float& fTimeDelta);
	void HitMotion(const _float& fTimeDelta);
	
private:
	_float						m_fMaxSize = 2.f;
	_float						m_fCurSize = 0.f;
	_float						m_fMinSize = 0.f;

	_float						m_fRenderTime = 5.f;
	_bool						m_IsRender = false;
	_bool						m_IsRenderMotion = false;
private: // Shader Option
	_float						m_fAlpha = 1.f;
	_float						m_fUTimeDelta = 0.f;
	_float						m_fVTimeDelta = 0.f;
	
	_vec3						m_vHUDSize;
	_vec3						m_vAbilitySize;
	_vec3						m_vItemSize;
	_vec3						m_vItemFontSize;
	_vec3						m_vHpSize;
	_vec3						m_vCpSize;
	_vec3						m_vEpSSize;
	_vec3						m_vEpMSize;
	_vec3						m_vEpFSize;
	_vec3						m_vEpHUDSize;
	_vec3						m_vCountFontSize;
	
	_vec3						m_vHUDPos;
	_vec3						m_vAbilityPos;
	_vec3						m_vItemPos;
	_vec3						m_vItemFontPos;	
	_vec3						m_vItemPos2;
	_vec3						m_vItemFontPos2;
	_vec3						m_vHpPos;
	_vec3						m_vCpPos;
	_vec3						m_vCp1Pos;
	_vec3						m_vCp2Pos;
	_vec3						m_vEpSPos;
	_vec3						m_vEpMPos;
	_vec3						m_vEpFPos;
	_vec3						m_vEpHUDPos;
	_vec3						m_vCountFontPos;
	_vec3						m_vCountFontPos2;

	_matrix						m_matLocalHUD;
	_matrix						m_matLocalItem;
	_matrix						m_matLocalItem2;
	_matrix						m_matLocalItemFont;
	_matrix						m_matLocalItemFont2;
	
	_matrix						m_matLocalAbility;
	_matrix						m_matLocalHp;
	_matrix						m_matLocalCp;
	_matrix						m_matLocalCp1;
	_matrix						m_matLocalCp2;
	_matrix						m_matLocalEpS;
	_matrix						m_matLocalEpM;
	_matrix						m_matLocalEpF;
	_matrix						m_matLocalEpHUD;
	_matrix						m_matLocalCountFont;
	_matrix						m_matLocalCountFont2;


private: // UI Info
	_float						m_fHP = 0.f; // 체력
	_float						m_fMaxHP = 0.f; // 체력
	_float						m_fCP = 0.f; // 카운터 기력
	_float						m_fMaxCP = 10.f; // 카운터 기력
	_float						m_fEP = 5.f; // 기력
	_float						m_fMaxEP = 0.f; // 기력	
	_float						m_fHPAcc = 0.f; // 임시변수


	_float						m_fConsumeCP_Cut = 0.f;
	_float						m_fConsumeCP_Acc = 0.f;

	_float						m_fDamageHp_Cut = 0.f;
	_float						m_fDamageHp_Acc = 0.f;
	_float						m_fDamageTime = 0.f;


	_float						m_fSlot1;
	_float						m_fSlot2;
	UISTATE						m_eState = STATE_STABILITY;
	_float						m_fHitTime = 3.f;
	_float						m_fAngle = 0.f;

	PLAYER_TYPE					m_ePlayerType;
private:
	_bool						m_DebugKey = false;	

private:
	Engine::CTexture*			m_pSlotTextureCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTexture*			m_pEpTextureCom = nullptr;
	Engine::CTexture*			m_pHpTextureCom = nullptr;
	Engine::CTexture*			m_pAbilityTextureCom = nullptr;
	Engine::CTexture*			m_pItemTextureCom = nullptr;
	Engine::CTexture*			m_pMaskTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;	
	Engine::CTransform*			m_pUITransformCom = nullptr;
	
	CUIObserver*				m_pObserver = nullptr;

	CMyFont*					m_pFont = nullptr;

#ifndef _FINAL
public:
	void Debug_Render();
#endif

public:
	static CUI_Stats* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual _ulong Free(void);
};




