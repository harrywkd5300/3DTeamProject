#pragma once

#include "Component.h"

BEGIN(Engine)

class DLL_EXPORT CTexture : public CComponent
{
public:
	enum TEXTURETYPE {TYPE_NORMAL, TYPE_CUBE, TYPE_END};
private:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture(void) = default;

public:
	HRESULT Ready_Texture(TEXTURETYPE eType, const _tchar* pFilePath, const _uint& iTexCnt);
	HRESULT SetUp_OnGraphicDev(const _ulong& dwStage, const _uint& iCnt = 0);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName, const _uint& iCnt = 0);
private:	
	vector<LPDIRECT3DBASETEXTURE9>			m_vecTextures;
	typedef vector<LPDIRECT3DBASETEXTURE9>	VECTEXTURES;
public:
	virtual CComponent* Clone(void);
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTURETYPE eType, const _tchar* pFilePath, const _uint& iTexCnt = 1);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void) final;
};

END
