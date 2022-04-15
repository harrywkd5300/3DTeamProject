#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;
class CMyFont;

class CComboUI : public CUI
{
private:
	explicit CComboUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CComboUI() = default;


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
	_float						m_fAlpha = 1.f;
	_float						m_fComboSizeX = 1.f;
	_float						m_fComboSizeY = 1.f;
	_float						m_fComboX = 1.f;
	_float						m_fComboY = 1.f;

	_bool						m_IsRender = false;
	_float						m_fRenderTime = 3.f;

	_float						m_fComboTime = 1.f;

private:
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	CMyFont*					m_pFont = nullptr;

	CUIObserver*				m_pObserver = nullptr;


public:
	static CComboUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free(void) final;

};

