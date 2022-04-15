#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class CFrustum;
class CQuadTree;
class DLL_EXPORT CTerrain_Buffer final : public CVIBuffer
{
private:
	explicit CTerrain_Buffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrain_Buffer(const CTerrain_Buffer& rhs);
	virtual ~CTerrain_Buffer() = default;


public:
	//HRESULT Ready_Buffer(const _tchar* pHeightBmp, const _uint & iVtxCntX, const _uint & iVtxCntZ, const _float & fInterval);
	HRESULT Ready_Buffer(const _tchar* pHeightBmp, const _float & fInterval);
	void Render_Buffer(void);
	int Get_Width() { return m_iMapWidth; }
	int Get_Height() { return m_iMapHeight; }


private:
	void CalcTangentVector(VTXTEXTBN& pVertex1, VTXTEXTBN& pVertex2, VTXTEXTBN& pVertex3, _vec3 vNormal);

private:
	//CQuadTree*				m_pQuadTree = nullptr;
	//INDEX16*					m_pIndices = nullptr;
	int m_iMapWidth = 0;
	int m_iMapHeight = 0;

public:
	virtual CComponent* Clone(void);
	static CTerrain_Buffer* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pHeightMap, const _float& fInterval = 1.0f);
	//static CTerrain_Buffer* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pHeightMap, const _uint& iVtxCntX, const _uint& iVtxCntZ, const _float& fInterval = 1.0f);
public:
#ifndef _FINAL
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void);

};

END