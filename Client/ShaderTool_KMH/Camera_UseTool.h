#pragma once
#include "Tool_Defines.h"
#include "Camera.h"

namespace Engine
{
	class CGameObject;
}

class CCamera_UseTool final : public Engine::CCamera
{
private:
	explicit CCamera_UseTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera_UseTool(void) = default; 
public:
	void Set_Position( const _vec3& _vPosition, const _bool& _bIsTeleport = true );
	void Set_CamRunMode( const _bool& _bIsRunMode );
	void Set_CamSpeed( const _float& _fSpeed ) { m_fCamSpeed = _fSpeed; }
	void Set_PlayerCamMode( const _bool& _bIsPlayerCamMode );
	void Set_LookTarget( Engine::CGameObject* _pTarget );
public:
	_float Get_CamSpeed( void ) { return m_fCamSpeed; }
	_bool Set_PlayerCamMode( void ) { return m_bIsPlayerCamMode; }
public:
	virtual HRESULT Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
									  , const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar);
	virtual _int Update_GameObject(const _float& fTimeDelta);
private:
	void Check_InputState(const _float& fTimeDelta);
	void Fix_MousePointer(const _uint& iX, const _uint& iY);
private:
	_float			m_fCamSpeed = 10.f;
	_float			m_fRotSpeed = 2.5f;
	_ubyte			m_iCount = 0;
	_bool			m_bIsRunMode = false;
	_vec3			m_vDestination;
	_bool			m_bGoDestination = false;
	_bool			m_bIsPlayerCamMode = false;
private:
	CGameObject*	m_pLookTarget = nullptr;
public:
	static CCamera_UseTool* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
									, const _float& fFovY = D3DXToRadian(60.0f), const _float& fAspect = _float(WINCX) / _float(WINCY), const _float& fNear = 0.2f, const _float& fFar = 1000.f);
private:
	virtual _ulong Free(void);

};

