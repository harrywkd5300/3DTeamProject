#pragma once

#include "MyEffect.h"

BEGIN(Engine)

class CTransform;
class CTexture;
class CRect_Texture;
class CCamera_Manager;
class CMyFX;
class DLL_EXPORT CMyEffect_Buff final : public CMyEffect
{
private:
	explicit  CMyEffect_Buff(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit  CMyEffect_Buff(const CMyEffect_Buff& rhs);
	virtual ~CMyEffect_Buff(void) = default;

// ------------- 맵툴용 ---------------//
public: // Getter
	_uint	Get_TexCount(void) { return m_iTexCnt; }

public:
	void	Set_TexComponent(CTexture* pTexture, _int& iTexIndex);
	void	Reset_Texture(void);
// -------------------------------------//

public: // 맵툴, 클라 공용
	virtual void	Reset_Info(void) final;
	virtual void	Set_ParentMatrix(const D3DXMATRIX* pMatParent) final;
	virtual void	Render_Effect(CMyFX* pFX);

public:
	virtual HRESULT	Ready_GameObject(const EFFECT_INFO& tInfo);
	virtual _int	Update_GameObject(const _float& fTimeDelta);

private:
	CTexture*			m_pTextureArr[5];
	CRect_Texture*		m_pBufferCom = nullptr;

	CCamera_Manager*	m_pCam_Manager = nullptr;

private:
	const _vec4		m_vUseColor[6] = {
		_vec4(0.f, 0.f, 0.f, 0.f),	// None
		_vec4(1.f, 1.f, 1.f, 1.f),		// Base
		_vec4(1.f, 0.f, 0.f, 0.f),	// R
		_vec4(0.f, 1.f, 0.f, 0.f),	// G
		_vec4(0.f, 0.f, 1.f, 0.f),	// B
		_vec4(0.f, 0.f, 0.f, 1.f),	// Alpha
	};

	_vec4		m_vDefault[5] = {
		_vec4(0.f, 0.f, 0.f, 0.f),
		_vec4(0.f, 0.f, 0.f, 0.f),
		_vec4(0.f, 0.f, 0.f, 0.f),
		_vec4(0.f, 0.f, 0.f, 0.f),
		_vec4(0.f, 0.f, 0.f, 0.f)
	};

private:
	_float		m_fTimeDelta = 0.f;
	_uint		m_iTexCnt = 0;

	char		m_szSampler[5][126];

private:
	PTC_INFO	m_tPtcInfo;

	// 리스트가 MyEffect에 없는 이유 -> CMyEffect_Particle 때문!
private:
	//list<CMyFX*>	m_listBuffer;
	//typedef list<CMyFX*> LISTBUFFER;
	//list<CMyFX*>	m_listRecycles;
	//typedef list<CMyFX*> LISTRECYCLES;

	JWVECTOR<CMyFX*> m_listBuffer;
	JWVECTOR<CMyFX*> m_listRecycles;

private:
	HRESULT			Ready_Component(void);
	virtual HRESULT	Init_System(void) final;
	virtual void	Add_Particle(void) final;
	void			Set_Attribute(CMyFX* pAtt);
	virtual void	SetUp_OnConstantTable(LPD3DXEFFECT pEffect, CMyFX* pFX);

public:
	static CMyEffect_Buff* Create(LPDIRECT3DDEVICE9 pGraphicDev, const EFFECT_INFO& tInfo);

public:
	virtual CMyEffect* Clone(void) final;
#ifndef _FINAL
	int iStartCount = 0;
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) final;
};

END

