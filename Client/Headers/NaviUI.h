#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;
class CMyFont;

class CNaviUI : public CUI
{
private:
	explicit CNaviUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CNaviUI() = default;


public:
	HRESULT Ready_Observer(void);
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

	HRESULT SetUp_IconConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_ArrowConstantTable(LPD3DXEFFECT pEffect);


public:
	void Add_NaviPoint(_vec3 vPos) { m_vecNaviPoint.push_back(vPos); }
	void Set_NaviContainerSize(_uint iSize) {
		m_vecNaviPoint.reserve(iSize),
		m_iNaviPointListSize = iSize; }
private:
	void Set_ArrowPosition();
	void MoveTo_UI(const _float& fTimeDelta);
	
private:	
	_float						m_fIdleTime = 0.f;

private:
	_bool						m_IsRender = false;
	_float						m_fAlpha = 1.f;

	_float						m_fIconSizeX = 0.f;
	_float						m_fIconSizeY = 0.f;

	_float						m_fIconMaxSizeX = 0.f;
	_float						m_fIconMaxSizeY = 0.f;
	_float						m_fIconMinSizeX = 0.f;
	_float						m_fIconMinSizeY = 0.f;
	
	_float						m_fIconX = 0.f;
	_float						m_fIconY = 0.f;

	_bool						m_IsIconChange = true;


private:
	_float						m_fDirMax = 1.5f;
	_float						m_fDirLength = 1.f;
	_float						m_fDirMin = 1.f;


	_bool						m_IsArrowChange = true;
private:

	_float						m_fVTimeDelta = 0.f;
	_float						m_fArrowAlpha = 1.f;


private:
	vector<_vec3>				m_vecNaviPoint;
	_uint						m_iNaviPointListSize = 0;
	_int						m_iNaviIndex = 0;
private:
	Engine::CTransform*			m_pArrowTransformCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;	
	CUIObserver*				m_pObserver = nullptr;

public:
	static CNaviUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free(void) final;

};