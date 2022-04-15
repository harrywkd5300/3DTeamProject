#include "..\Headers\Rect_Color.h"

USING(Engine)

CRect_Color::CRect_Color(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CRect_Color", std::bind(&CRect_Color::Debug_Render, this));
#endif

}
CRect_Color::CRect_Color(const CRect_Color & rhs)
	: CVIBuffer(rhs)
{
#ifndef _FINAL
	Tag(L"CRect_Color", std::bind(&CRect_Color::Debug_Render, this));
#endif

}
CRect_Color::~CRect_Color()
{
}

HRESULT CRect_Color::Ready_Buffer(void)
{
	m_iVertexSize = sizeof(VTXCOL);
	m_iVertexCnt = 4;
	m_dwVertexFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iTriCnt = 2;



	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	if (nullptr == m_pVB)
		return E_FAIL;

	VTXCOL*			pVertex = nullptr;


	// Lock : �ٸ������尡 ������ �� ������ ���´�. (���� �����ϴ� ���̴ϱ�)
	//      : ����� ������ �ּҸ� ���´�.(���� �����Ϸ���)
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec3(-1.f, 1.f, 0.f);
	pVertex[0].dwColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);

	pVertex[1].vPosition = _vec3(1.f, 1.f, 0.f);
	pVertex[1].dwColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);

	pVertex[2].vPosition = _vec3(1.f, -1.f, 0.f);
	pVertex[2].dwColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);

	pVertex[3].vPosition = _vec3(-1.f, -1.f, 0.f);
	pVertex[3].dwColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);

	m_pVB->Unlock();


	INDEX16*			pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return NOERROR;
}

void CRect_Color::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

CComponent * CRect_Color::Clone(void)
{
	return new CRect_Color(*this);
}

CRect_Color * CRect_Color::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRect_Color *	pInstance = new CRect_Color(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		MessageBox(0, L"CRect_Color Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}
#ifndef _FINAL
void CRect_Color::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CRect_Color");

}
#endif
_ulong CRect_Color::Free(void)
{
	return Engine::CVIBuffer::Free();	
}
