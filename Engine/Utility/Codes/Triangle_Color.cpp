#include "..\Headers\Triangle_Color.h"

USING(Engine)

CTriangle_Color::CTriangle_Color(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CTriangle_Color", std::bind(&CTriangle_Color::Debug_Render, this));
#endif

}
CTriangle_Color::CTriangle_Color(const CTriangle_Color & rhs)
	: CVIBuffer(rhs)
{
#ifndef _FINAL
	Tag(L"CTriangle_Color", std::bind(&CTriangle_Color::Debug_Render, this));
#endif

}
CTriangle_Color::~CTriangle_Color()
{
}

HRESULT CTriangle_Color::Ready_Buffer(void)
{
	m_iVertexSize = sizeof(VTXCOL);
	m_iVertexCnt = 3;
	m_dwVertexFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iTriCnt = 1;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	if (nullptr == m_pVB)
		return E_FAIL;

	VTXCOL*			pVertex = nullptr;


	// Lock : �ٸ������尡 ������ �� ������ ���´�. (���� �����ϴ� ���̴ϱ�)
	//      : ����� ������ �ּҸ� ���´�.(���� �����Ϸ���)
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec3(0.f, 1.f, 0.f);
	pVertex[0].dwColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);

	pVertex[1].vPosition = _vec3(1.f, -1.f, 0.f);
	pVertex[1].dwColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);

	pVertex[2].vPosition = _vec3(-1.f, -1.f, 0.f);
	pVertex[2].dwColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);

	m_pVB->Unlock();

	INDEX16*			pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	m_pIB->Unlock();

	return NOERROR;
}

void CTriangle_Color::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

CComponent * CTriangle_Color::Clone(void)
{
	return new CTriangle_Color(*this);
}

CTriangle_Color * CTriangle_Color::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTriangle_Color *	pInstance = new CTriangle_Color(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		MessageBox(0, L"CTriangle_Color Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}
#ifndef _FINAL
void CTriangle_Color::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CShader");

}
#endif
_ulong CTriangle_Color::Free(void)
{
	return Engine::CVIBuffer::Free();	
}
