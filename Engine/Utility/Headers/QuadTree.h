#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Frustum.h"

BEGIN(Engine)

class CQuadTree : public CBase
{
	enum CHILD {CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END};
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END};
	enum CORNER {CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END};
private:
	explicit CQuadTree(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CQuadTree() = default;
public:
	HRESULT Ready_QuadTree(const _vec3* pVertexPos, const _ulong& dwCntX, const _ulong& dwCntZ);
	void Culling(CFrustum* pFrustum, INDEX32* pIndices, _uint* pTriCnt, _vec3* pVertexPos, _matrix matWorld);
private:
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
	CQuadTree*				m_pChild[CHILD_END] = {nullptr};
	CQuadTree*				m_pNeighbor[NEIGHBOR_END] = {nullptr};
	_ulong					m_dwCorner[CORNER_END] = {0};
	_ulong					m_dwCenter = 0;
	_float					m_fRadius = 0.f;
private:
	void Make_QuadTree_Child(const _vec3* pVertexPos);
	void Make_Neighbor(void);
	HRESULT SetUp_CornerIndex(const _ulong& dwLT, const _ulong& dwRT, const _ulong& dwRB, const _ulong& dwLB);
	tuple<_ulong, _ulong, _ulong, _ulong> Compute_ChildIndex_Corner(CHILD eType);
	_bool isRender_LevelofDetail(const _vec3* pVertexPos);
	
public:
	static CQuadTree* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pVertexPos, const _ulong& dwCntX, const _ulong& dwCntZ);
	static CQuadTree* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwLT, const _ulong& dwRT, const _ulong& dwRB, const _ulong& dwLB);

public:
#ifndef _FINAL
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void);
};

END