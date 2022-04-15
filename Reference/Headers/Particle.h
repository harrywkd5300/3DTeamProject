#pragma once

#include "Component.h"

BEGIN(Engine)

class DLL_EXPORT CParticle : public CComponent
{
private:
	explicit CParticle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CParticle(const CParticle& rhs);
	virtual ~CParticle(void) = default;

private:
	PTC_INFO		m_tInfo;

	_vec3			m_vOrigin = _vec3(0.f, 0.f, 0.f); // 파티클 원천
	PTC_BOX			m_tBox = PTC_BOX();

	_float			m_fRatePerSec = 0.f;
	_float			m_fSize = 0.f;

	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;


private:
	HRESULT		Ready_Particle(PTC_INFO tPtcInfo);

public:
	static CParticle*	Create(LPDIRECT3DDEVICE9 pGraphicDev, PTC_INFO tPtcInfo);
	virtual CComponent* Clone(void);
	virtual _ulong		Free(void);
};

END