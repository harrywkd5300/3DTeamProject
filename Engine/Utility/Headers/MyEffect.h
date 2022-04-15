#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CRandom;
class CShader;
class CMyFX;
class DLL_EXPORT CMyEffect abstract : public CGameObject
{
protected:
	explicit CMyEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMyEffect(const CMyEffect& rhs);
	virtual ~CMyEffect() = default;

	// Getter
public:
	const EFFECT_INFO&	Get_EffectInfo(void) { return m_tEffInfo; }

	// Setter
public:
	void			Set_EffectInfo(const EFFECT_INFO& tInfo) { m_tEffInfo = tInfo; }
	virtual void	Set_ParentMatrix(const D3DXMATRIX* pMatParent) = 0;

public:
	virtual void	Reset_Info(void) = 0; // 맵툴용

public:
	virtual HRESULT	Ready_GameObject(const EFFECT_INFO& tInfo) = 0;
	virtual _int	Update_GameObject(const _float& fTimeDelta) = 0;
	virtual void	Render_Effect(CMyFX* pFX);

protected:
	CRandom*		m_pRandomMgr = nullptr;
	CShader*		m_pShaderCom = nullptr;
	// 자기 자신을 의미하는 행렬 -> 시스템 클래스가 되면서 필요없어짐
	//CTransform*		m_pTransformCom = nullptr;

protected:
	_bool		m_bIsClone = false;

protected:
	EFFECT_INFO		m_tEffInfo;

protected:
	HRESULT			Ready_Component(void); // 하위클래스에서 반드시 호출해야함
	virtual HRESULT	Init_System(void) = 0;
	virtual void	Add_Particle(void) = 0;
	virtual void	SetUp_OnConstantTable(LPD3DXEFFECT pEffect, CMyFX* pFX);

public:
	virtual CMyEffect* Clone(void) = 0;
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) override;
};

END