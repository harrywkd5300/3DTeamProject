#pragma once

#include "Component.h"

BEGIN(Engine)

class DLL_EXPORT CMultiTexture : public CComponent
{
public:
	enum MULTITEXTURETYPE { TEX_TYPE_DIFFUSE, TEX_TYPE_NORMAL, TEX_TYPE_SPECULAR, TEX_TYPE_END};
private:
	explicit CMultiTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMultiTexture(const CMultiTexture& rhs);
	virtual ~CMultiTexture(void) = default;

public:
	HRESULT Ready_Texture(const _tchar* pFilePath, const _tchar* pFileName);
	//HRESULT SetUp_OnGraphicDev(const _ulong& dwStage, MULTITEXTURETYPE eType, const _tchar * pEffectHandle);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, MULTITEXTURETYPE eType, const char* pConstantName);
private:	
	LPDIRECT3DTEXTURE9 m_pTextureArr[TEX_TYPE_END];

public:
	virtual CComponent* Clone(void);
	static CMultiTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void) final;
};

END
