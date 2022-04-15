#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Timer.h"

#include "Random.h"
#include "String_Manager.h"
#include "Input_Device.h"
#include "Mouse.h"
#include "Debug_Manager.h"

BEGIN(Engine)

class DLL_EXPORT CComponent abstract : public CBase
{
public:
	enum COM_TYPE {TYPE_STATIC, TYPE_DYNAMIC, TYPE_END};
protected:	
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent();
public:
	virtual void Update_Component(const _float& fTimeDelta) { }
	virtual void LastUpdate_Component(const _float& fTimeDelta) { }
public:
	virtual CComponent* Clone(void) = 0;
protected:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
protected:
	_bool						m_isClone = false;
	const CTimer*				m_pTimer = nullptr;
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) override;

};

END