#pragma once

#include "Resources.h"

BEGIN(Engine)

// 버텍스, 인덱스버퍼클래스들의 부모
// 각각의 공통된 기능을 모아둔다.

class DLL_EXPORT CVIBuffer : public CResources
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();
public:
	const _vec3* Get_VertexPos() const;
	tuple<_uint, _uint, _float> Get_VertexCntXZItv(void) const;

	_uint Get_TriCnt() { return m_iTriCnt; }
	_uint Get_IndexSize() { return m_iIndexSize; }
	LPDIRECT3DINDEXBUFFER9	Get_IndexBuffer() { return m_pIB; }

public:
	HRESULT Ready_Buffer(void);
	void Render_Buffer(void);	
protected:
	_bool							m_isClone = false;
protected:
	LPDIRECT3DVERTEXBUFFER9			m_pVB = nullptr;
	_uint							m_iVertexSize = 0;
	_uint							m_iVertexCntX = 0;
	_uint							m_iVertexCntZ = 0;
	_uint							m_iVertexCnt = 0;
	_ulong							m_dwVertexFVF = 0;
	_float							m_fVertexItv = 0;

	
protected:
	LPDIRECT3DINDEXBUFFER9			m_pIB = nullptr;
	_uint							m_iIndexSize;
	_uint							m_iTriCnt = 0;
	D3DFORMAT						m_IndexFmt = D3DFMT_UNKNOWN;
protected:
	_vec3*							m_pVertexPos = nullptr;

public:
	virtual CComponent* Clone(void) = 0;
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) override ;	
};

END