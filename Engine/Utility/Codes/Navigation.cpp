#include "..\Headers\Navigation.h"
#include "Cell.h"

USING(Engine)

CNavigation::CNavigation(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
#ifndef _FINAL
	Tag(L"CNavigation", std::bind(&CNavigation::Debug_Render, this));
#endif

}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_vecCell(rhs.m_vecCell)
{
	_uint iSize = _uint(m_vecCell.size());

	for (size_t i = 0; i < iSize; i++)
		m_vecCell[i]->AddRef();

#ifndef _FINAL
	Tag(L"CNavigation", std::bind(&CNavigation::Debug_Render, this));
#endif
	
}	

HRESULT CNavigation::Add_Cell(const _vec3 * pPointA, const _vec3 * pPointB, const _vec3 * pPointC, const _uint& iIndex)
{
	CCell*		pCell = CCell::Create(m_pGraphicDev, pPointA, pPointB, pPointC, iIndex);

	if (nullptr == pCell)
		return E_FAIL;

	Correct_Navigation(pCell);

	m_vecCell.push_back(pCell);
	
	return NOERROR;
}

HRESULT CNavigation::Ready_Navigation(const _tchar* pFilePath)
{
	HANDLE		hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong			dwByte = 0;

	_ulong			dwNumCell = 0;

	ReadFile(hFile, &dwNumCell, sizeof(_ulong), &dwByte, nullptr);

	Reserve_Navigation(dwNumCell);

	for (_uint i = 0; i < dwNumCell; i++)
	{
		_vec3			vPoint[3];

		ReadFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			return E_FAIL;
		
		Add_Cell(vPoint, vPoint + 1, vPoint + 2, i);
	}

	CloseHandle(hFile);	 

	// 이웃셀들에대한 정보를 셋팅하는 작업.
	Ready_Neighbor_Cell();


	return NOERROR;
}

HRESULT CNavigation::Ready_Navigation()
{
	return NOERROR;
}

HRESULT CNavigation::Reserve_Navigation(const _uint & iNumCell)
{
	if (false == m_vecCell.empty())
		return E_FAIL;

	m_vecCell.reserve(iNumCell);

	return NOERROR;
}

HRESULT CNavigation::Ready_Neighbor_Cell(void)
{
	_uint			iSize = _uint(m_vecCell.size());

	for (size_t i = 0; i < iSize; i++)
	{
		for (size_t j = 0; j < iSize; j++)
		{
			if (m_vecCell[i] == m_vecCell[j])
				continue;

			// Check_Neighbor(이웃에 유무에 대한 체크를 하고 멤버에 대입하낟)
			if (true == m_vecCell[j]->Check_Neighbor(m_vecCell[i]->Get_Point(CCell::POINT_A), m_vecCell[i]->Get_Point(CCell::POINT_B), m_vecCell[i]))			
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j]);			

			else if (true == m_vecCell[j]->Check_Neighbor(m_vecCell[i]->Get_Point(CCell::POINT_B), m_vecCell[i]->Get_Point(CCell::POINT_C), m_vecCell[i]))
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j]);

			else if (true == m_vecCell[j]->Check_Neighbor(m_vecCell[i]->Get_Point(CCell::POINT_C), m_vecCell[i]->Get_Point(CCell::POINT_A), m_vecCell[i]))
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j]);

		}
	}
	return NOERROR;
}

Engine::CNavigation::MOVETYPE CNavigation::Check_Navigation(const _vec3 * pResultPos)
{
	return m_vecCell[m_iCurrentIdx]->Check_Cell(pResultPos, &m_iCurrentIdx);
}

void CNavigation::Render_Navigation(void)
{
	if (m_vecCell.empty())
		return;

	for (size_t i = 0; i < m_vecCell.size(); i++)
	{
		if (nullptr == m_vecCell[i])
			continue;

		m_vecCell[i]->Render_Cell();
	}
}

void CNavigation::Release_Navigation_Neighbor()
{
	_uint			iSize = _uint(m_vecCell.size());

	for (size_t i = 0; i < iSize; i++)
	{
		for (size_t j = 0; j < iSize; j++)
		{
			if (m_vecCell[i] == m_vecCell[j])
				continue;

			// Check_Neighbor(이웃에 유무에 대한 체크를 하고 멤버에 대입하낟)
			m_vecCell[j]->Set_Release_Neighbor();

		}
	}

	return;
}

void CNavigation::Load_Navigation(CCell * pCell)
{
	m_vecCell.push_back(pCell);
}

