#include "Timer_Manager.h"
#include "Timer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager(void)
{
	m_MainGameTimer = CTimer::Create();
	m_mapTimer.insert(MAPTIMER::value_type(L"Timer_60", m_MainGameTimer));
}

CTimer_Manager::~CTimer_Manager(void)
{
}

float CTimer_Manager::Get_TimeDelta(const TCHAR* pTimerTag) const
{
	CTimer*			pTimer = nullptr;

	pTimer = Find_Timer(pTimerTag);

	if(nullptr == pTimer)
		return 0.0f;

	return pTimer->Get_TimeDelta();
}


HRESULT CTimer_Manager::Add_Timer(const TCHAR* pTimerTag, CTimer* pTimer)
{
	if (nullptr != Find_Timer(pTimerTag))
		return NOERROR;
		//return E_FAIL;

	m_mapTimer.insert(MAPTIMER::value_type(pTimerTag, pTimer));
	
	return NOERROR;
}

void CTimer_Manager::Compute_Timer(const TCHAR* pTimerTag)
{
	CTimer*			pTimer = NULL;

	pTimer = Find_Timer(pTimerTag);

	if(NULL == pTimer)
		return;

	pTimer->Compute_Timer();
}

CTimer* CTimer_Manager::Find_Timer(const TCHAR* pTimerTag) const
{	
	//auto iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CFind_Tag(pTimerTag));
	auto iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), [&](const MAPTIMER::value_type& Pair) {return !lstrcmp(pTimerTag, Pair.first); });

	if(iter == m_mapTimer.end())
		return NULL;

	return iter->second;
}

_ulong CTimer_Manager::Free(void)
{
	//for (auto& Pair	: m_mapTimer)	
	//	Safe_Release(Pair.second);
	//m_mapTimer.clear();
	
	for_each(m_mapTimer.begin(), m_mapTimer.end(), CRelease_Pair());
	m_mapTimer.clear();

	/*for_each(m_mapTimer.begin(), m_mapTimer.end()
		, [](map<const _tchar*, CTimer*>::value_type& Pair) 
	{ Engine::Safe_Release(Pair.second);});
	m_mapTimer.clear();*/

	return 0;
	
}
