#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

// �������� �����Ѵ�.
// ������ �������� �־�α� ���� �޸𸮵��(����)�� �����.

class DLL_EXPORT CRect_Texture final : public CVIBuffer
{
private:
	explicit CRect_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRect_Texture(const CRect_Texture& rhs);
	virtual ~CRect_Texture();
public:
	HRESULT Ready_Buffer(void);
	void Render_Buffer(void);
public:
	virtual CComponent* Clone(void) final;
public:
	static CRect_Texture* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void) final;

};

END

