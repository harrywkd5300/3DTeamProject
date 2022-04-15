#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;

class CScreenUI : public CUI
{

private:
	explicit CScreenUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScreenUI() = default;

public:	
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	
private:
	_float						m_fAlpha = 1.f;

	_bool						m_IsRender = false;

private:
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

public:
	static CScreenUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free(void) final;

};

