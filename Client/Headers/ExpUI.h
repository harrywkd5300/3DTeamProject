#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;
class CMyFont;

class CExpUI : public CUI
{
private:
	explicit CExpUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CExpUI() = default;


public:
	HRESULT Ready_Observer(void);
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

	HRESULT SetUp_ExpGlowConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_ExpBloodConstantTable(LPD3DXEFFECT pEffect);

private:
	void MoveTo_UI(const _float& fTimeDelta);

private:
	_float						m_fAlpha = 1.f;
	_float						m_fGlowSizeX = 1.f;
	_float						m_fGlowSizeY = 1.f;
	_float						m_fGlowX = 1.f;
	_float						m_fGlowY = 1.f;

	_float						m_fBloodSizeX = 1.f;
	_float						m_fBloodSizeY = 1.f;
	_float						m_fBloodX = 1.f;
	_float						m_fBloodY = 1.f;

	_float						m_fMaxX = 1.f;
	_float						m_fMinX = 1.f;

	
	_bool						m_IsRender = false;
	_float						m_fRenderTime = 3.f;
private:
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	CUIObserver*				m_pObserver = nullptr;
	CMyFont*					m_pFont = nullptr;

public:
	static CExpUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free(void) final;

};

