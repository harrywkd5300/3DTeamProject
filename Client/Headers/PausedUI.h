#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;
class CMyFont;
class COptionUI;
class CAbilityUI;

class CPausedUI : public CUI
{

private:
	explicit CPausedUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPausedUI() = default;

public:
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_IconConstantTable(LPD3DXEFFECT pEffect, const _int& iCurNum);
	HRESULT SetUp_KeyConstantTable(LPD3DXEFFECT pEffect, const _int& iCurNum);

private:
	void Move_To_UI(const _float& fTimeDelta);
	void Check_InputState(const _float& fTimeDelta);
private: 
	_float						m_fAlpha = 0.7f;
	_bool						m_IsRender = false;
	_bool						m_IsMove = false;
	_bool						m_IsAction = false;
	_bool						m_IsInputCheck = false;
	_bool						m_IsSelectState = false;

	_float						m_fCumulativeTime = 0.f;
private:// Option 창
	_vec2						m_vOptionSize;
	_vec2						m_vOptionMaxSize = { (_float)BACKSIZEX ,(_float)BACKSIZEY};

private: // Option의 Menu 아이콘
	_float						m_fIconAlpha = 0.7f;
	
	_vec2						m_vIconSize = { 0.f,0.f };
	_vec2						m_vIconMaxSize = { 0.f,0.f };
	_vec2						m_vIconPos[4];
	_vec2						m_vIconMaxPos[4];

	_int						m_iSelectIndex = 0;
	
private:
	_vec2						m_vKeySize[2];
	_vec2						m_vKeyPos[2];
private:
	_vec2						m_vFontPos;
private:
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTexture*			m_pMenuTextureCom = nullptr;
	Engine::CTexture*			m_pKeyTextureCom = nullptr;

	Engine::CShader*			m_pShaderCom = nullptr;
	CMyFont*					m_pFont = nullptr;
	CMyFont*					m_pOptionFont = nullptr;

	COptionUI*					m_pOptionUI = nullptr;
	CAbilityUI*					m_pAbilityUI = nullptr;
public:
	static CPausedUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free(void) final;

};

