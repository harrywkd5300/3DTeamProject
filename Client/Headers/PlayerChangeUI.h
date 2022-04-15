#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;

class CPlayerChangeUI final : public CUI
{
private:
	explicit CPlayerChangeUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayerChangeUI() = default;

public:
	HRESULT Ready_Observer(void);
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
	
private:
	void Set_LocalMatrix(_matrix* pMatWorld, _vec3* vSize, _vec3* vPos);
	void Set_SelectInfo(PLAYER_TYPE eType);
private:
	void MoveTo_UI(const _float& fTimeDelta);
	void SelectMotion(const _float& fTimeDelta);

private:
	HRESULT SetUp_DirectionConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_DonConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_LeoConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_MikeConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_RaphConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_SelectConstantTable(LPD3DXEFFECT pEffect);

private:
	_float						m_fRenderTime = 3.f;
	_bool						m_IsRender = false;

	_bool						m_IsSelect = false;
	_float						m_fSelectTime = 0.1f;
	_float						m_fMaxSelectTime = 0.1f;

	PLAYER_TYPE					m_eCurType;
	PLAYER_TYPE					m_ePreType;


private:
	_float						m_fAlpha = 1.f;
	_float						m_fX = 1.f;
	_float						m_fY = 1.f;
	_float						m_fSizeX = 1.f;
	_float						m_fSizeY = 1.f;

	_float						m_fMaxLookSize = 12.f;    
	_float						m_fLookSize = 11.f;
	_float						m_fMinLookSize = 11.f;

	_float						m_fMaxRightSize = 8.5f;
	_float						m_fRightSize = 8.f;
	_float						m_fMinRightSize = 8.f;

	_float						m_fMaxUpSize = -3.f;
	_float						m_fUpSize = -3.5f;
	_float						m_fMinUpSize = -3.5f;
	
	_float						m_fMaxSize = 1.f;
	_float						m_fMinSize = 0.f;
	_float						m_fCurSize = 0.f;
	
	_vec3						m_vDirSize;
	_vec3						m_vDonSize;
	_vec3						m_vLeoSize;
	_vec3						m_vMikeSize;
	_vec3						m_vRaphSize;
		
	_vec3						m_vDirPos;
	_vec3						m_vDonPos;
	_vec3						m_vLeoPos;
	_vec3						m_vMikePos;
	_vec3						m_vRaphPos;

	_matrix						m_matDonLocal;
	_matrix						m_matLeoLocal;
	_matrix						m_matMikeLocal;
	_matrix						m_matRaphLocal;

private:
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	CUIObserver*				m_pObserver = nullptr;

	Engine::CTransform*			m_pUITransformCom = nullptr;

public:
	static CPlayerChangeUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual _ulong Free(void) final;

};