void CNavigation::Correct_Navigation(CCell * pCell)
{
	_vec3 Point[3] = { *pCell->Get_Point(CCell::POINT_A), *pCell->Get_Point(CCell::POINT_B), *pCell->Get_Point(CCell::POINT_C) };

	_vec3 Normal[3] = { *pCell->Get_NormalVec(CCell::DIR_AB), *pCell->Get_NormalVec(CCell::DIR_BC), *pCell->Get_NormalVec(CCell::DIR_CA) };

	_vec3 vCompairPos = _vec3((Point[0].x + Point[1].x + Point[2].x) / 3.f,
		(Point[0].y + Point[1].y + Point[2].y) / 3.f,
		(Point[0].z + Point[1].z + Point[2].z) / 3.f);

	_vec3 ObjectDir[3];
	ObjectDir[0] = vCompairPos - Point[0];
	ObjectDir[1] = vCompairPos - Point[1];
	ObjectDir[2] = vCompairPos - Point[2];

	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3Normalize(&ObjectDir[i], &ObjectDir[i]);

		float fDotResult = D3DXVec3Dot(&ObjectDir[i], &Normal[i]);

		if (fDotResult > 0.f)
		{
			_vec3 vDir[CCell::DIR_END];
			vDir[CCell::DIR_AB] = *pCell->Get_Point(CCell::POINT_A) - *pCell->Get_Point(CCell::POINT_B);
			vDir[CCell::DIR_BC] = *pCell->Get_Point(CCell::POINT_B) - *pCell->Get_Point(CCell::POINT_C);
			vDir[CCell::DIR_CA] = *pCell->Get_Point(CCell::POINT_C) - *pCell->Get_Point(CCell::POINT_A);

			pCell->Set_DirectionNormal(&vDir[CCell::DIR_AB], &vDir[CCell::DIR_BC], &vDir[CCell::DIR_CA]);
			return;
		}
	}
	return;
}

_uint CNavigation::Picking_Navigation(const _vec3 * PickPos)
{
	if (m_vecCell.empty())
		return -1;

	for (size_t i = 0; i < m_vecCell.size(); ++i)
	{
		_vec3 ObjectDir[3];

		ObjectDir[0] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_A);
		ObjectDir[1] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_B);
		ObjectDir[2] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_C);

		_vec3 Normal[CCell::DIR_END];
		Normal[CCell::DIR_AB] = *m_vecCell[i]->Get_NormalVec(CCell::DIR_AB);
		Normal[CCell::DIR_BC] = *m_vecCell[i]->Get_NormalVec(CCell::DIR_BC);
		Normal[CCell::DIR_CA] = *m_vecCell[i]->Get_NormalVec(CCell::DIR_CA);

		for (int j = 0; j < 3; ++j)
		{
			D3DXVec3Normalize(&ObjectDir[j], &ObjectDir[j]);

			float fDotResult = D3DXVec3Dot(&ObjectDir[j], &Normal[j]);

			if (fDotResult > 0.f)
				break;

			if (2 == j)
			{
				m_vecCell[i]->Set_LineColor(D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
				return i;
			}
		}
	}

	return -1;
}

