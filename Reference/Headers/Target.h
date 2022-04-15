#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN( Engine )

#ifdef RENDERTARGETNAME
class CDebug;
#endif
class CTarget : public CBase
{
public:
	explicit CTarget(LPDIRECT3DDEVICE9 pGrapihcDev);
	virtual ~CTarget(void) = default;
public:
	HRESULT Get_SurfaceLevel( const _uint& iLevel, LPDIRECT3DSURFACE9* pOut );
	LPDIRECT3DTEXTURE9 Get_Texture( void );
public:
	HRESULT Ready_Target(const _uint& iTargetCX, const _uint& iTargetCY, D3DFORMAT eFormat, D3DXCOLOR Color);	
	void Clear_Target(void);
	void Save_Target(const _tchar* pName);

	HRESULT Ready_DebugBuffer(const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
#ifdef RENDERTARGETNAME
	void Render_DebugBuffer(const _tchar* _pName);
#else
	void Render_DebugBuffer(void);
#endif

public:
	HRESULT SetUp_OnGraphicDev(const _uint& iIndex);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName);
	HRESULT Release_OnGraphicDev(const _uint& iIndex);
#ifdef RENDERTARGETNAME
private:
	_float				m_fPosX = 0.f, m_fPosY = 0.f;
	CDebug*				m_pDebug = nullptr;
	char				m_szName[256] = "";
#endif
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
	LPDIRECT3DTEXTURE9	m_pTargetTexture = nullptr; // 생성한 텍스쳐
	LPDIRECT3DSURFACE9	m_pRenderTarget = nullptr; // 위에서 생성한 텍스쳐로 뽑아낸 렌더타겟
	LPDIRECT3DSURFACE9	m_pOldTarget = nullptr;
private:
	D3DXCOLOR			m_ClearColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

private:
	LPDIRECT3DVERTEXBUFFER9			m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9			m_pIB = nullptr;

public:
	static CTarget* Create(LPDIRECT3DDEVICE9 pGrapihcDev, const _uint& iTargetCX, const _uint& iTargetCY, D3DFORMAT eFormat, D3DXCOLOR Color);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

END