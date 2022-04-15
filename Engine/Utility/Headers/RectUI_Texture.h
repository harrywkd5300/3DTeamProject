#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

// �������� �����Ѵ�.
// ������ �������� �־�α� ���� �޸𸮵��(����)�� �����.

class DLL_EXPORT CRectUI_Texture final : public CVIBuffer
{
private:
	explicit CRectUI_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRectUI_Texture(const CRectUI_Texture& rhs);
	virtual ~CRectUI_Texture();
public:
	HRESULT Ready_Buffer(void);
	void Render_Buffer(void);
public:
	virtual CComponent* Clone(void) final;
public:
	static CRectUI_Texture* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void) final;

};

END

