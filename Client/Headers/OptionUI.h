#pragma once
#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;	
}

class CUIObserver;
class CMyFont;

class COptionUI : public CUI
{

private:
	explicit COptionUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~COptionUI() = default;

public:
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

public:
	void Set_ActivateOption(_bool IsActivate) { m_IsActivate = IsActivate; }
private:
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect,_int iCurNum,_int iTexture);
	HRESULT SetUp_ScrollConstantTable(LPD3DXEFFECT pEffect, _int iCurNum);
	HRESULT SetUp_KeyConstantTable(LPD3DXEFFECT pEffect, const _int& iCurNum);
private:
	void Update_OptionSet();
	void Check_InputState(const _float& fTimeDelta);
	void Check_Option(_int iSelect);
private:
	_float						m_fAlpha = 1.f;
	_bool						m_IsRender = false;
	_bool						m_IsActivate = false;
	_bool						m_IsUpdate = false;
private:
	_vec2						m_vIconSize;
	_vec2						m_vIconPos[5];
private:
	_vec2						m_vScrollSize;
	_vec2						m_vScrollPos[2];
private:
	_int						m_iSelectIndex = 0;
private:
	_int						m_TextureNumArray[5];
	_vec4						m_vColor[5];
private:
	_vec2						m_vKeySize[2];
	_vec2						m_vKeyPos[2];
private:
	_bool						m_IsSsao = false;
	_bool						m_IsShadow = false;
	_bool						m_IsMotionblur = false;
	_bool						m_IsEdgeDectecting = false;
	_bool						m_IsHDR = false;
private:
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	Engine::CTexture*			m_pKeyTextureCom = nullptr;

	CMyFont*					m_pFont = nullptr;
	CMyFont*					m_pOptionFont = nullptr;

public:
	static COptionUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free(void) final;

};


