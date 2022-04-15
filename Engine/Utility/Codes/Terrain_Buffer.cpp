#include "..\Headers\Terrain_Buffer.h"
#include "Frustum.h"
#include "QuadTree.h"

USING(Engine)

CTerrain_Buffer::CTerrain_Buffer(LPDIRECT3DDEVICE9	pGraphicDev)
	: CVIBuffer(pGraphicDev)

{
#ifndef _FINAL
	Tag(L"CTerrain_Buffer", std::bind(&CTerrain_Buffer::Debug_Render, this));
#endif

}

CTerrain_Buffer::CTerrain_Buffer(const CTerrain_Buffer & rhs)
	: CVIBuffer(rhs), 
	m_iMapWidth(rhs.m_iMapWidth), 
	m_iMapHeight(rhs.m_iMapHeight)
{

#ifndef _FINAL
	Tag(L"CTerrain_Buffer", std::bind(&CTerrain_Buffer::Debug_Render, this));
#endif

}


HRESULT CTerrain_Buffer::Ready_Buffer(const _tchar * pHeightBmp, const _float & fInterval)
{
	/*Load HeightMap*/
	HANDLE hFile;
	hFile = CreateFile(pHeightBmp, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
		goto exception;

	_ulong dwByte = 0;

	BITMAPFILEHEADER fh;
	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	if (0 == dwByte)
		goto exception;

	BITMAPINFOHEADER ih;
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);
	if (0 == dwByte)
		goto exception;

	_ulong PixelCnt = ih.biHeight * ih.biWidth;
	_ulong* Pixel = new _ulong[PixelCnt];

	ReadFile(hFile, Pixel, sizeof(_ulong)*PixelCnt, &dwByte, nullptr);
	if (0 == dwByte)
		goto exception;

	CloseHandle(hFile);


	/*set VertexCount*/
	const _ulong dwVtxCountX = ih.biWidth;
	const _ulong dwVtxCountZ = ih.biHeight;

	/*vertex parameters*/
	m_iVertexSize = sizeof(VTXTEXTBN);
	m_iVertexCnt = dwVtxCountX * dwVtxCountZ;
	m_dwVertexFVF = VTXTEXTBN::FVF;

	/*index parameters*/
	m_iIndexSize = sizeof(INDEX32);
	m_IndexFmt = D3DFMT_INDEX32;
	m_iTriCnt = int(((dwVtxCountX - 1) * (dwVtxCountZ - 1)) * 2);

	/*Set Member*/
	m_pVertexPos = new _vec3[m_iVertexCnt];
	m_iVertexCntX = ih.biWidth;
	m_iVertexCntZ = ih.biHeight;
	m_fVertexItv = fInterval;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		goto exception;

	if (nullptr == m_pIB || nullptr == m_pVB)
		goto exception;

	/*memory allocate done*/
	_long VertexBufferSize = dwVtxCountX*dwVtxCountZ;
	VTXTEXTBN*	pVertex = new VTXTEXTBN[VertexBufferSize];



	for (_ulong i = 0; i < dwVtxCountZ; ++i)
	{
		for (_ulong j = 0; j < dwVtxCountX; ++j)
		{
			_ulong dwIndex = i * dwVtxCountX + j;


			pVertex[dwIndex].vPosition = _vec3(j * fInterval
				, ((Pixel[dwIndex] & 0x000000ff)) / 10.f, i * fInterval);
		
			m_pVertexPos[dwIndex] = pVertex[dwIndex].vPosition;
			pVertex[dwIndex].vNormal = _vec3(0.0f, 0.f, 0.f);
			pVertex[dwIndex].vTexUV = _vec2(j / _float(ih.biWidth - 1) *1.f, i / _float(ih.biHeight - 1) * 1.f);

			pVertex[dwIndex].vTangent = _vec3(0.f, 0.f, 0.f);
			pVertex[dwIndex].vBinormal = _vec3(0.f, 0.f, 0.f);

			m_pVertexPos[dwIndex] = pVertex[dwIndex].vPosition;
		}
	}
	_long IndexBufferSize = (dwVtxCountX - 1)*(dwVtxCountZ - 1) * 2;

	INDEX32* pIndex = new INDEX32[IndexBufferSize];



	_uint		iIndex = 0;
	for (_uint i = 0; i < dwVtxCountZ - 1; ++i)
	{
		for (_uint j = 0; j < dwVtxCountX - 1; ++j)
		{
			_uint		iVtxIdx = i * dwVtxCountX + j;
			_uint		iLT, iRT, iLB, iRB;
			D3DXVECTOR4 vTangent;

			iLT = iVtxIdx + dwVtxCountX;
			iRT = iLT + 1;
			iLB = iVtxIdx;
			iRB = iLB + 1;

			pIndex[iIndex]._0 = iLT;
			pIndex[iIndex]._1 = iRT;
			pIndex[iIndex]._2 = iRB;

			_vec3 vPolygonNormal, v1, v2;

			v1 = pVertex[pIndex[iIndex]._2].vPosition - pVertex[pIndex[iIndex]._1].vPosition;
			v2 = pVertex[pIndex[iIndex]._1].vPosition - pVertex[pIndex[iIndex]._0].vPosition;
			D3DXVec3Cross(&vPolygonNormal, &v2, &v1);

			CalcTangentVector(pVertex[pIndex[iIndex]._0], pVertex[pIndex[iIndex]._1], pVertex[pIndex[iIndex]._2], vPolygonNormal);

			++iIndex;

			pIndex[iIndex]._0 = iLT;
			pIndex[iIndex]._1 = iRB;
			pIndex[iIndex]._2 = iLB;

			v1 = pVertex[pIndex[iIndex]._2].vPosition - pVertex[pIndex[iIndex]._1].vPosition;
			v2 = pVertex[pIndex[iIndex]._1].vPosition - pVertex[pIndex[iIndex]._0].vPosition;
			D3DXVec3Cross(&vPolygonNormal, &v2, &v1);

			CalcTangentVector(pVertex[pIndex[iIndex]._0], pVertex[pIndex[iIndex]._1], pVertex[pIndex[iIndex]._2], vPolygonNormal);

			++iIndex;
		}
	}

	for (size_t iIndex = 0; iIndex < VertexBufferSize; ++iIndex)
	{
		D3DXVec3Normalize(&pVertex[iIndex].vNormal, &pVertex[iIndex].vNormal);
		D3DXVec3Normalize(&pVertex[iIndex].vTangent, &pVertex[iIndex].vTangent);
		D3DXVec3Normalize(&pVertex[iIndex].vBinormal, &pVertex[iIndex].vBinormal);
	}

	VTXTEXTBN*			pVertexBuffer = nullptr;
	m_pVB->Lock(0, 0, (void**)&pVertexBuffer, 0);
	memcpy(pVertexBuffer, pVertex, sizeof(VTXTEXTBN)*VertexBufferSize);
	m_pVB->Unlock();

	INDEX32*			pIndexBuffer = nullptr;
	m_pIB->Lock(0, 0, (void**)&pIndexBuffer, 0);
	memcpy(pIndexBuffer, pIndex, sizeof(INDEX32)*IndexBufferSize);
	m_pIB->Unlock();

	Engine::Safe_Delete_Array(pVertex);
	Engine::Safe_Delete_Array(pIndex);
	Engine::Safe_Delete_Array(Pixel);


	//_vec3* TestLeakvec = new _vec3(0.f, 0.f, 0.f);
	
	return NOERROR;

exception:
	MSG_BOX("Terrain Ready Failed err:00923");
	return E_FAIL;
}

