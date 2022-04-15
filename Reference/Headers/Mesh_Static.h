#pragma once

#include "Mesh.h"

BEGIN(Engine)

class DLL_EXPORT CMesh_Static final : public CMesh
{
private:
	explicit CMesh_Static(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMesh_Static(const CMesh_Static& rhs);
	virtual ~CMesh_Static() = default;
public:
	const _vec3* Get_Min() const {
		return &m_vMin;}
	const _vec3* Get_Max() const {
		return &m_vMax;}
public:
	virtual HRESULT Ready_Mesh(const _tchar* pFilePath, const _tchar* pFileName);
	virtual void Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_DiffuseTex_Name, const char* pConstant_NormalTex_Name, const char* pConstant_SpecularTex_Name);
	virtual void Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_DiffuseTex_Name, const char* pConstant_NormalTex_Name, const char* pConstant_SpecularTex_Name, _bool isAlpha);
	virtual void Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_DiffuseTex_Name);
	virtual void Render_Mesh(void);

	
private:
	LPD3DXBUFFER					m_pAdjancey = nullptr;
	LPD3DXBUFFER					m_pSubsets = nullptr;
	_ulong							m_dwNumSubsets = 0;
	LPD3DXMESH						m_pMesh = nullptr;
	D3DXVECTOR3						m_vMin, m_vMax;
	D3DXMATERIAL_DERIVED*			m_pSubsetInfo = nullptr;	
	TEXTUREDATA*					m_ppTextures = nullptr;
private:

public:
	LPD3DXMESH*	Get_Mesh() { return &m_pMesh; }


public:
	virtual CComponent* Clone(void);
	static CMesh_Static* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

END