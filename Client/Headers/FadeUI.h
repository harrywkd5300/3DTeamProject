#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;

class CFadeUI : public CUI
{
public:
	enum FADETYPE {FADE_IN,FADE_OUT, FADE_NONE};

private:
	explicit CFadeUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFadeUI() = default;
	
public:
	HRESULT Ready_Observer(void);
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

	HRESULT SetUp_ComboConstantTable(LPD3DXEFFECT pEffect);

private:
	void MoveTo_UI(const _float& fTimeDelta);

private:
	_float						m_fAlpha = 0.f;
	
	_bool						m_IsRender = false;
	_float						m_fRenderTime = 3.f;
	
	FADETYPE					m_eFadeType = FADE_NONE;
private:
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	CUIObserver*				m_pObserver = nullptr;


public:
	static CFadeUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free(void) final;

};