_uint CNavigation::Picking_Navigation(HWND hWnd)
{
	if ( nullptr == m_pGraphicDev )
		return -1;

	POINT			ptMouse;

	// In.Screen Space
	GetCursorPos(&ptMouse);

	// In.Client Space(Viewport Space)
	ScreenToClient(hWnd, &ptMouse);

	_vec3			vMousePos;

	// In.Projection Space
	// Compute BackBufferSize
	D3DVIEWPORT9		ViewPort;
	m_pGraphicDev->GetViewport(&ViewPort);
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.0f;

	// In.View Space
	_matrix			matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3			vRay, vPivot;
	vPivot = _vec3(0.0f, 0.0f, 0.0f);
	vRay = vMousePos - vPivot;

	// In.World Space
	_matrix			matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	D3DXVec3TransformCoord(&vPivot, &vPivot, &matView);
	D3DXVec3TransformNormal(&vRay, &vRay, &matView);

	_float		fU = 0.f, fV = 0.f, fDist = 0.f;

	for (size_t i = 0; i < m_vecCell.size(); ++i)
	{
		if (nullptr == m_vecCell[i])
			continue;

		_vec3 ObjectDir[3];

		ObjectDir[0] = *m_vecCell[i]->Get_Point(CCell::POINT_A);
		ObjectDir[1] = *m_vecCell[i]->Get_Point(CCell::POINT_B);
		ObjectDir[2] = *m_vecCell[i]->Get_Point(CCell::POINT_C);

		if (TRUE == D3DXIntersectTri(&ObjectDir[0], &ObjectDir[1], &ObjectDir[2], &vPivot, &vRay , &fU, &fV, &fDist))
		{
			m_vecCell[i]->Set_LineColor(D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
			return i;
		}
	}

	return -1;
}

_uint CNavigation::Picking_Navigation(HWND hWnd, _vec3 & vGetPos)
{
	if (nullptr == m_pGraphicDev)
		return -1;

	POINT			ptMouse;

	// In.Screen Space
	GetCursorPos(&ptMouse);

	// In.Client Space(Viewport Space)
	ScreenToClient(hWnd, &ptMouse);

	_vec3			vMousePos;

	// In.Projection Space
	// Compute BackBufferSize
	D3DVIEWPORT9		ViewPort;
	m_pGraphicDev->GetViewport(&ViewPort);
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.0f;

	// In.View Space
	_matrix			matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3			vRay, vPivot;
	vPivot = _vec3(0.0f, 0.0f, 0.0f);
	vRay = vMousePos - vPivot;

	// In.World Space
	_matrix			matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	D3DXVec3TransformCoord(&vPivot, &vPivot, &matView);
	D3DXVec3TransformNormal(&vRay, &vRay, &matView);

	_float		fU = 0.f, fV = 0.f, fDist = 0.f;

	for (size_t i = 0; i < m_vecCell.size(); ++i)
	{
		if (nullptr == m_vecCell[i])
			continue;

		_vec3 ObjectDir[3];

		ObjectDir[0] = *m_vecCell[i]->Get_Point(CCell::POINT_A);
		ObjectDir[1] = *m_vecCell[i]->Get_Point(CCell::POINT_B);
		ObjectDir[2] = *m_vecCell[i]->Get_Point(CCell::POINT_C);

		if (TRUE == D3DXIntersectTri(&ObjectDir[0], &ObjectDir[1], &ObjectDir[2], &vPivot, &vRay, &fU, &fV, &fDist))
		{
			vGetPos = vPivot + vRay * fDist;
			m_vecCell[i]->Set_LineColor(D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
			return i;
		}
	}

	return -1;
}

_bool CNavigation::Picking_Dist_Short(const _vec3 * PickPos, _vec3 & vGetPos)
{
	if (m_vecCell.empty())
		return false;

	for (size_t i = 0; i < m_vecCell.size(); ++i)
	{
		_vec3 ObjectDir[3];
		ObjectDir[0] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_A);
		ObjectDir[1] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_B);
		ObjectDir[2] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_C);

		_float fDist[3];

		fDist[0] = D3DXVec3Length(&ObjectDir[0]);
		if (fDist[0] < 0.4f)
		{
			vGetPos = *m_vecCell[i]->Get_Point(CCell::POINT_A);
			return true;
		}
		fDist[1] = D3DXVec3Length(&ObjectDir[1]);
		if (fDist[1] < 0.4f)
		{
			vGetPos = *m_vecCell[i]->Get_Point(CCell::POINT_B);
			return true;
		}
		fDist[2] = D3DXVec3Length(&ObjectDir[2]);
		if (fDist[2] < 0.4f)
		{
			vGetPos = *m_vecCell[i]->Get_Point(CCell::POINT_C);
			return true;
		}
	}

	return false;
}


CCell * CNavigation::Get_Neighbor(const _uint & iIdx, const _uint & iNeighbor)
{
	CCell::NEIGHBOR eType = CCell::NEIGHBOR_AB;

	if (0 == iNeighbor)
		eType = CCell::NEIGHBOR_AB;
	else if (1 == iNeighbor)
		eType = CCell::NEIGHBOR_BC;
	else if (2 == iNeighbor)
		eType = CCell::NEIGHBOR_CA;
	else
		return nullptr;

	return m_vecCell[iIdx]->Get_Neighbor(eType);
}

