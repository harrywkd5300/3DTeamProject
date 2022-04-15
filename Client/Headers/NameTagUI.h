#pragma once
#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;

class CNameTagUI : public CUI
{
private:
	explicit CNameTagUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CNameTagUI() = default;


public:
	HRESULT Ready_Observer(void);
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(PLAYER_TYPE eType);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);


	HRESULT SetUp_NameConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_ArrowConstantTable(LPD3DXEFFECT pEffect);

private:
	void Set_WorldToView();
	void MoveTo_UI(const _float& fTimeDelta);

private:
	_float						m_fAlpha = 1.f;

	_vec2						m_vNamePos;
	_vec2						m_vNameSize;

	_vec2						m_vArrowPos;
	_vec2						m_vArrowSize;

	_bool						m_IsRender = true;
private:
	PLAYER_TYPE					m_ePlayerType = PLAYER_TYPE::PLAYER_None;
private:
	Engine::CTransform*			m_pArrowTransformCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	CUIObserver*				m_pObserver = nullptr;

public:
	static CNameTagUI* Create(LPDIRECT3DDEVICE9 pGraphicDev , PLAYER_TYPE eType);

private:
	virtual _ulong Free(void) final;

};