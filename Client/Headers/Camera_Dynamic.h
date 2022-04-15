#pragma once

#include "Defines.h"
#include "Camera.h"

class CCamera_Dynamic final : public Engine::CCamera
{
private:
	explicit CCamera_Dynamic(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera_Dynamic(void) = default; 
public:
	virtual HRESULT Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
		, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar);
	virtual _int Update_GameObject(const _float& fTimeDelta);
private:
	void Check_InputState(const _float& fTimeDelta);
	void Fix_MousePointer(const _uint& iX, const _uint& iY);
private:
	_float			m_fCamSpeed = 10.f;
public:
	static CCamera_Dynamic* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
		, const _float& fFovY = D3DXToRadian(60.0f), const _float& fAspect = _float(BACKSIZEX) / _float(BACKSIZEY), const _float& fNear = 0.2f, const _float& fFar = 1000.f);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void);

};