void CTerrain_Buffer::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

void CTerrain_Buffer::CalcTangentVector(VTXTEXTBN & pVertex1, VTXTEXTBN & pVertex2, VTXTEXTBN & pVertex3, _vec3 vNormal)
{
	_vec3 vTangent( 0.f, 0.f, 0.f );
	_vec3 vBinormal( 0.f, 0.f, 0.f );

	_float fS1 = pVertex1.vTexUV.x - pVertex2.vTexUV.x;
	_float fS2 = pVertex3.vTexUV.x - pVertex2.vTexUV.x;

	_float fT1 = pVertex1.vTexUV.y - pVertex2.vTexUV.y;
	_float fT2 = pVertex3.vTexUV.y - pVertex2.vTexUV.y;

	_vec3 vQ1 = pVertex1.vPosition - pVertex2.vPosition;
	_vec3 vQ2 = pVertex3.vPosition - pVertex2.vPosition;

	vTangent.x = fT2 * vQ1.x - fT1 * vQ2.x;
	vTangent.y = fT2 * vQ1.y - fT1 * vQ2.y;
	vTangent.z = fT2 * vQ1.z - fT1 * vQ2.z;

	D3DXVec3Cross( &vBinormal, &vTangent, &vNormal );

	pVertex1.vNormal += vNormal;
	pVertex1.vTangent += vTangent;
	pVertex1.vBinormal += vBinormal;

	pVertex2.vNormal += vNormal;
	pVertex2.vTangent += vTangent;
	pVertex2.vBinormal += vBinormal;

	pVertex3.vNormal += vNormal;
	pVertex3.vTangent += vTangent;
	pVertex3.vBinormal += vBinormal;

	//_vec3 vPos1, vPos2;
	//D3DXVECTOR2 uv1, uv2;
	//_vec3 tangent, binormal, normal = vNormal;
	//
	//// 1 0 2
	//vPos1 = pVertex2.vPosition - pVertex1.vPosition;
	//vPos2 = pVertex3.vPosition - pVertex1.vPosition;
	//
	//uv1 = pVertex2.vTexUV - pVertex1.vTexUV;
	//uv2 = pVertex3.vTexUV - pVertex1.vTexUV;
	//
	//float den;
	//
	//// Calculate the denominator of the tangent/binormal equation.
	//double tmpZeroCheck = (uv1.x * uv2.y - uv1.y * uv2.x);
	//
	//if (fabsl(tmpZeroCheck) < SMALL_NUMBER)    // almost equal to zero
	//{
	//	tangent.x = 1.0f;
	//	tangent.y = 0.0f;
	//	tangent.z = 0.0f;
	//
	//	binormal.x = 0.0f;
	//	binormal.y = 1.0f;
	//	binormal.z = 0.0f;
	//}
	//else
	//{
	//	den = float(1.0 / tmpZeroCheck);
	//	tangent = (uv2.y * vPos1 - uv1.y * vPos2) * den;
	//	binormal = (uv1.x * vPos2 - uv2.x * vPos1) * den;
	//}
	//
	//D3DXVec3Normalize(&tangent, &tangent);
	//D3DXVec3Normalize(&binormal, &binormal);
	//D3DXVec3Normalize(&normal, &normal);
	//
	//D3DXVec3Cross(&binormal, &binormal, &normal);
	//D3DXVec3Cross(&tangent, &tangent, &normal);
	//
	//D3DXVec3Normalize(&tangent, &tangent);
	//D3DXVec3Normalize(&binormal, &binormal);
	//
	//pVertex3.vNormal += normal;
	//pVertex3.vTangent.x += tangent.x;
	//pVertex3.vTangent.y += tangent.y;
	//pVertex3.vTangent.z += tangent.z;
	//pVertex3.vBinormal += binormal;
	//
	//pVertex2.vNormal += normal;
	//pVertex2.vTangent.x += tangent.x;
	//pVertex2.vTangent.y += tangent.y;
	//pVertex2.vTangent.z += tangent.z;
	//pVertex2.vBinormal += binormal;
	//
	//pVertex1.vNormal += normal;
	//pVertex1.vTangent.x += tangent.x;
	//pVertex1.vTangent.y += tangent.y;
	//pVertex1.vTangent.z += tangent.z;
	//pVertex1.vBinormal += binormal;
}

CComponent * CTerrain_Buffer::Clone(void)
{
	return new CTerrain_Buffer(*this);
}

CTerrain_Buffer * CTerrain_Buffer::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pHeightMap, const _float & fInterval)
{
	CTerrain_Buffer *	pInstance = new CTerrain_Buffer(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(pHeightMap, fInterval)))
	{
		MessageBox(0, L"CTerrain_Buffer Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}
#ifndef _FINAL
void CTerrain_Buffer::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CTerrain_Buffer");

}
#endif

_ulong CTerrain_Buffer::Free(void)
{
	return Engine::CVIBuffer::Free();
}
