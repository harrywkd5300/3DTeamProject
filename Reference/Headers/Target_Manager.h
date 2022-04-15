#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CTarget;
class DLL_EXPORT CTarget_Manager : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	explicit CTarget_Manager(void);
	virtual ~CTarget_Manager(void) = default;
public:
#ifdef ADD_GETTER_TARGETANDSHADER
	const _tchar* Get_TargetName( const _uint& _iIndex );
	_uint Get_TargetCount( void );
#endif
	HRESULT Get_SurfaceLevel( const _tchar* pTargetName, const _uint& iLevel, LPDIRECT3DSURFACE9* pOut );
	LPDIRECT3DTEXTURE9 Get_Texture( const _tchar* pTargetName );
public:
	HRESULT Add_Target(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTargetTag, const _uint& iTargetCX, const _uint& iTargetCY, D3DFORMAT eFormat, D3DXCOLOR Color);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const _tchar* pTargeTag, const char* pConstantName);
public:
	HRESULT Begin_MRT(const _tchar* pMRTTag);
	HRESULT End_MRT(const _tchar* pMRTTag);
	HRESULT Begin_Target(const _tchar* pTargetTag, _uint iIndex = 0);
	HRESULT End_Target(const _tchar* pTargetTag, _uint iIndex = 0);
#ifdef KMH_MODIFY // By.KMH
	HRESULT Clear_MRT( const _tchar* pMRTTag );
	HRESULT Clear_Target( const _tchar* pTargetTag );
	void Save_Target( const _tchar* pTargetTag );
#endif

	HRESULT Ready_DebugBuffer(const _tchar* pTargetTag, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
	void Render_DebugBuffer(const _tchar* pMRTTag);
	void Render_DebugTarget(const _tchar* pTargetTag);
	void Render_DebugAllTarget( void );

private:
	map<const _tchar*, CTarget*>				m_mapTargets;
	typedef map<const _tchar*, CTarget*>		MAPTARGETS;
private:
	map<const _tchar*, list<CTarget*>>			m_mapMRT;
	typedef map<const _tchar*, list<CTarget*>>	MAPMRT;
private:
	CTarget* Find_Target(const _tchar* pTargetTag);
	list<CTarget*>* Find_MRT(const _tchar* pMRTTag);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
public:
	virtual _ulong Free(void);	
};

END