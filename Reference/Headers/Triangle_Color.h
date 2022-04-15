#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

// �������� �����Ѵ�.
// ������ �������� �־�α� ���� �޸𸮵��(����)�� �����.

class DLL_EXPORT CTriangle_Color final : public CVIBuffer
{
private:
	explicit CTriangle_Color(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTriangle_Color(const CTriangle_Color& rhs);
	virtual ~CTriangle_Color();
public:
	HRESULT Ready_Buffer(void);
	void Render_Buffer(void);
public:
	virtual CComponent* Clone(void) final;
public:
	static CTriangle_Color* Create(LPDIRECT3DDEVICE9 pGraphicDev);
public:
#ifndef _FINAL
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void) final;

};

END

