#include "..\Headers\Cell.h"
#include "Camera_Manager.h"

USING(Engine)

CCell::CCell(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	ZeroMemory(m_vPoint, sizeof(_vec3) * POINT_END);
	ZeroMemory(m_vDirection, sizeof(_vec3) * DIR_END);
	ZeroMemory(m_vNormal, sizeof(_vec3) * DIR_END);
	ZeroMemory(m_pNeightbor_Cell, sizeof(CCell*) * NEIGHBOR_END);

	m_pGraphicDev->AddRef();
#ifndef _FINAL
	Tag(L"CCell", std::bind(&CCell::Debug_Render, this));
#endif

}

CCell* CCell::Get_Neighbor(NEIGHBOR Neighbor)
{
	return m_pNeightbor_Cell[Neighbor];
}

void CCell::Set_Release_Neighbor()
{
	for (size_t i = NEIGHBOR_AB; i < CCell::NEIGHBOR_CA; ++i)
		m_pNeightbor_Cell[i] = nullptr;
}

void CCell::Set_Point(POINT ePoint, const _vec3 * vChangePos)
{
	m_vPoint[ePoint] = *vChangePos;
}

void CCell::Set_Option(NAVIOPTION pOption)
{
	m_pOption = pOption;
}

_bool CCell::Check_DistShort(const _vec3 * pSour, const _vec3 * pDest)
{
	_vec3 vCheck = _vec3(0.f, 0.f, 0.f);

	vCheck = *pSour - *pDest;

	_float fDist = D3DXVec3Length(&vCheck);

	if (fDist < 0.3f)
		return true;

	return false;
}

HRESULT CCell::Set_Neighbor(NEIGHBOR eNeighbor, CCell * pNeighbor)
{
	if (nullptr != m_pNeightbor_Cell[eNeighbor]
		|| nullptr == pNeighbor)
		return E_FAIL;

	m_pNeightbor_Cell[eNeighbor] = pNeighbor;

	return NOERROR;
}

void CCell::Set_DirectionNormal(const _vec3 * pDirAB, const _vec3 * pDirBC, const _vec3 * pDirCA)
{
	m_vDirection[DIR_AB] = *pDirAB;
	m_vDirection[DIR_BC] = *pDirBC;
	m_vDirection[DIR_CA] = *pDirCA;

	m_vNormal[DIR_AB] = _vec3(-m_vDirection[DIR_AB].z, m_vDirection[DIR_AB].y, m_vDirection[DIR_AB].x);
	m_vNormal[DIR_BC] = _vec3(-m_vDirection[DIR_BC].z, m_vDirection[DIR_BC].y, m_vDirection[DIR_BC].x);
	m_vNormal[DIR_CA] = _vec3(-m_vDirection[DIR_CA].z, m_vDirection[DIR_CA].y, m_vDirection[DIR_CA].x);

	for (size_t i = 0; i < DIR_END; i++)
		D3DXVec3Normalize(&m_vNormal[i], &m_vNormal[i]);
}

HRESULT CCell::Ready_Cell(const _vec3 * pPointA, const _vec3 * pPointB, const _vec3 * pPointC, const _uint& iIndex)
{
	ZeroMemory(&m_pOption, sizeof(NAVIOPTION));

	if (FAILED(D3DXCreateLine(m_pGraphicDev, &m_pLine)))
		return E_FAIL;

	m_pLineColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	m_vPoint[0] = *pPointA;
	m_vPoint[1] = *pPointB;
	m_vPoint[2] = *pPointC;

	m_iIndex = iIndex;

	/*
	_vec3				m_vDirection[DIR_END]; // 셀의 세변을 이루는 방향벡터.(시계방향)
	_vec3				m_vNormal[DIR_END];	// 방향벡터의 법선벡터
	*/
	m_vDirection[DIR_AB] = m_vPoint[POINT_B] - m_vPoint[POINT_A];
	m_vDirection[DIR_BC] = m_vPoint[POINT_C] - m_vPoint[POINT_B];
	m_vDirection[DIR_CA] = m_vPoint[POINT_A] - m_vPoint[POINT_C];

	m_vNormal[DIR_AB] = _vec3(m_vDirection[DIR_AB].z * -1.f, m_vDirection[DIR_AB].y, m_vDirection[DIR_AB].x);
	m_vNormal[DIR_BC] = _vec3(m_vDirection[DIR_BC].z * -1.f, m_vDirection[DIR_BC].y, m_vDirection[DIR_BC].x);
	m_vNormal[DIR_CA] = _vec3(m_vDirection[DIR_CA].z * -1.f, m_vDirection[DIR_CA].y, m_vDirection[DIR_CA].x);

	for (size_t i = 0; i < DIR_END; i++)
			D3DXVec3Normalize(&m_vNormal[i], &m_vNormal[i]);	

	//if (FAILED(Ready_Font()))
	//	return E_FAIL;
	
	return NOERROR;
}

