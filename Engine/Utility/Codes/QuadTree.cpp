#include "..\Headers\QuadTree.h"
#include "Frustum.h"

USING(Engine)

CQuadTree::CQuadTree(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
#ifndef _FINAL
	Tag(L"CQuadTree", std::bind(&CQuadTree::Debug_Render, this));
#endif

}

HRESULT CQuadTree::Ready_QuadTree(const _vec3* pVertexPos, const _ulong& dwCntX, const _ulong& dwCntZ)
{
	m_dwCorner[CORNER_LT] = dwCntX * dwCntZ - dwCntX;
	m_dwCorner[CORNER_RT] = dwCntX * dwCntZ - 1;
	m_dwCorner[CORNER_LB] = 0;
	m_dwCorner[CORNER_RB] = dwCntX - 1;

	Make_QuadTree_Child(pVertexPos);

	Make_Neighbor();

	return NOERROR;
}

void CQuadTree::Culling(CFrustum* pFrustum, INDEX32* pIndices, _uint* pTriCnt, _vec3* pVertexPos, _matrix matWorld)
{
	// 현재 조사하는 쿼드트리는 절두체와 충돌해있어!!

	//if(nullptr == m_pChild[CHILD_LT]) // 마지막 쿼드트리였다면.
	if(true == isRender_LevelofDetail(pVertexPos)
		|| nullptr == m_pChild[CHILD_LT])
	{
		_bool		isDraw[4] = { true, true, true, true };

		if(m_pNeighbor[NEIGHBOR_LEFT])
			isDraw[0] = m_pNeighbor[NEIGHBOR_LEFT]->isRender_LevelofDetail(pVertexPos);
		if(m_pNeighbor[NEIGHBOR_TOP])
			isDraw[1] = m_pNeighbor[NEIGHBOR_TOP]->isRender_LevelofDetail(pVertexPos);
		if(m_pNeighbor[NEIGHBOR_RIGHT])
			isDraw[2] = m_pNeighbor[NEIGHBOR_RIGHT]->isRender_LevelofDetail(pVertexPos);
		if(m_pNeighbor[NEIGHBOR_BOTTOM])
			isDraw[3] = m_pNeighbor[NEIGHBOR_BOTTOM]->isRender_LevelofDetail(pVertexPos);

		_vec3		vPointPos[] =
		{
			pVertexPos[m_dwCorner[CORNER_LT]],
			pVertexPos[m_dwCorner[CORNER_RT]],
			pVertexPos[m_dwCorner[CORNER_RB]],
			pVertexPos[m_dwCorner[CORNER_LB]],
		};

		_bool		isIn[4] = {true};

		isIn[0] = pFrustum->isIn_Frustum_Local(matWorld, &vPointPos[CORNER_LT], 0.0f);
		isIn[1] = pFrustum->isIn_Frustum_Local(matWorld, &vPointPos[CORNER_RT], 0.0f);
		isIn[2] = pFrustum->isIn_Frustum_Local(matWorld, &vPointPos[CORNER_RB], 0.0f);
		isIn[3] = pFrustum->isIn_Frustum_Local(matWorld, &vPointPos[CORNER_LB], 0.0f);

		// 내 이웃들이 나와 같은 단계로 모두 그려지는 상황이다.
		// 크게 삼각형 두개로 그리낟.
		if(true == isDraw[NEIGHBOR_LEFT]
			&& true == isDraw[NEIGHBOR_TOP]
			&& true == isDraw[NEIGHBOR_RIGHT]
			&& true == isDraw[NEIGHBOR_BOTTOM])
		{
			// 오른쪽 위삼격을 그릴자
			if(true == isIn[0]
				|| true == isIn[1]
				|| true == isIn[2])
			{
				pIndices[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_RT];
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RB];
				++(*pTriCnt);
			}

			// 왼쪽 하단 삼가곃을 그릴지
			if(true == isIn[0]
				|| true == isIn[2]
				|| true == isIn[3])
			{
				pIndices[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_RB];
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_LB];
				++(*pTriCnt);
			}
			return;
		}

		_ulong		dwTC, dwRC, dwBC, dwLC;

		dwTC = _ulong((m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT]) * 0.5f);
		dwRC = _ulong((m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_RB]) * 0.5f);
		dwBC = _ulong((m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) * 0.5f);
		dwLC = _ulong((m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_LB]) * 0.5f);

		if(false == isDraw[NEIGHBOR_TOP])
		{
			if(true == isIn[0]
				|| true == isIn[1]
				|| true == isIn[2])
			{
				pIndices[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._1 = dwTC;
				pIndices[*pTriCnt]._2 = m_dwCenter;
				++(*pTriCnt);	

				pIndices[*pTriCnt]._0 = m_dwCenter;
				pIndices[*pTriCnt]._1 = dwTC;
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RT];
				++(*pTriCnt);
			}
		}
		else
		{
			if(true == isIn[0]
				|| true == isIn[1]
				|| true == isIn[2])
			{
				pIndices[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_RT];
				pIndices[*pTriCnt]._2 = m_dwCenter;
				++(*pTriCnt);
			}
		}

		if(false == isDraw[NEIGHBOR_RIGHT])
		{
			if(true == isIn[0]
				|| true == isIn[1]
				|| true == isIn[2])
			{
				pIndices[*pTriCnt]._0 = m_dwCorner[CORNER_RT];
				pIndices[*pTriCnt]._1 = dwRC;
				pIndices[*pTriCnt]._2 = m_dwCenter;
				++(*pTriCnt);

				pIndices[*pTriCnt]._0 = m_dwCenter;
				pIndices[*pTriCnt]._1 = dwRC;
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RB];
				++(*pTriCnt);
			}
		}
		else
		{
			if(true == isIn[0]
				|| true == isIn[1]
				|| true == isIn[2])
			{
				pIndices[*pTriCnt]._0 = m_dwCorner[CORNER_RT];
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_RB];
				pIndices[*pTriCnt]._2 = m_dwCenter;
				++(*pTriCnt);
			}
		}

		if(false == isDraw[NEIGHBOR_BOTTOM])
		{
			if(true == isIn[0]
				|| true == isIn[2]
				|| true == isIn[3])
			{
				pIndices[*pTriCnt]._0 = m_dwCorner[CORNER_RB];
				pIndices[*pTriCnt]._1 = dwBC;
				pIndices[*pTriCnt]._2 = m_dwCenter;
				++(*pTriCnt);

				pIndices[*pTriCnt]._0 = m_dwCenter;
				pIndices[*pTriCnt]._1 = dwBC;
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_LB];
				++(*pTriCnt);
			}
		}
		else
		{
			if(true == isIn[0]
				|| true == isIn[2]
				|| true == isIn[3])
			{
				pIndices[*pTriCnt]._0 = m_dwCorner[CORNER_RB];
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LB];
				pIndices[*pTriCnt]._2 = m_dwCenter;
				++(*pTriCnt);
			}
		}

		if(false == isDraw[NEIGHBOR_LEFT])
		{
			if(true == isIn[0]
				|| true == isIn[2]
				|| true == isIn[3])
			{
				pIndices[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._1 = m_dwCenter;
				pIndices[*pTriCnt]._2 = dwLC;
				++(*pTriCnt);

				pIndices[*pTriCnt]._0 = dwLC;
				pIndices[*pTriCnt]._1 = m_dwCenter;
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_LB];
				++(*pTriCnt);
			}
		}
		else
		{
			if(true == isIn[0]
				|| true == isIn[2]
				|| true == isIn[3])
			{
				pIndices[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._1 = m_dwCenter;
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_LB];
				++(*pTriCnt);			
			}
		}
		return;
	}		

	if(true == pFrustum->isIn_Frustum_Local(matWorld, &pVertexPos[m_dwCenter], m_fRadius))
	{			
		for(size_t i = 0; i < CHILD_END; ++i)
		{
			if(nullptr != m_pChild[i])
				m_pChild[i]->Culling(pFrustum, pIndices, pTriCnt, pVertexPos, matWorld);
		}
	}	
	
}

