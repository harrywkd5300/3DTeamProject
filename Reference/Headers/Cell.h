#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Navigation.h"
//#include "Text_3D.h"

BEGIN(Engine)

// 네비게이션 메시를 이루는 삼각형 하나.
class CCell : public CBase
{
public:
	enum POINT {POINT_A, POINT_B, POINT_C, POINT_END};
	enum DIRECTION { DIR_AB, DIR_BC, DIR_CA, DIR_END };
	enum NEIGHBOR {NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END};
private:
	explicit CCell(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCell() = default;
public:
	const _vec3* Get_Point(POINT ePoint) const {
		return &m_vPoint[ePoint]; }

	_vec3* Get_NormalVec(DIRECTION iDir) { return &m_vNormal[iDir]; }
	CCell* Get_Neighbor(NEIGHBOR Neighbor);
public:
	HRESULT Set_Neighbor(NEIGHBOR eNeighbor, CCell* pNeighbor);

	void	Set_DirectionNormal(const _vec3* pDirAB, const _vec3* pDirBC, const _vec3* pDirCA);
	void	Set_LineColor(D3DXCOLOR dwColor) { m_pLineColor = dwColor; }
	void	Set_Index(const _uint& iIdx) { m_iIndex = iIdx; }
	void	Set_Release_Neighbor();

	void	Set_Point(POINT ePoint, const _vec3* vChangePos);
	// Option
	void	Set_Optione_AniIdx(const _uint& iIdx) { m_pOption.iAniIdx = iIdx; }
	void	Set_Optione_Move(_bool Move) { m_pOption.bMove = Move; }
	void	Set_Optione_Packour(_bool Packour) { m_pOption.bPakour = Packour; }
	void	Set_Option(NAVIOPTION pOption);

public:
	NAVIOPTION*	Get_Option() { return &m_pOption; }

private:
	_bool  Check_DistShort(const _vec3 * pSour, const _vec3 * pDest);
public:
	HRESULT Ready_Cell(const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC, const _uint& iIndex);
	HRESULT Ready_Font();
	_bool Check_Neighbor(const _vec3* pSour, const _vec3* pDest, CCell* pCell);
	CNavigation::MOVETYPE Check_Cell(const _vec3* pResultPos, _uint* pCurrentIdx);
	void Render_Cell(void);
	void Render_Text();
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
	_vec3				m_vPoint[POINT_END];
	_vec3				m_vDirection[DIR_END];
	_vec3				m_vNormal[DIR_END];	
private:
	CCell*				m_pNeightbor_Cell[NEIGHBOR_END];
	_uint				m_iIndex = 0;
	LPD3DXLINE			m_pLine = nullptr;
	D3DXCOLOR			m_pLineColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
private:
	NAVIOPTION			m_pOption;

public:
	static CCell* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC, const _uint& iIndex);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

END