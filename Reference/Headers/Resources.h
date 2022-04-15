#pragma once

#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)

class DLL_EXPORT CResources : public CComponent
{
protected:
	explicit CResources(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CResources(const CResources& rhs);
	virtual ~CResources();
public:
	virtual CComponent* Clone(void) = 0;
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) override;
};

END