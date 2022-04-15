#pragma once

#include "Resources.h"

BEGIN(Engine)

class DLL_EXPORT CMesh abstract : public CResources
{
protected:
	explicit CMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;
public:
	virtual HRESULT Ready_Mesh(const _tchar* pFilePath, const _tchar* pFileName);
protected:
	_bool isAlpha(const char* pFileName, const char* pKeyWord);
	//const _tchar* Convert_KeyWordTextureName(const char* pFileName, const char* pKeyWord);

public:
	virtual CComponent* Clone(void) = 0;
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

END