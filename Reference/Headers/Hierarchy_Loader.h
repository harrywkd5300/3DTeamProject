#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CHierarchy_Loader : public ID3DXAllocateHierarchy
{
public:
	CHierarchy_Loader(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath);
	~CHierarchy_Loader();
public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name,
		LPD3DXFRAME *ppNewFrame) ;
	STDMETHOD(CreateMeshContainer)(THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER *ppNewMeshContainer) ;
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree) ;
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree) ;
private:
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
	const _tchar*			m_pPath = nullptr;
private:
	HRESULT Allocate_Name(char** ppDestName, const char* const pName);
	_bool isAlpha(const char* pFileName, const char* pKeyWord);
	//const _tchar* Convert_KeyWordTextureName(const char* pFileName, const char* pKeyWord);
public:
	static CHierarchy_Loader* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath);
};

END