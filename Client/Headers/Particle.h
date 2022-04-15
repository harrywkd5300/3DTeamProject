#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CTransform;
	class CRenderer;
	class CShader;
	class CTexture;
	class CCamera_Manager;
}

class CParticle : public Engine::CGameObject
{
private:
	explicit CParticle(LPDIRECT3DDEVICE9 pGraphicDev);
	//explicit CParticle(const CParticle& rhs);
	virtual ~CParticle(void) = default;

public:
	virtual HRESULT		Ready_GameObject(void);
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject(void);

private:
	//Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CRandom*			m_pRandomCom = nullptr;

	Engine::CCamera_Manager*	m_pCam_Manager = nullptr;

private:
	PTC_INFO	m_tInfo;
	_ulong		m_dwTotalCnt = 0;

	_uint		m_iViewportHeight = 0;

	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;

	list<PTC_ATTRIBUTE*>		m_listParticles;
	typedef	list<PTC_ATTRIBUTE*>	LISTPARTICLES;

	// For.Debug
	_float		m_fTimeDelta = 0.f;

private:
	HRESULT		Ready_Component(void);
	HRESULT		Init_ParticleSystem(void);
	void		Add_Particle(void);
	void		Set_Attribute(PTC_ATTRIBUTE* pAtt);

private:
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
	void		Render_Buffer(void);
	_bool		Is_PointInside(const _vec3& vPos);
	void		Remove_DeadParticles(void);

public:
	//static CParticle*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const PTC_INFO& tPtcInfo, const _tchar* pFilePath);
	static CParticle*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong		Free(void);
};