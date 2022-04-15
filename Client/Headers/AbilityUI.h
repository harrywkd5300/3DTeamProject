#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;
class CMyFont;

class CAbilityUI : public CUI
{

private:
	explicit CAbilityUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAbilityUI() = default;

public:
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

private:
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_KeyConstantTable(LPD3DXEFFECT pEffect,_int iCurNum);

public:
	void	Set_ActivateOption(_bool IsActivate) { m_IsActivate = IsActivate; }

private:
	_float						m_fAlpha = 1.f;
	_bool						m_IsRender = false;

private:
	_vec2						m_vKeySize[2];
	_vec2						m_vKeyPos[2];
private:
	CMyFont*					m_pKeyFont = nullptr;
private:
	_bool						m_IsActivate = false;
	_bool						m_IsUpdate = false;

	_int						m_iSelectIndex = 0;
private:
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTexture*			m_pKeyTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

public:
	static CAbilityUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free(void) final;

};


