#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

// 정점들을 생성한다.
// 생성한 정점들을 넣어두기 위한 메모리덩어리(버퍼)를 만든다.

class DLL_EXPORT CRect_Color final : public CVIBuffer
{
private:
	explicit CRect_Color(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRect_Color(const CRect_Color& rhs);
	virtual ~CRect_Color();
public:
	HRESULT Ready_Buffer(void);
	void Render_Buffer(void);
public:
	virtual CComponent* Clone(void) final;
public:
	static CRect_Color* Create(LPDIRECT3DDEVICE9 pGraphicDev);

#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void) final;

};

END

