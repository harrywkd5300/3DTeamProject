#pragma once

#include "Component.h"

BEGIN(Engine)

class CCell;
class DLL_EXPORT CNavigation final : public CComponent
{
public:
	enum MOVETYPE {MOVE_NEIGHBOR, MOVE_OUT, MOVE_IN};
private:
	explicit CNavigation(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CNavigation(const CNavigation& rhs);
	virtual ~CNavigation(void) = default;
public:
	HRESULT Add_Cell(const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC, const _uint& iIndex);
	void Set_Currrent_Index(const _uint& iIndex) {
		m_iCurrentIdx = iIndex; }

public:
	HRESULT Ready_Navigation(const _tchar* pFilePath);
	HRESULT Ready_Navigation();

	HRESULT Reserve_Navigation(const _uint& iNumCell);
	HRESULT Ready_Neighbor_Cell(void);
	MOVETYPE Check_Navigation(const _vec3* pResultPos);
	void Render_Navigation(void);

public:
	void	Release_Navigation_Neighbor();
	void	Load_Navigation(CCell* pCell);

private:
	void Correct_Navigation(CCell* pCell);

public:
	_uint Picking_Navigation(const _vec3* PickPos);
	_uint Picking_Navigation(HWND hWnd);
	_uint Picking_Navigation(HWND hWnd, _vec3 & vGetPos);
	_bool Picking_Dist_Short(const _vec3 * PickPos, _vec3 & vGetPos);


public:	// Getter
	vector<CCell*>*	Get_VecCell() { return &m_vecCell; }
	CCell*			Get_CurVecCell() { return m_vecCell[m_iCurrentIdx]; }
	CCell*			Get_SelectCell(const _uint& iIdx) { return m_vecCell[iIdx]; }
	CCell*			Get_Neighbor(const _uint& iIdx, const _uint& iNeighbor);
	_uint&			Get_CurIndex() { return m_iCurrentIdx; }
	const _int&		Get_PickPoint(const _uint& iIdx, const _vec3* vPickPos);

	NAVIOPTION*		Get_NaviOption(const _uint& iIdx);
	NAVIOPTION*		Get_CurNaviOption();

public:	// Setter
	void	Set_CurLineColor(const _uint& iIdx, D3DXCOLOR dwColor);
	void	Set_CurIndex(const _uint& iIdx) { m_iCurrentIdx = iIdx; }
	void	Set_Change_SelectCell_Dot_Point(const _uint& iIdx, const _uint& iPoint, const _vec3* vChangePos);

	// Cell Option
	void	Set_Navi_AniIdx(const _uint& iIdx, const _uint& iAniIdx);
	void	Set_Navi_Move(const _uint& iIdx, _bool Move);
	void	Set_Navi_Packour(const _uint& iIdx, _bool Move);
	void	Set_Option(const _uint & iIdx, NAVIOPTION pOption);
public:	// Function
	_bool	Delete_SelectCell(const _uint& iIdx);
	void	ReSet_Index();

private:
	vector<CCell*>				m_vecCell;
	typedef vector<CCell*>		VECCELL;
private:
	_uint						m_iCurrentIdx = 0;
#ifndef _FINAL
public:
	void Debug_Render();
#endif
public:
	virtual CComponent* Clone(void);
	static CNavigation* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath);
	static CNavigation* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual _ulong Free(void);
};

END