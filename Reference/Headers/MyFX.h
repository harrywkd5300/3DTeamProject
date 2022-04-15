#pragma once

#include "Engine_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRenderer;
class CMyEffect;
class CCamera_Manager;
class DLL_EXPORT CMyFX : public CGameObject
{
private:
	explicit CMyFX(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMyFX(const CMyFX& rhs);
	virtual ~CMyFX(void) = default;

public:
	FX_ATT&	Get_FXInfo(void) { return m_tAtt; }

public:
	void	Set_Alive(_bool bIsAlive = true) { m_bIsAlive = bIsAlive; }

public:
	_int	Update_FX(const _float& fTimeDelta, EFFECT_INFO& tSystemInfo);

public:
	virtual HRESULT Ready_GameObject(CMyEffect* pSystem);
	virtual void	Render_GameObject(void);

public:
	void	Set_ParentMatrix(const D3DXMATRIX * pMatParent);

private:
	CRenderer*		m_pRendererCom = nullptr;
	CMyEffect*		m_pSystem = nullptr;

	CCamera_Manager*	m_pCam_Manager = nullptr;
	CRandom*			m_pRandom_Manager = nullptr;

private:
	FX_ATT			m_tAtt;
	_bool			m_bIsAlive = true;
	_bool			m_bIsClone = false;

	_matrix			m_matRot;

private:
	HRESULT		Ready_Component(void);

private:
	void	Set_BillBoard(const _float& fTimeData, EFFECT_INFO& tInfo);
	void	Update_Sprite(const _float& fTimeDelta);
	void	Update_Movement(const _float& fTimeDelta, EFFECT_INFO& tInfo);

private: // Ä¿ºê
	void	Curve_Scale(_float& fTimeRatio, EFFECT_INFO& tInfo);
	void	Curve_Alpha(_float& fTimeRatio, EFFECT_INFO& tInfo);
	void	Curve_Speed(_float& fTimeRatio, EFFECT_INFO& tInfo);
	void	Curve_Color(_float& fTimeRatio, EFFECT_INFO& tInfo);
	void	Curve_Bright(_float& fTimeRatio, EFFECT_INFO& tInfo);

public:
	//CMyFX*			Clone(void);
	static CMyFX*	Create(LPDIRECT3DDEVICE9 pGraphicDev, CMyEffect* pSystem);

private:
	virtual _ulong Free(void) final;
};

END

