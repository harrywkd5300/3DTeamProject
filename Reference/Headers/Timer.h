#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class DLL_EXPORT CTimer : public CBase
{
private:
	explicit CTimer(void);
	virtual ~CTimer(void);
public:
	FORCEINLINE float Get_TimeDelta(void) const {
		return m_fTimeDelta;}
public:
	HRESULT Initialize(void);
private:
	LARGE_INTEGER 			m_FrameTime;
	LARGE_INTEGER 			m_FixTime;
	LARGE_INTEGER 			m_LastTime;
	LARGE_INTEGER 			m_CpuTick;
private:
	float					m_fTimeDelta;
public:
	void Compute_Timer(void); // ���� Ÿ�ӵ�Ÿ������Ѵ�. // �������Ӹ��� ȣ��

public: // ������ ���õ� �Լ�.
	static CTimer* Create(void);
private:
	virtual _ulong Free(void) final;
};

END
