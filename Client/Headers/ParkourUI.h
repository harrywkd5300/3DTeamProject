#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;
class CMyFont;

class CParkourUI : public CUI
{
private:
	explicit CParkourUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CParkourUI() = default;


public:
	HRESULT Ready_Observer(void);
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(_vec3 vPos, _vec3 vAngle);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

	HRESULT SetUp_IconConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_LogoConstantTable(LPD3DXEFFECT pEffect);

private:
	void Set_Direction();
	void MoveTo_UI(const _float& fTimeDelta);


private:
	_uint						m_iTenthDigit = 0;
	_uint						m_iUnitsDigit = 0;

private:
	_vec3						m_vOriginalDir;
	_float						m_fDir = 1.f;
private:
	_float						m_fAlpha = 1.f;
	_float						m_fOriginAngleY = 0.f;
	_float						m_fAngleY = 0.f; // 0부터 시작이 아닐수있음. 

	_float						m_fLogoAlpha = 1.f;

	_vec3						m_vLogoMove = { 0.3f,-0.05f,0.f };
	_vec3						m_vLogoMinMove = { -0.15f,-0.05f,0.f };
	_vec3						m_vLogoMaxMove = { 0.3f,0.05f,0.f };
		
	_float						m_fShakingTime = 0.f;

	_float						m_fFontAlpha = 1.f;

	_vec3						m_vFontMove = {0.15f,0.05f,0.f};
	_vec3						m_vFontMinMove = { 0.15f,0.05f,0.f };
	_vec3						m_vFontMaxMove = { 0.3f,0.2f,0.f };

	_bool						m_IsRender = true;
	_bool						m_IsAction = false;

private:
	_vec3						m_vFontAngle;
	_vec3						m_vFontScale;
	_vec3						m_vFontPos;

private:
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	Engine::CTransform*			m_pLogoTransformCom = nullptr;
	CUIObserver*				m_pObserver = nullptr;
	CMyFont*					m_pFont = nullptr;

public:
	static CParkourUI* Create(LPDIRECT3DDEVICE9 pGraphicDev,_vec3 vPos,_vec3 vAngle);

private:
	virtual _ulong Free(void) final;

};


