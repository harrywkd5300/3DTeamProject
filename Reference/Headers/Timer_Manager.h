#pragma once

// For.타이머들을 여러개 보관하기위해 만들어진 클래스
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CTimer;
class DLL_EXPORT CTimer_Manager final : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)
private:
	explicit CTimer_Manager(void);
	virtual ~CTimer_Manager(void);
public:
	float Get_TimeDelta(const TCHAR* pTimerTag) const;
	//const CTimer* Get_Timer(const TCHAR* pTimerTag) const;
public: //추가하겠다.
	HRESULT Add_Timer(const TCHAR* pTimerTag, CTimer* pTimer);
	void Compute_Timer(const TCHAR* pTimerTag);
	//void Set_MainGameTimer(const TCHAR* pTimerTag);
	FORCEINLINE const CTimer* Get_MainGameTimer() { return m_MainGameTimer; }
private:
	map<const TCHAR*, CTimer*>			m_mapTimer;
	CTimer*	m_MainGameTimer = nullptr;
public:
	typedef map<const TCHAR*, CTimer*>	MAPTIMER;
private:
	CTimer* Find_Timer(const TCHAR* pTimerTag) const;
private:
	virtual _ulong Free(void) final;
};

END