void CQuadTree::Make_QuadTree_Child(const _vec3* pVertexPos)
{
	if(m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_LT] <= 1)
		return;

	m_dwCenter = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RB]) >> 1;
	m_fRadius = D3DXVec3Length(&(pVertexPos[m_dwCorner[CORNER_LT]] - pVertexPos[m_dwCenter]));
	
	for (_uint i = 0; i < CHILD_END; ++i)
	{
		tuple<_ulong, _ulong, _ulong, _ulong>	Corner_Child;

		Corner_Child = Compute_ChildIndex_Corner(CQuadTree::CHILD(i));
		
		m_pChild[i] = CQuadTree::Create(m_pGraphicDev, get<CORNER_LT>(Corner_Child)
			, get<CORNER_RT>(Corner_Child), get<CORNER_RB>(Corner_Child), get<CORNER_LB>(Corner_Child));

		if(nullptr == m_pChild[i])
			return;

		m_pChild[i]->Make_QuadTree_Child(pVertexPos);
	}
}

void CQuadTree::Make_Neighbor(void)
{
	// 가장 작은 쿼드트리가 아닐때만 
	if(nullptr != m_pChild[CHILD_LT])
	{
		if(nullptr == m_pChild[CHILD_LT]->m_pChild[CHILD_LT])
			return;
		// 나의 자식들의 이웃을 셋팅한다.
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RT];
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LB];

		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LT];
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RB];

		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_LT];
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RB];

		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_RT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LB];

		if(nullptr != m_pNeighbor[NEIGHBOR_LEFT])
		{
			m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RT];
			m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RB];
		}

		if(nullptr != m_pNeighbor[NEIGHBOR_TOP])
		{
			m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_LB];
			m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_RB];
		}

		if(nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
		{
			m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LT];
			m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LB];
		}

		if(nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
		{
			m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_LT];
			m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_RT];
		}

		for(size_t i = 0; i < CHILD_END; ++i)
		{
			if(nullptr != m_pChild[i])
				m_pChild[i]->Make_Neighbor();
		}
	}
}