const _int & CNavigation::Get_PickPoint(const _uint& iIdx, const _vec3 * vPickPos)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	_vec3 ObjectDir[3];
	ObjectDir[0] = *vPickPos - *m_vecCell[iIdx]->Get_Point(CCell::POINT_A);
	ObjectDir[1] = *vPickPos - *m_vecCell[iIdx]->Get_Point(CCell::POINT_B);
	ObjectDir[2] = *vPickPos - *m_vecCell[iIdx]->Get_Point(CCell::POINT_C);

	_float fDist[3];

	fDist[0] = D3DXVec3Length(&ObjectDir[0]);
	fDist[1] = D3DXVec3Length(&ObjectDir[1]);
	fDist[2] = D3DXVec3Length(&ObjectDir[2]);

	_float fSortDist[3] = { fDist[0], fDist[1], fDist[2] };

	_uint i = 0, j = 0; 
	_float temp = 0;

	for (j = 0; j < 3; j++)
	{
		for (i = 0; i< 2 ; i++)
		{
			if (fSortDist[i]>fSortDist[i + 1])
			{
				temp = fSortDist[i];
				fSortDist[i] = fSortDist[i + 1];
				fSortDist[i + 1] = temp;
			}
		}
	}

	for (size_t i = 0; i < 3; ++i)
	{
		if (fSortDist[0] == fDist[i])
			return i;
	}

	return -1;
}

NAVIOPTION * CNavigation::Get_NaviOption(const _uint & iIdx)
{
	if (nullptr == m_vecCell[iIdx])
		return nullptr;

	return m_vecCell[iIdx]->Get_Option();
}

NAVIOPTION * CNavigation::Get_CurNaviOption()
{
	if (nullptr == m_vecCell[m_iCurrentIdx])
		return nullptr;

	return m_vecCell[m_iCurrentIdx]->Get_Option();
}

void CNavigation::Set_CurLineColor(const _uint & iIdx, D3DXCOLOR dwColor)
{
	if (nullptr == m_vecCell[iIdx])
		return;

	m_vecCell[iIdx]->Set_LineColor(dwColor);
}

void CNavigation::Set_Change_SelectCell_Dot_Point(const _uint & iIdx, const _uint & iPoint, const _vec3 * vChangePos)
{
	if (nullptr == m_vecCell[iIdx])
		return;

	if( 0 == iPoint )
		m_vecCell[iIdx]->Set_Point(CCell::POINT_A, vChangePos);
	else if( 1 ==  iPoint )
		m_vecCell[iIdx]->Set_Point(CCell::POINT_B, vChangePos);
	else if (2 == iPoint)
		m_vecCell[iIdx]->Set_Point(CCell::POINT_C, vChangePos);
}

void CNavigation::Set_Navi_AniIdx(const _uint & iIdx, const _uint & iAniIdx)
{
	if (nullptr == m_vecCell[iIdx])
		return;

	m_vecCell[iIdx]->Set_Optione_AniIdx(iAniIdx);
}

void CNavigation::Set_Navi_Move(const _uint & iIdx, _bool Move)
{
	if (nullptr == m_vecCell[iIdx])
		return;

	m_vecCell[iIdx]->Set_Optione_Move(Move);
}

void CNavigation::Set_Navi_Packour(const _uint & iIdx, _bool Move)
{
	if (nullptr == m_vecCell[iIdx])
		return;

	m_vecCell[iIdx]->Set_Optione_Packour(Move);
}

void CNavigation::Set_Option(const _uint & iIdx, NAVIOPTION pOption)
{
	if (nullptr == m_vecCell[iIdx])
		return;

	m_vecCell[iIdx]->Set_Option(pOption);
}

_bool CNavigation::Delete_SelectCell(const _uint & iIdx)
{
	Safe_Release(m_vecCell[iIdx]);
	m_vecCell.erase(m_vecCell.begin() + iIdx);

	ReSet_Index();

	return true;
}

void CNavigation::ReSet_Index()
{
	_uint iIdx = 0;
	for (auto iter : m_vecCell)
	{
		m_vecCell[iIdx]->Set_Index(iIdx);
		++iIdx;
	}
}

#ifndef _FINAL
void CNavigation::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CNavigation");
}
#endif
CComponent * CNavigation::Clone(void)
{
	return new CNavigation(*this);
}

CNavigation * CNavigation::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath)
{
	CNavigation *		pInstance = new CNavigation(pGraphic_Device);

	if (FAILED(pInstance->Ready_Navigation(pFilePath)))
	{
		MSG_BOX("CNavigation Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CNavigation * CNavigation::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CNavigation *		pInstance = new CNavigation(pGraphic_Device);

	if (FAILED(pInstance->Ready_Navigation()))
	{
		MSG_BOX("CNavigation Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CNavigation::Free(void)
{
	const _uint& iSize = _uint(m_vecCell.size());

	for (size_t i = 0; i < iSize; i++)
	{
		Safe_Release(m_vecCell[i]);
	}
	m_vecCell.clear();

	CComponent::Free();

	return 0;	
}
