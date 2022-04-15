#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN( Engine )

class CFrustum;
class CCamera_Manager;
class DLL_EXPORT CLight : public CBase
{
private:
	explicit CLight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLight(void);
public:
	HRESULT Ready_Light( const D3DLIGHT9* pLightInfo, const _vec3& _vLightPower );
	void Render_Light(LPD3DXEFFECT pEffect);
#ifdef LIGHT_DEBUGING
	void Render_DebugLight( void );
	_bool CheckPicking( D3DLIGHT9* _pOut, const _vec3& _vRayPos, const _vec3& _vRayDir );
	void Set_IsDrawDetail( const _bool& _bisDetailDraw ) { m_bIsDetailDraw = _bisDetailDraw; }
	_bool ActOcculsionTest( Engine::CFrustum* _pFrustum, const _matrix& _matViewProj );
#endif
#ifdef KMH_MODIFY
private:
	Engine::CCamera_Manager*	m_pCam_Manager = nullptr;
#endif
#ifdef LIGHT_DEBUGING
private:
	LPDIRECT3DVERTEXBUFFER9		m_pVB_Box = nullptr;
	LPDIRECT3DINDEXBUFFER9		m_pIB_Box = nullptr;
	LPD3DXMESH					m_pSphere = nullptr;
	LPD3DXMESH					m_pSphere_Range = nullptr;
	LPD3DXLINE					m_pLine = nullptr;
	_matrix						m_matWorld;
	_matrix						m_matTrans;
	_matrix						m_matInvWorld;
	_vec3						m_vDir[8];
	_bool						m_bIsDetailDraw = false;
#endif
private:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
	D3DLIGHT9					m_LightInfo;
private:
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;
	_matrix						m_matView, m_matOrthoProj;
private:
	_vec4						m_vLightPower = _vec4( 1.f, 1.f, 1.f, 1.f );
private:
	HRESULT Ready_Matrix( const _float& _fFovY, const _float& _fAspect, const _float& _fNear, const _float& _fFar );
public:
	static CLight* Create(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo, const _vec3& _vLightPower);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

END