HRESULT CCell::Ready_Font()
{
	//m_pText = C3D_Text::Create(m_pGraphicDev, L"Arial", 15);
	//if (nullptr == m_pText)
	//	return E_FAIL;

	return NOERROR;
}

_bool CCell::Check_Neighbor(const _vec3 * pSour, const _vec3 * pDest, CCell * pCell)
{
	if (Check_DistShort(&m_vPoint[POINT_A], pSour))
	{
		if (Check_DistShort(&m_vPoint[POINT_B] , pDest))
		{
			m_pNeightbor_Cell[NEIGHBOR_AB] = pCell;
			return true;
		}
		else if (Check_DistShort(&m_vPoint[POINT_C] , pDest))
		{
			m_pNeightbor_Cell[NEIGHBOR_CA] = pCell;
			return true;
		}		
	}

	if (Check_DistShort(&m_vPoint[POINT_B] , pSour))
	{
		if (Check_DistShort( &m_vPoint[POINT_A] , pDest))
		{
			m_pNeightbor_Cell[NEIGHBOR_AB] = pCell;
			return true;
		}
		else if (Check_DistShort( &m_vPoint[POINT_C] ,pDest))
		{
			m_pNeightbor_Cell[NEIGHBOR_BC] = pCell;
			return true;
		}
	}

	if (Check_DistShort( &m_vPoint[POINT_C] ,pSour ))
	{
		if (Check_DistShort ( &m_vPoint[POINT_A]  , pDest))
		{
			m_pNeightbor_Cell[NEIGHBOR_CA] = pCell;
			return true;
		}
		else if (Check_DistShort ( &m_vPoint[POINT_B]  ,pDest))
		{
			m_pNeightbor_Cell[NEIGHBOR_BC] = pCell;
			return true;
		}
	}
	
	
	return false;
}

CNavigation::MOVETYPE CCell::Check_Cell(const _vec3 * pResultPos, _uint* pCurrentIdx)
{
	for (size_t i = 0; i < DIR_END; i++)
	{
		_vec3	vSour = *pResultPos - m_vPoint[i];
		if (D3DXVec3Dot(D3DXVec3Normalize(&vSour, &vSour), &m_vNormal[i]) > 0)
		{
			if (nullptr == m_pNeightbor_Cell[i])
				return CNavigation::MOVE_OUT;
			else
			{
				if (false == m_pNeightbor_Cell[i]->Get_Option()->bMove)
				{
					return CNavigation::MOVE_OUT;
				}
				else
				{
					*pCurrentIdx = m_pNeightbor_Cell[i]->m_iIndex;
					return CNavigation::MOVE_NEIGHBOR;
				}
			}

		}
	}
	return CNavigation::MOVE_IN;
}

void CCell::Render_Cell(void)
{
#ifndef _FINAL

	_vec3		vPoint[] = {
		m_vPoint[0],
		m_vPoint[1],
		m_vPoint[2],
		m_vPoint[0] };

	_matrix		matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	for (size_t i = 0; i < 4; i++)
	{
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matView);
		if (vPoint[i].z < 0.0f)
			vPoint[i].z = 0.f;

		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matProj);
	}

	_matrix			matTmp;

	m_pLine->SetWidth(5.f);
	m_pLine->Begin();

	m_pLine->DrawTransform(vPoint, 4, D3DXMatrixIdentity(&matTmp), m_pLineColor);
	
	m_pLine->End();

	//Render_Text();

#endif

}

void CCell::Render_Text()
{
	//_tchar chIndex[16] = {0};
	//wsprintf(chIndex, L"Idx: %d", m_iIndex);

	//_vec3 vPositon = _vec3((m_vPoint[0].x + m_vPoint[1].x + m_vPoint[2].x) / 3.f,
	//					   (m_vPoint[0].y + m_vPoint[1].y + m_vPoint[2].y) / 3.f,
	//					   (m_vPoint[0].z + m_vPoint[1].z + m_vPoint[2].z) / 3.f);

	//_vec3 vScale = _vec3(1.f, 1.f, 1.f);

	//CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurrentCamera();
	//_matrix matCamWorld = pCamera->Get_WorldMatrix();

	////m_pText->Draw_Text_3D_BillBoard(chIndex, matCamWorld, vScale, vPositon);
	//m_pText->Draw_Text_3D(chIndex);
}

CCell * CCell::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 * pPointA, const _vec3 * pPointB, const _vec3 * pPointC, const _uint& iIndex)
{
	CCell *		pInstance = new CCell(pGraphicDev);

	if (FAILED(pInstance->Ready_Cell(pPointA, pPointB, pPointC, iIndex)))
	{
		MSG_BOX("CCell Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CCell::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CCell");
}
#endif
_ulong CCell::Free(void)
{
	Safe_Release(m_pLine);
	Safe_Release(m_pGraphicDev);
	//Safe_Release(m_pText);

	return _ulong();
}
