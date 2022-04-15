#pragma once

#include "Defines.h"
#include "Camera.h"


namespace Engine
{
	class CTransform;
	class CCollider;
}

class CCameraObserver;

class CCamera_CCTV : public CCamera
{
private:
	explicit CCamera_CCTV(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera_CCTV(void) = default;

public:
	_bool  Get_TargetOn() { return m_bTargetOn; }

public:
	HRESULT Set_Target(CTransform* pTargetTransform);
	void Set_Relative(_float fRelative) { m_fRelative = fRelative; }
	void Set_TargetOn(_bool bTargetOn) { m_bTargetOn = bTargetOn; }

public:	
	HRESULT Ready_Component();
	HRESULT Ready_Observer();
	virtual HRESULT Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
		, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar);
	virtual _int Update_GameObject(const _float& fTimeDelta);
private:
	_float			m_fCamSpeed = 10.f;
public:
	static CCamera_CCTV* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
		, const _float& fFovY = D3DXToRadian(60.0f), const _float& fAspect = _float(BACKSIZEX) / _float(BACKSIZEY), const _float& fNear = 0.2f, const _float& fFar = 1000.f);
private:
	virtual _ulong Free(void);

private:
	CTransform*				m_pTargetTransform = nullptr;
	CCameraObserver*		m_pObserver = nullptr;
private:
	_vec3			m_vRelative;
	_float			m_fRelative = 0.f;
	_bool			m_bTargetOn = false;
};

