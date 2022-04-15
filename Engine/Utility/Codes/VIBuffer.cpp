#include "..\Headers\VIBuffer.h"

USING(Engine)

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CResources(pGraphicDev)
	, m_isClone(false)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_iVertexCnt(0)
	, m_iVertexSize(0)
	, m_dwVertexFVF(0)
	, m_iTriCnt(0)
	, m_pVertexPos(nullptr)
{
#ifndef _FINAL
	Tag(L"CVIBuffer", std::bind(&CVIBuffer::Debug_Render, this));
#endif

}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CResources(rhs)
	, m_isClone(true)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iVertexCnt(rhs.m_iVertexCnt)
	, m_iVertexSize(rhs.m_iVertexSize)
	, m_dwVertexFVF(rhs.m_dwVertexFVF)
	, m_iTriCnt(rhs.m_iTriCnt)
	, m_pVertexPos(rhs.m_pVertexPos)
	, m_iVertexCntX(rhs.m_iVertexCntX)
	, m_iVertexCntZ(rhs.m_iVertexCntZ)
	, m_fVertexItv(rhs.m_fVertexItv)
	, m_iIndexSize(rhs.m_iIndexSize)
{
	m_pVB->AddRef();
	m_pIB->AddRef();
#ifndef _FINAL
	Tag(L"CVIBuffer", std::bind(&CVIBuffer::Debug_Render, this));
#endif

}


CVIBuffer::~CVIBuffer()
{
}

const _vec3 * CVIBuffer::Get_VertexPos() const
{
	if(nullptr == m_pVertexPos)
		return nullptr;

	return m_pVertexPos;
}

tuple<_uint, _uint, _float> CVIBuffer::Get_VertexCntXZItv(void) const
{
	return tuple<_uint, _uint, _float>(m_iVertexCntX, m_iVertexCntZ, m_fVertexItv);
}

HRESULT CVIBuffer::Ready_Buffer(void)
{
	if (nullptr == m_pGraphicDev)
		return E_FAIL;

	// 버텏흐버퍼를 생성하고  m_pVB라는 녀석에게 버퍼를 대표할 수 있도록 한다.
	if (FAILED(m_pGraphicDev->CreateVertexBuffer(m_iVertexSize * m_iVertexCnt, 0, m_dwVertexFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
	{
		MSG_BOX("Create VertexBuffer Failed");
		return E_FAIL;
	}

	if (FAILED(m_pGraphicDev->CreateIndexBuffer(m_iIndexSize * m_iTriCnt, 0, m_IndexFmt, D3DPOOL_MANAGED, &m_pIB, nullptr)))
	{
		MSG_BOX("Create IndexBuffer Failed");
		return E_FAIL;
	}

	return NOERROR;
}

void CVIBuffer::Render_Buffer(void)
{
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_iVertexSize);
	m_pGraphicDev->SetFVF(m_dwVertexFVF);
	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVertexCnt, 0, m_iTriCnt);
}
#ifndef _FINAL
void CVIBuffer::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CVIBuffer");

}
#endif
_ulong CVIBuffer::Free(void)
{
	_ulong dwRefCnt = 0;

	if(false == m_isClone)
		Engine::Safe_Delete_Array(m_pVertexPos);

	dwRefCnt = Engine::Safe_Release(m_pVB);
	dwRefCnt = Engine::Safe_Release(m_pIB);

	return CResources::Free();
}
