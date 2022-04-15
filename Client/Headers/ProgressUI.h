#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;

class CProgressUI : public CUI
{
	enum PLAYERSTATE { IDLE, MOVE, CHANGE,REVERSECHANGE,STATE_NONE };
private:
	explicit CProgressUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CProgressUI() = default;


public:
	HRESULT Ready_Observer(void);
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

	HRESULT SetUp_CharacterConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_ProgressConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_ProgressBarConstantTable(LPD3DXEFFECT pEffect);
	
	void Set_TargetPosition(_vec3 vPos) { m_vTargetPoint = vPos; }

private:
	void Update_State();
private:

	PLAYERSTATE					m_eCurState;
	PLAYERSTATE					m_ePreState = STATE_NONE;

	PLAYER_TYPE					m_eCurType;
	//PLAYER_TYPE				m_eChangeType;
	PLAYER_TYPE					m_ePreType = PLAYER_TYPE::PLAYER_None;

	_float						m_fMaxFrame = 0.f;
	_float						m_fCurFrame = 0.f;
	_float						m_fAddFrame = 0.f;
	_float						m_fFrameSpeed = 5.f;

	_float						m_fChangeFrame = 1.f;

	_bool						m_IsRender = false;
	_float						m_fRenderTime = 3.f;

	_float						m_fAlpha = 1.f;

private:
	_float						m_fProgressSizeX = 0.f;
	_float						m_fProgressSizeY = 0.f;
	_float						m_fProgressX = 0.f;
	_float						m_fProgressY = 0.f;

	_float						m_fProgressBarSizeX = 0.f;
	_float						m_fProgressBarSizeY = 0.f;
	_float						m_fProgressBarX = 0.f;
	_float						m_fProgressBarY = 0.f;

	_float						m_fCharacterSizeX = 0.f;
	_float						m_fCharacterSizeY = 0.f;
	_float						m_fCharacterX = 0.f;
	_float						m_fCharacterY = 0.f;

	_float						m_fMinCharY = 0.f;
	_float						m_fMaxCharY = 0.f;


private:
	_vec3						m_vTargetPoint;

private:
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTexture*			m_pDonTextureCom = nullptr;
	Engine::CTexture*			m_pLeoTextureCom = nullptr;
	Engine::CTexture*			m_pMikeTextureCom = nullptr;
	Engine::CTexture*			m_pRaphTextureCom = nullptr;
	
	Engine::CShader*			m_pShaderCom = nullptr;
	CUIObserver*				m_pObserver = nullptr;



public:
	static CProgressUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free(void) final;

};