HRESULT CQuadTree::SetUp_CornerIndex(const _ulong & dwLT, const _ulong & dwRT, const _ulong & dwRB, const _ulong & dwLB)
{
	m_dwCorner[CORNER_LT] = dwLT;
	m_dwCorner[CORNER_RT] = dwRT;
	m_dwCorner[CORNER_RB] = dwRB;
	m_dwCorner[CORNER_LB] = dwLB;

	return NOERROR;
}

tuple<_ulong, _ulong, _ulong, _ulong> CQuadTree::Compute_ChildIndex_Corner(CHILD eType)
{
	_ulong		dwLT, dwRT, dwLB, dwRB;

	_ulong		dwLC = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_LB]) >> 1;
	_ulong		dwTC = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT]) >> 1;
	_ulong		dwRC = (m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_RB]) >> 1;
	_ulong		dwBC = (m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) >> 1;

	switch(eType)
	{
	case CHILD_LT:
		dwLT = m_dwCorner[CORNER_LT];
		dwRT = dwTC;
		dwLB = dwLC;
		dwRB = m_dwCenter;
		break;
	case CHILD_RT:
		dwLT = dwTC;
		dwRT = m_dwCorner[CORNER_RT];
		dwLB = m_dwCenter;
		dwRB = dwRC;
		break;
	case CHILD_LB:
		dwLT = dwLC;
		dwRT = m_dwCenter;
		dwLB = m_dwCorner[CORNER_LB];
		dwRB = dwBC;
		break;
	case CHILD_RB:
		dwLT = m_dwCenter;
		dwRT = dwRC;
		dwLB = dwBC;
		dwRB = m_dwCorner[CORNER_RB];
		break;
	}

	return tuple<_ulong, _ulong, _ulong, _ulong>(dwLT, dwRT, dwRB, dwLB);
}

_bool CQuadTree::isRender_LevelofDetail(const _vec3* pVertexPos)
{
	_matrix			matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3			vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(_vec3));

	_float			fDistance = D3DXVec3Length(&(pVertexPos[m_dwCenter] - vCamPos));

	_float			fWidth = D3DXVec3Length(&(pVertexPos[m_dwCorner[CORNER_RT]] - pVertexPos[m_dwCorner[CORNER_LT]]));

	if(fDistance * 0.2f > fWidth)
		return true;

	return false;	
}

CQuadTree * CQuadTree::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pVertexPos, const _ulong& dwCntX, const _ulong& dwCntZ)
{
	CQuadTree *	pInstance = new CQuadTree(pGraphicDev);

	if(FAILED(pInstance->Ready_QuadTree(pVertexPos, dwCntX, dwCntZ)))
	{
		MessageBox(0, L"CQuadTree Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CQuadTree * CQuadTree::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong & dwLT, const _ulong &dwRT, const _ulong & dwRB, const _ulong & dwLB)
{
	CQuadTree *	pInstance = new CQuadTree(pGraphicDev);

	if(FAILED(pInstance->SetUp_CornerIndex(dwLT, dwRT, dwRB, dwLB)))
		return nullptr;

	return pInstance;
}
#ifndef _FINAL
void CQuadTree::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CQuadTree");

}
#endif
_ulong CQuadTree::Free(void)
{
	Safe_Release(m_pGraphicDev);

	for(auto pChildQuadTree : m_pChild)
	{
		Safe_Release(pChildQuadTree);
	}

	return _ulong();
}
